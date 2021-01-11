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

/* centers a inside b */
static Rectangle
centerrect(Rectangle a, Rectangle b)
{
	Rectangle a’, b’;
	Point ps;

	a’ = rectsubpt(a, a.min);
	b’ = rectsubpt(b, b.min);

	ps = divpt(b’.max, 2);
	ps = subpt(ps, divpt(a’.max, 2));

	return Rpt(addpt(b.min, ps), subpt(b.max, ps));
}

Point
boxredraw(Widget *w, Image *dst, Rectangle r)
{
	Box *box;
	/* wrect = where widget is allowed to draw, arect = where widget actually drew */
	Rectangle wrect, arect;
	Image *tmp;
	Point sz;

	box = coerce(w);
	if(memcmp(&box->maxsize, &ZP, sizeof(Point)) != 0) /* box->maxsize != ZP */
	{
		wrect = Rpt(r.min, addpt(r.min, box->maxsize));
		rectclip(&wrect, r);
	}
	else
		wrect = r;

	if(box->flags & B_CENTER_CONTENT)
		wrect = centerrect(wrect, r);

	tmp = allocimage(dst->display, wrect, RGBA32, 0, DTransparent);
	sz  = redrawwidget(box->content, tmp, wrect);

	if(box->flags & B_CENTER_CONTENT)
		arect = centerrect(Rpt(ZP, sz), r);
	else
		arect = Rpt(r.min, addpt(r.min, sz));

	draw(dst, r, box->bg, nil, ZP);
	draw(dst, arect, tmp, nil, wrect.min);

	freeimage(tmp);

	box->bounds = wrect;

	return subpt(r.max, r.min);
}

int
boxmouse(Widget *w, Image *dst, Rectangle, Mouse m, Channel *chan)
{
	Box *box;

	box = coerce(w);

	if(ptinrect(m.xy, box->bounds))
	{
		box->focused = 1;
		return mouseevent(box->content, dst, box->bounds, m, chan);
	}
	else
		box->focused = 0;

	return 0;
}

int
boxkbd(Widget *w, Image *dst, Rectangle, Rune r, Channel *chan)
{
	Box *box;

	box = coerce(w);

	if(box->focused)
		return kbdevent(box->content, dst, box->bounds, r, chan);

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
	box->maxsize	= ZP;
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
