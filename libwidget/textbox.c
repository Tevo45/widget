#include <u.h>
#include <libc.h>
#include <draw.h>
#include <thread.h>
#include <mouse.h>
#include <keyboard.h>
#include <String.h>

#include <widget.h>
#include "w-internal.h"

static char *tbkind = "Textbox";

int
istextbox(Widget *w)
{
	return strcmp(w->kind, tbkind) == 0;
}

Point
tbredraw(Widget *w, Image *dst, Rectangle r)
{
	Textbox *tb;
	Point ret;

	if(!istextbox(w))
		werror("tbredraw: not a textbox");

	tb = (Textbox*)w;
	s_terminate(tb->content);
	tb->lastpt = string(dst, r.min, tb->fg, ZP, tb->font, s_to_c(tb->content));
	ret = subpt(tb->lastpt, r.min);
	ret.y += tb->font->height;

	return ret;
}

void
tbfree(Widget *w)
{
	Textbox *tb;

	if(!istextbox(w))
		werror("tbfree: not a textbox");

	tb = (Textbox*)w;
	s_free(tb->content);
	free(tb);
}

Textbox*
newtextbox(int selectable, int editable, Font *f, char *content)
{
	Textbox *tb;

	tb = emalloc(sizeof(*tb));
	wdefaults(tb);
	tb->kind	= tbkind;
	tb->redraw	= tbredraw;
	tb->cleanup	= tbfree;

	tb->selectable	= selectable;
	tb->editable	= editable;

	if(f != nil)
		tb->font = f;
	else
		tb->font = font;

	if(content != nil)
		tb->content = s_copy(content);
	else
		tb->content	= s_new();

	return tb;
}
