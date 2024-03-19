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
    int x = gettid();
    lprintf("IN IDLE! %d\n", x);
    while (1)
    {
    }
}
