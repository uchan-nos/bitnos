#include <stddef.h>

//=============================================================================
// set SZ bytes of S to C
//=============================================================================
void * memset (void *d, int c, size_t sz)
{
	void *tmp = d;
	char *dp = (char*)d;

	while (sz--)
		*dp++ = c;

	return tmp;
}
