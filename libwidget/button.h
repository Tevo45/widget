/*** Button ***/

typedef struct Button Button;

struct Button
{
	Widget;

	Widget *content;
	int pressed;

	/* don't touch */
	Box *box;
};

int isbutton(Widget*);

Button* newbutton(Widget*);
Button* newtextbutton(Font*, char *content);

enum	/* messages */
{
	M_BUTTON_PRESSED	= C2I('b', 't', 'n', 'p'),
	M_BUTTON_RELEASED	= C2I('b', 't', 'n', 'r')
};

