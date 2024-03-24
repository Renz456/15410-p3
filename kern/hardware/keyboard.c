/**
 * @file keyboard.c
 * @brief File contains implementation of keyboard driver interface.
 *
 * Keyboard buffer implemented as a global variable
 * @author Rene Ravanan (rravanan)
 *
 * @bug No Known bugs
 */

#include <stdio.h>
#include <keyhelp.h>
#include <interrupt_defines.h>
#include <asm.h>
#include <thread.h>
#include <scheduler.h>
#include <simics.h>
#include <synchronization/mutex_kern.h>
#include <synchronization/cvar_kern.h>
#include <console.h>
#include <inc/thread.h>

// Reasonable buffer size given console height and width
#define KB_BUFF_SIZE 256

char kb_buffer[KB_BUFF_SIZE];
int current_buffer_index = 0;
int newest_char_index = 0;

mutex_t keyb_mp = {0};
cond_t keyb_cv = {0};

void install_keyboard()
{
  mutex_init(&keyb_mp);
  cond_init(&keyb_cv);
}

/**
 * @brief Helper to increment buffer indices
 *
 * @param index : index to increment
 * @return int : incremented index
 */
int increment_index(int index)
{
  return (index + 1) % KB_BUFF_SIZE;
}

void keyboard_interupt_handler()
{
  char new_byte = inb(KEYBOARD_PORT);
  mutex_lock(kernel_gettid(), &keyb_mp);
  if (increment_index(newest_char_index) == current_buffer_index)
  {
    mutex_unlock(&keyb_mp);
    return;
  }
  lprintf("received a key press\n");
  kb_buffer[newest_char_index] = new_byte;
  newest_char_index = increment_index(newest_char_index);
  mutex_unlock(&keyb_mp);
  cond_signal(&keyb_cv);
  outb(INT_CTL_PORT, INT_ACK_CURRENT);
  return;
}

int readchar(void)
{
  disable_interrupts();
  // read from the buffer process scancode and then
  // return first valid character
  mutex_lock(kernel_gettid(), &keyb_mp);
  while (current_buffer_index != newest_char_index)
  {
    char ch = kb_buffer[current_buffer_index];
    current_buffer_index = increment_index(current_buffer_index);
    kh_type augch = process_scancode(ch);
    if (KH_HASDATA(augch) && KH_ISMAKE(augch))
    {
      mutex_unlock(&keyb_mp);
      enable_interrupts();
      return KH_GETCHAR(augch);
    }
  }
  mutex_unlock(&keyb_mp);
  enable_interrupts();
  return -1;
}

int readline(int len, char *buf)
{

  if (len <= 0 || len > KB_BUFF_SIZE)
  {
    lprintf("bad readline input len %d\n", len);
    return -1;
  }

  lprintf("entering readline\n");
  char tmp_buf[len];
  int n = 0;
  int current_char = 0;
  while (n < len && (char)current_char != '\n')
  {
    current_char = readchar();
    int tid = kernel_gettid();
    mutex_lock(tid, &keyb_mp);
    switch (current_char)
    {
    case -1:
      cond_wait(&keyb_cv, &keyb_mp);
      break;
    case '\b':
      if (n > 0)
      {
        putbyte(current_char);
        n--;
      }
      break;
    default:
      putbyte(current_char);
      tmp_buf[n] = current_char;
      n++;
      break;
    }
    mutex_unlock(&keyb_mp);
  }
  for (int i = 0; i < n; i++)
  {
    buf[i] = tmp_buf[i];
  }

  return n;
}
