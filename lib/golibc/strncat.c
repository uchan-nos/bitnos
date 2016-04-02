#include <stddef.h>

//=============================================================================
// append no more SZ bytes from S to D
//=============================================================================
char* strncat (char *d, const char *s, size_t sz)
{
	char *tmp = d;

	while ('\0' != *d)
		d++;

	while ('\0' != *s) {
		if (0 == sz)
			break;
		sz--;
		*d++ = *s++;
	}

	if (sz)
		*d = '\0';

	return tmp;
}
