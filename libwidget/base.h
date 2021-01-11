#pragma lib "libwidget.a"

typedef struct Widget Widget;
typedef struct Widgetctl Widgetctl;
typedef struct Widgetmsg Widgetmsg;

typedef struct Menumsg Menumsg;

struct Widget
{
	int id;
	char *kind;

	void *aux;	/* for the user */

	Image *bg, *fg;

	Point (*redraw)(Widget*, Image*, Rectangle);

	/* can be nil if the widget doesn't take events */
	int (*kbdevent)(Widget*, Image*, Rectangle, Rune, Channel* /*(Widgetmsg*)*/);
	int (*mouseevent)(Widget*, Image*, Rectangle, Mouse, Channel* /*(Widgetmsg*)*/);

	void (*cleanup)(Widget*);
};

struct Widgetctl
{
	Channel *c;			/* chan(Widgetmsg*)[16]	*/
	Channel *kbdc;		/* chan(Rune)[20]		*/
	Channel *menuc;		/* chan(Menumsg)[16]	*/
	Channel *mousec;	/* chan(Mouse)[16]		*/
	Channel *resizec;
	/* NOTE user retains ownership of the root widget */
	Widget *root;

	Keyboardctl *kbd;
	Mousectl *mouse;

	/* if non-nil, shown when widgets don't handle the respective mouse events */
	Menu *left, *middle, *right;

	Image *image;

	int flags;

	/* user shouln't care about anything below this point */
	int pflags;
	int wthread;
};

enum /* flags */
{
	FORWARD_KBD		= 1<<0,
	FORWARD_MOUSE	= 1<<1
};

enum /* mouse buttons */
{
	M_LEFT		= 1<<0,
	M_MIDDLE	= 1<<1,
	M_RIGHT		= 1<<2
	/* TODO add scroll up/down */
};

struct Menumsg
{
	Menu *menu;
	int button, hit;
};

struct Widgetmsg
{
	Widget *sender;
	u32int what;
};

Widgetctl* initwidget(Image*, Keyboardctl*, Mousectl*, Widget *root, int flags);
void redrawwctl(Widgetctl*);
void closewidget(Widgetctl*);

Point redrawwidget(Widget*, Image*, Rectangle);
void freewidget(Widget*);

int kbdevent(Widget*, Image*, Rectangle, Rune, Channel* /*(Widgetmsg*)*/);
int mouseevent(Widget*, Image*, Rectangle, Mouse, Channel* /*(Widgetmsg*)*/);

void wdefaults(Widget*);
int nextid(void);
Widgetmsg* newmsg(Widget*, u32int what);

#define C2I(a, b, c, d) ((a) << 24 | (b) << 16 | (c) << 8 | (d))

extern void (*werror)(char*, ...);

