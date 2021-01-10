#include <u.h>
#include <libc.h>
#include <draw.h>
#include <thread.h>
#include <mouse.h>
#include <keyboard.h>
#include <String.h>

#include <widget.h>
#include "w-internal.h"

static char *boxkind = "Box";

int
isbox(Widget *w)
{
	return strcmp(w->kind, boxkind) == 0;
}

static Box*
coerce(Widget *w)
{
	if(!isbox(w))
		werror("coerce: not a box");

	return (Box*)w;
}

Point
boxredraw(Widget *w, Image *dst, Rectangle r)
{
	Image *tmp;
	Box *box;
	Point boxsz, pos, sz;

	box = coerce(w);

	tmp = allocimage(dst->display, r, RGBA32, 0, DTransparent);
	sz  = redrawwidget(box->content, tmp, r);

	pos = boxsz = subpt(r.max, r.min);
	pos = divpt(pos, 2);
	pos = subpt(pos, divpt(sz, 2));

	box->conrect = Rpt(pos, subpt(r.max, pos));

	draw(dst, r, box->bg, nil, ZP);
	draw(dst, box->conrect, tmp, nil, ZP);

	freeimage(tmp);

	return boxsz;
}

int
boxmouse(Widget *w, Image *dst, Rectangle rect, Mouse m, Channel *chan)
{
	Box *box;

	box = coerce(w);

	if(ptinrect(m.xy, box->conrect))
	{
		box->focused = 1;
		return mouseevent(box->content, dst, rect, m, chan);
	}
	else
		box->focused = 0;

	return 0;
}

int
boxkbd(Widget *w, Image *dst, Rectangle rect, Rune r, Channel *chan)
{
	Box *box;

	box = coerce(w);

	if(box->focused)
		return kbdevent(box->content, dst, rect, r, chan);

	return 0;
}

void
boxfree(Widget *w)
{
	Box *box;

	box = coerce(w);
	freewidget(box->content);
	free(box);
}

Box*
newbox(Widget *w, int flags)
{
	Box *box;

	box = emallocz(sizeof(*box), 1);
	wdefaults(box);
	box->kind	= boxkind;
	box->redraw	= boxredraw;
	box->flags	= flags;
	box->kbdevent	= boxkbd;
	box->mouseevent	= boxmouse;
	box->cleanup	= boxfree;
	box->content	= w;

	return box;
}

Box*
newcenterbox(Widget *w)
{
	return newbox(w, B_CENTER_CONTENT);
}
