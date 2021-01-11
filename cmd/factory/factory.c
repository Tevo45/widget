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
	Menumsg mmsg;
	Box *root;
	Rune rune;

	ARGBEGIN {
	default:
		usage();
	} ARGEND;

	if(initdraw(nil, nil, "widget factory") < 0)
		sysfatal("initdraw: %r");

	enum
	{
		MIDDLE_EXIT
	};

	char *mentries[] =
	{
		[MIDDLE_EXIT] = "exit",
		nil,
	};

	Menu middle =
	{
		.item		= mentries,
		.lasthit	= -1
	};

	root = newcenterbox(
		newtextbutton(nil, "hello, world!")
	);

	root->maxsize = Pt(256, 128);

	if((wctl = initwidget(screen, nil, nil, root, FORWARD_KBD)) == nil)
		sysfatal("initwidget: %r");

	wctl->middle = &middle;

	enum
	{
		MESSAGE, RESIZE, KEYBOARD, MENU
	};

	Alt chans[] = 
	{
		{ wctl->c,			&msg,	CHANRCV },
		{ wctl->resizec,	nil,	CHANRCV },
		{ wctl->kbdc,		&rune,	CHANRCV },
		{ wctl->menuc,		&mmsg,	CHANRCV },

		{ nil,				nil,	CHANEND }
	};

	for(;;)
		switch(alt(chans))
		{
		case MESSAGE:
			switch(msg->what)
			{
			case M_BUTTON_PRESSED:
			//	print("button %d was pressed!\n", msg->sender->id);
				break;
			case M_BUTTON_RELEASED:
			//	print("button %d was released!\n", msg->sender->id);
				break;
			}
			free(msg);
			break;
		case KEYBOARD:
			if(rune == '')
				goto end;
			break;
		case MENU:
			switch(mmsg.button)
			{
			case M_MIDDLE:
				switch(mmsg.hit)
				{
				case MIDDLE_EXIT:
					goto end;
				}
				break;
			}
			break;
		case RESIZE:
			if(getwindow(display, Refnone) < 0)
				sysfatal("getwindow: cannot resize: %r");
			redrawwctl(wctl);
			break;
		}

end:
	closewidget(wctl);
	freewidget(root);

	exits(0);
}
