/*** Box ***/

typedef struct Box Box;

struct Box
{
	Widget;

	Widget *content;
	int flags;

	Point maxsize;

	/* don't touch */
	int focused;
	Rectangle bounds;
};

enum /* flags */
{
	B_CENTER_CONTENT	= 1<<0
};

int isbox(Widget*);

Box* newbox(Widget*, int flags);
Box* newcenterbox(Widget*);

