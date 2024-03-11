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
    int x = 1;
    // x += 1;
    x = gettid();
    lprintf("IN IDLE! %d\n", x);
    // MAGIC_BREAK;
    while (1)
    {
    }
}
