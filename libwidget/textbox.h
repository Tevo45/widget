
typedef struct Textbox Textbox;

struct Textbox
{
	Widget;

	Font *font;
	String *content;
	Point lastpt;
	int selectable;
	int editable;
};

int istextbox(Widget*);

Textbox* newtextbox(int selectable, int editable, Font*, char *content);
