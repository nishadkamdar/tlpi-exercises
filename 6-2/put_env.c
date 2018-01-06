#include <stdlib.h>

extern char **environ;
int main ()
{
char **ep;
static char *var = "HOME_ALONE=/usr/home";
int ret;




ret = putenv(var);

for (ep = environ; *ep != NULL; ep++)
	puts (*ep);

exit (EXIT_SUCCESS);
}
