#pragma lib "libwidget.a"

typedef struct Widget Widget;
typedef struct Widgetctl Widgetctl;
typedef struct Widgetmsg Widgetmsg;

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
	Channel *c;	/* chan(Widgetmsg*)[16] */
	Channel *resizec;
	Widget *root;

	Keyboardctl *kbd;
	Mousectl *mouse;

	Image *image;
};

struct Widgetmsg
{
	Widget *sender;
	u32int what;
};

Widgetmsg* newmsg(Widget*, u32int what);

#define C2I(a, b, c, d) ((a) << 24 | (b) << 16 | (c) << 8 | (d))

extern void (*werror)(char*, ...);

Widgetctl* initwidget(Image*, Keyboardctl*, Mousectl*, Widget *root);

void wdefaults(Widget*);

int nextid(void);

Point redrawwidget(Widget*, Image*, Rectangle);

int kbdevent(Widget*, Image*, Rectangle, Rune, Channel* /*(Widgetmsg*)*/);
int mouseevent(Widget*, Image*, Rectangle, Mouse, Channel* /*(Widgetmsg*)*/);

void freewidget(Widget*);
