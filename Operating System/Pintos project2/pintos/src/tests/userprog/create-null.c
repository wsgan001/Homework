/* Tries to create a file with the null pointer as its name.
   The process must be terminated with exit code -1. */

#include "tests/lib.h"
#include "tests/main.h"

void
test_main (void) 
{
	int t=create (NULL, 0);
  msg ("create(NULL): %d", t);
  
}
