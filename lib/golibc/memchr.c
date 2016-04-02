#include <stdio.h>
#include <stddef.h>

//=============================================================================
// search SZ bytes of D for C
//=============================================================================
void* memchr (void *d, int c, size_t sz)
{
	char *dp = (char*)d;

	while (sz--) {
		if (c == *dp)
			return dp;
		dp++;
	}

	return NULL;
}
