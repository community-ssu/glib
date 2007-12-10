/*

    Test for ARMv6 instruction set support.

    Copyright (C) 2007 Nokia Corporation.

    Contact: Jussi Laako <jussi.laako@nokia.com>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

*/

/* this test breaks the POSIX rules on setjmp()/longjmp() a bit, but it
 * happens to work
 */

#include <stdio.h>
#include <signal.h>
#include <setjmp.h>

void set (volatile int *atomic, int newval)
{
  unsigned long result;

  do {
    asm volatile (
      "ldrex %0, [%1]\n"
      "strex %0, %2, [%1]\n"
      : "=&r" (result)
      : "r" (atomic), "r" (newval)
      : "cc", "memory");
  } while (result);
}

static jmp_buf errjmp;

void sig_handler (int signo)
{
	switch (signo)
	{
		case SIGILL:
			longjmp(errjmp, 1);
			break;
	}
}
	
int main (int argc, char *argv[])
{
	volatile int a;

	signal(SIGILL, sig_handler);
	if (setjmp(errjmp) == 0)
	{
		set(&a, 7);
		return 0;
	}
	signal(SIGILL, SIG_DFL);

	return 1;
}

