static void*
emalloc(ulong sz)
{
	void *p;

	p = malloc(sz);
	if(p == nil)
		werror("malloc: %r");
	setmalloctag(p, getcallerpc(&sz));
	return p;
}

/* Widgetctl->flags */
enum
{
	OURKBD		= 1<<0,
	OURMOUSE	= 1<<1
};
