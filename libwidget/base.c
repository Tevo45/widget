#include <u.h>
#include <libc.h>
#include <draw.h>
#include <thread.h>
#include <mouse.h>
#include <keyboard.h>
#include <String.h>

#include <widget.h>
#include "w-internal.h"

void (*werror)(char*, ...) = sysfatal;

int
nextid(void)
{
	static int curid;
	return curid++;
}

void
handlemenu(Widgetctl *ctl, Menu *menu, int button)
{
	Menumsg msg =
	{
		.menu	= menu,
		.button	= button,
	};

	msg.hit = menuhit(button, ctl->mouse, menu, ctl->image->screen);

	redrawwctl(ctl);

	send(ctl->menuc, &msg);
}

void
widgetmain(Widgetctl *ctl)
{
	Mouse mouse;
	Rune rune;

	Alt chans[] =
	{
		{ ctl->mouse->c,		&mouse,	CHANRCV },
		{ ctl->kbd->c,			&rune,	CHANRCV },

		{ nil,					nil,	CHANEND }
	};

	enum
	{
		MOUSE, KEYBOARD
	};

	threadsetname("widgetmain");

	for(;;)
	{
		while(ctl->root == nil)
			yield();

		switch(alt(chans))
		{
		case MOUSE:
			/* 
			 * FIXME if the user clicks once, the widget handles it, then
			 * the user drags and the widget claims to not have handled it
			 * a menu will be shown for that button (if there's one), but 
			 * it probably shouldn't
			 */
			if(!mouseevent(ctl->root, ctl->image, ctl->image->r, mouse, ctl->c))
			{
				if((mouse.buttons & M_LEFT) && ctl->left != nil)
				{
					handlemenu(ctl, ctl->left, M_LEFT);
					break;
				}

				if((mouse.buttons & M_MIDDLE) && ctl->middle != nil)
				{
					handlemenu(ctl, ctl->middle, M_MIDDLE);
					break;
				}

				if((mouse.buttons & M_RIGHT) && ctl->right != nil)
				{
					handlemenu(ctl, ctl->right, M_RIGHT);
					break;
				}

				if(ctl->flags & FORWARD_MOUSE)
					send(ctl->mousec, &mouse);
			}
			break;
		case KEYBOARD:
			if(!kbdevent(ctl->root, ctl->image, ctl->image->r, rune, ctl->c)
				&& ctl->flags & FORWARD_KBD)
				send(ctl->kbdc, &rune);
			break;
		}
		flushimage(ctl->image->display, 1);
	}
}

Widgetctl*
initwidget(Image *img, Keyboardctl *kbd, Mousectl *mouse, Widget *root, int flags)
{
	Widgetctl *ctl;

	if((ctl = mallocz(sizeof(*ctl), 1)) == nil)
		return nil;

	if(mouse == nil)
		if((mouse = initmouse(nil, img)) == nil)
		{
			free(ctl);
			return nil;
		}
		else
			ctl->pflags |= OURMOUSE;

	if(kbd == nil)
		if((kbd = initkeyboard(nil)) == nil)
		{
			free(ctl);
			closemouse(mouse);
			return nil;
		}
		else
			ctl->pflags |= OURKBD;

	ctl->image = img;
	ctl->mouse = mouse;
	ctl->root = root;
	ctl->kbd = kbd;
	ctl->c = chancreate(sizeof(Widgetmsg*), 16);
	ctl->kbdc = chancreate(sizeof(Rune), 20);
	ctl->menuc = chancreate(sizeof(Menumsg), 16);
	ctl->mousec = chancreate(sizeof(Mouse), 16);
	ctl->resizec = mouse->resizec;
	ctl->flags = flags;

	ctl->wthread = threadcreate((void(*)(void*))widgetmain, ctl, 16384);

	redrawwidget(root, img, img->r);
	flushimage(img->display, 1);

	return ctl;
}

void
closewidget(Widgetctl *ctl)
{
	chanclose(ctl->c);
	chanclose(ctl->kbdc);
	chanclose(ctl->menuc);
	chanclose(ctl->mousec);
	chanclose(ctl->resizec);

	threadint(ctl->wthread);

	if(ctl->pflags & OURKBD)
		closekeyboard(ctl->kbd);

	if(ctl->pflags & OURMOUSE)
		closemouse(ctl->mouse);

	free(ctl);
}

void
redrawwctl(Widgetctl *ctl)
{
	redrawwidget(ctl->root, ctl->image, ctl->image->r);
	flushimage(ctl->image->display, 1);
}

/* TODO set clipr */
Point
redrawwidget(Widget *w, Image *dst, Rectangle r)
{
	return w->redraw(w, dst, r);
}

int
kbdevent(Widget *w, Image *img, Rectangle rect, Rune r, Channel *c)
{
	if(w->kbdevent != nil)
		return w->kbdevent(w, img, rect, r, c);
	return 0;
}

int
mouseevent(Widget *w, Image *img, Rectangle rect, Mouse m, Channel *c)
{
	if(w->mouseevent != nil)
		return w->mouseevent(w, img, rect, m, c);
	return 0;
}

void
freewidget(Widget *w)
{
	if(w != nil)	/* freeing nil widgets should be a no-op */
		w->cleanup(w);
}

void
wdefaults(Widget *w)
{
	w->id = nextid();
	w->bg = display->white;
	w->fg = display->black;
	w->kbdevent = nil;
	w->mouseevent = nil;
	w->cleanup = (void(*)(Widget*))free;
}

Widgetmsg*
newmsg(Widget *w, u32int what)
{
	Widgetmsg *msg;

	msg = emalloc(sizeof(*msg));
	msg->sender = w;
	msg->what = what;

	return msg;
}
