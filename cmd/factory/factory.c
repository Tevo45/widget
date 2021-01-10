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
	Widgetctl *wctl;
	Widgetmsg *msg;
	Button *root;
	Rune rune;

	ARGBEGIN {
	default:
		usage();
	} ARGEND;

	if(initdraw(nil, nil, "widget factory") < 0)
		sysfatal("initdraw: %r");

	root = newtextbutton(nil, "hello, world!");

	if((wctl = initwidget(screen, nil, nil, root, FORWARD_KBD)) == nil)
		sysfatal("initwidget: %r");

	enum
	{
		MESSAGE, RESIZE, KEYBOARD
	};

	Alt chans[] = 
	{
		{ wctl->c,			&msg,	CHANRCV },
		{ wctl->resizec,	nil,	CHANRCV },
		{ wctl->kbdc,		&rune,	CHANRCV },

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
		case KEYBOARD:
			if(rune == '')
				goto end;
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
	closewidget(wctl);
	freewidget(root);

	exits(0);
}
