#include <stddef.h>

//=============================================================================
// compare no more SZ bytes D and S
//=============================================================================
int	strncmp (const char *d, const char *s, size_t sz)
{
#if 0
	/* Gaku's code */

	while ( sz-- && '\0' != *d ) {
		if (*d != *s)
			return *d - *s;
		d++;
		s++;
	}
	return *d - *s; /* bug? (if sz == 0) */

#endif

#if 1
	/* Kawai's code */

	while (sz--) {
		if (*d == '\0')
			goto ret;
		if (*d != *s) {
ret:
			return (unsigned char)*d - (unsigned char)*s;
		}
		d++;
		s++;
	}
	return 0;
#endif

}
