/* Wait for a subprocess to finish. */

#include <syscall.h>
#include "tests/lib.h"
#include "tests/main.h"

void
test_main (void) 
{
	int t=wait (exec ("child-simple"));
//	printf("t=%d\n",t);
  msg ("wait(exec()) = %d", t);
}
