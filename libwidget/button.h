
typedef struct Button Button;

struct Button
{
	Widget;

	Widget *content;
	int pressed;
};

int isbutton(Widget*);

Button* newbutton(Widget*);
Button* newtextbutton(Font*, char *content);

enum
{
	M_BUTTON_PRESSED = C2I('b', 't', 'n', 'p')
};

