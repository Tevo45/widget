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

static Button*
coerce(Widget *w)
{
	if(!isbutton(w))
		werror("coerce: not a button");

	return (Button*)w;
}

Point
btnredraw(Widget *w, Image *dst, Rectangle r)
{
	Button *btn;

	btn = coerce(w);

	btn->box->content = btn->content;
	btn->box->bg = btn->pressed ? btn->fg : btn->bg;

	return redrawwidget(btn->box, dst, r);
}

int
btnmouse(Widget *w, Image *dst, Rectangle rect, Mouse m, Channel *chan)
{
	Button *btn;
	int pressed;
	Widgetmsg *msg;

	btn = coerce(w);
	if((pressed = m.buttons & 1) != btn->pressed)
	{
		msg = newmsg(btn, pressed ? M_BUTTON_PRESSED : M_BUTTON_RELEASED);
		send(chan, &msg);
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

	btn = coerce(w);
	btn->box->content = nil;	/* to avoid double-free */
	freewidget(btn->content);
	freewidget(btn->box);
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

	btn = emallocz(sizeof(*btn), 1);
	wdefaults(btn);
	btn->bg			= lightblue;
	btn->fg			= darkblue;
	btn->box		= newcenterbox(w);
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
