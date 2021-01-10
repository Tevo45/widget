#include <u.h>
#include <libc.h>
#include <draw.h>
#include <thread.h>
#include <mouse.h>
#include <keyboard.h>
#include <String.h>
#include <widget.h>

void
usage(void)
{
	fprint(2, "usage: %s", argv0);
	exits("usage");
}

void
threadmain(int argc, char **argv)
{
	Keyboardctl *kbctl;
	Mousectl *mctl;
	Button *root;
	Widgetctl *wctl;
	Widgetmsg *msg;

	ARGBEGIN {
	default:
		usage();
	} ARGEND;

	if(initdraw(nil, nil, "widget factory") < 0)
		sysfatal("initdraw: %r");

	if((mctl = initmouse(nil, screen)) == nil)
		sysfatal("initmouse: %r");

	if((kbctl = initkeyboard(nil)) == nil)
		sysfatal("initkeyboard: %r");

	root = newtextbutton(nil, "hello, world!");

	if((wctl = initwidget(screen, kbctl, mctl, root)) == nil)
		sysfatal("initwidget: %r");

	enum
	{
		MESSAGE, RESIZE
	};

	Alt chans[] = 
	{
		{ wctl->c,			&msg,	CHANRCV },
		{ wctl->resizec,	nil,	CHANRCV },

		{ nil,				nil,	CHANEND }
	};

	for(;;)
	{
		switch(alt(chans))
		{
		case MESSAGE:
			print("got message for %d!\n", msg->what);
			free(msg);
			break;
		case RESIZE:
			if(getwindow(display, Refnone) < 0)
				sysfatal("getwindow: cannot resize: %r");
			/* FIXME most users shouldn't need to call this directly */
			redrawwidget(root, screen, screen->r);
			break;
		}
		flushimage(display, 1);
	}

end:
	closemouse(mctl);
	closekeyboard(kbctl);
	freewidget(root);

	exits(0);
}
