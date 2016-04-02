#include <stddef.h>

//=============================================================================
// compare D and S
//=============================================================================
int strcmp (const char *d, const char *s)
{
	while (*d == *s) {
		if ('\0' == *d)
			return 0;
		d++;
		s++;
	}

	return (unsigned char)*d - (unsigned char)*s;
}
