/*** Box ***/

typedef struct Box Box;

struct Box
{
	Widget;

	Widget *content;
	int flags;

	/* don't touch */
	Rectangle conrect;
	int focused;
};

enum /* flags */
{
	B_CENTER_CONTENT	= 1<<0
};

int isbox(Widget*);

Box* newbox(Widget*, int flags);
Box* newcenterbox(Widget*);

