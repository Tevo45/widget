#include <u.h>
#include <libc.h>
#include <draw.h>
#include <thread.h>
#include <mouse.h>
#include <keyboard.h>
#include <String.h>

#include <widget.h>
#include "w-internal.h"

static char *btnkind = "Button";
static Image *lightblue, *darkblue;

int
isbutton(Widget *w)
{
	return strcmp(w->kind, btnkind) == 0;
}

Point
btnredraw(Widget *w, Image *dst, Rectangle r)
{
	Image *tmp;
	Button *btn;
	Point btsz, pos, sz;
	Rectangle conrect;

	if(!isbutton(w))
		werror("btnredraw: not a button");

	btn = (Button*)w;
	tmp = allocimage(dst->display, r, RGBA32, 0, DTransparent);
	sz  = redrawwidget(btn->content, tmp, r);

	pos = btsz = subpt(r.max, r.min);
	pos = divpt(pos, 2);
	pos = subpt(pos, divpt(sz, 2));

	conrect = Rpt(pos, subpt(r.max, pos));

	draw(dst, r, btn->pressed ? btn->fg : btn->bg, nil, ZP);
	draw(dst, conrect, tmp, nil, ZP);

	freeimage(tmp);

	return btsz;
}

int
btnmouse(Widget *w, Image *dst, Rectangle rect, Mouse m, Channel *chan)
{
	Button *btn;
	int pressed;
	Widgetmsg *msg;

	if(!isbutton(w))
		werror("btndraw: not a button");

	btn = (Button*)w;
	if((pressed = m.buttons & 1) != btn->pressed)
	{
		if(pressed)
		{
			msg = newmsg(btn, M_BUTTON_PRESSED);
			send(chan, &msg);
		}
		btn->pressed = pressed;
		btnredraw(btn, dst, rect);
		return 1;
	}

	return 0;
}

void
btnfree(Widget *w)
{
	Button *btn;

	if(!isbutton(w))
		werror("btnfree: not a button");

	btn = (Button*)w;
	freewidget(btn->content);
	free(btn);
}

Button*
newbutton(Widget *w)
{
	Button *btn;

	if(lightblue == nil)
		lightblue = allocimagemix(display, DPalebluegreen, DWhite);

	if(darkblue == nil)
		darkblue = allocimagemix(display, DPurpleblue, DPalebluegreen);

	btn = emalloc(sizeof(*btn));
	wdefaults(btn);
	btn->bg			= lightblue;
	btn->fg			= darkblue;
	btn->kind		= btnkind;
	btn->redraw		= btnredraw;
	btn->cleanup	= btnfree;
	btn->content	= w;

	btn->mouseevent	= btnmouse;

	return btn;
}

Button*
newtextbutton(Font *f, char *content)
{
	return newbutton(newtextbox(0, 0, f, content));
}
