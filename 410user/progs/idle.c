/** @file 410user/progs/idle.c
 *  @author ?
 *  @brief Idle program.
 *  @public yes
 *  @for p2 p3
 *  @covers
 *  @status done
 */

#include <simics.h>
#include <syscall.h>

int main()
{
    long x = 1;
    // x += 1;
    // x = gettid();
    lprintf("IN IDLE! %ld\n", x);
    // MAGIC_BREAK;
    while (1)
    {
    }
}
