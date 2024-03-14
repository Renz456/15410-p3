/**
 * The 15-410 kernel project.
 * @name loader.c
 *
 * Functions for the loading
 * of user programs from binary
 * files should be written in
 * this file. The function
 * elf_load_helper() is provided
 * for your use.
 */
/*@{*/

/* --- Includes --- */
#include <string.h>
#include <stdio.h>
#include <malloc.h>
#include <exec2obj.h>
#include <loader.h>
#include <elf_410.h>
#include <simics.h>
#include <inc/vm.h>

/* --- Local function prototypes --- */

int find_app_index(const char *filename)
{
  for (int i = 0; i < MAX_NUM_APP_ENTRIES; i++)
  {
    if (strcmp(exec2obj_userapp_TOC[i].execname, filename) == 0)
      return i;
  }
  return -1;
}

/**
 * Copies data from a file into a buffer.
 *
 * @param filename   the name of the file to copy data from
 * @param offset     the location in the file to begin copying from
 * @param size       the number of bytes to be copied
 * @param buf        the buffer to copy the data into
 *
 * @return returns the number of bytes copied on succes; -1 on failure
 */
int getbytes(const char *filename, int offset, int size, char *buf)
{
  int app_index = find_app_index(filename);
  if (app_index == -1)
  {
    return -1;
  }

  memcpy(buf, exec2obj_userapp_TOC[app_index].execbytes + offset, size);
  return size;
}

/*@}*/

int init_address_space(const char *filename)
{
  if (elf_check_header(filename) != ELF_SUCCESS)
  {
    lprintf("Invalid file name!\n");
    return -1;
  }

  simple_elf_t se_hdr;
  if (elf_load_helper(&se_hdr, filename) != ELF_SUCCESS)
  {
    lprintf("Failed to load elf file!\n");
    return -1;
  }

  int check_align;

  lprintf("check text start %p\n", (void *)se_hdr.e_txtstart);

  check_align = align_pages((void *)se_hdr.e_txtstart, se_hdr.e_txtlen);
  if (check_align < -1)
    lprintf("App text pages failed!!\n");

  if (getbytes(se_hdr.e_fname, se_hdr.e_txtoff, se_hdr.e_txtlen, (char *)se_hdr.e_txtstart) < 0)
    lprintf("app text get bytes failed\n");

  // if ((void *)se_hdr.e_datstart)
  check_align = align_pages((void *)se_hdr.e_datstart, se_hdr.e_datlen);
  if (check_align < -1)
    lprintf("App data pages failed!!\n");
  // if ((void *)se_hdr.e_datstart)

  if (getbytes(se_hdr.e_fname, se_hdr.e_datoff, se_hdr.e_datlen, (char *)se_hdr.e_datstart) < 0)
    lprintf("app data get bytes failed\n");

  // if ((void *)se_hdr.e_rodatstart)
  check_align = align_pages((void *)se_hdr.e_rodatstart, se_hdr.e_rodatlen);
  if (check_align < -1)
    lprintf("App rodata pages failed!!\n");
  // if ((void *)se_hdr.e_rodatstart)

  if (getbytes(se_hdr.e_fname, se_hdr.e_rodatoff, se_hdr.e_rodatlen, (char *)se_hdr.e_rodatstart) < 0)
    lprintf("app rodata get bytes failed\n");

  check_align = align_pages((void *)se_hdr.e_bssstart, se_hdr.e_bsslen);
  if (check_align < -1)
    lprintf("App bss pages failed!!\n");

  // bss does not seem to have an offset
  // getbytes(se_hdr.e_fname, 0, se_hdr.e_bsslen, (char *)se_hdr.e_txtstart);
  memset((void *)se_hdr.e_bssstart, 0, se_hdr.e_bsslen);

  return 0;
}