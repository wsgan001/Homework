/* Tries to create a file with the empty string as its name. */

#include "tests/lib.h"
#include "tests/main.h"

void
test_main (void) 
{
	int t=create ("", 0);
	//printf("createok\n");
  msg ("create(\"\"): %d", t);
}
