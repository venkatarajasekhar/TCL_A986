
#include <stdio.h>
#include <stdlib.h>

#include "op_version.h"
#include "config.h"

void show_version(char const * app_name)
{
	/* Do not change the version format: it is documented in html doc */
	printf("%s: " PACKAGE " " VERSION " compiled on "
	       __DATE__ " " __TIME__ "\n", app_name);
	exit(EXIT_SUCCESS);
}
