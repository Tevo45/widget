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
