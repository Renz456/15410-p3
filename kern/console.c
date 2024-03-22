/**
 * @file console.c
 * @brief Contains implementations of Console Interface functions.
 *        Descriptions of functions can be found in p1kern.h
 *
 * Console driver functions are implemented by keeping global values for cursor
 * position and colour. This allows the file to keep track of cursor state
 * after each function call. To keep the cursor "hidden" when required, I chose
 * to set the cursor outside of the console bounds (at CONSOLE_HEIGHT,
 * CONSOLE_WIDTH). To "scroll" when we reach the end of the console, I choose to
 * memmove the specified number of rows upwards as we are writing in overlapping
 * memory regions. Additional helpers exist as part of the implementation and
 * have been documented below.
 *
 * @author Rene Ravanan (rravanan)
 *         Abhinav Gupta (abhinav6)
 *
 * @bug No known bugs
 */

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <p1kern.h>
#include <asm.h>

// Global constants to keep track of cursor position,
// status and terminal colour
static int cursor_row = 0;
static int cursor_col = 0;
static int is_cursor_hidden = 0;
// personal choice, if not initialised
static int terminal_color = FGND_WHITE | BGND_BLACK;

/**
 * @brief Gets address on console memory for a given index
 *
 * @param row : row on console
 * @param col : column on console
 * @return void* : pointer to given index on console
 */
void *get_addr_from_index(int row, int col)
{
    return (void *)(CONSOLE_MEM_BASE + ((row * CONSOLE_WIDTH + col) * 2));
}

/**
 * @brief Helper function to clear console from a specified start point
 *
 * @param row Starting row to clear from
 * @param col Starting column to clear from
 */
void clear_console_from(int row, int col)
{
    for (int i = row; i < CONSOLE_HEIGHT; i++)
    {
        for (int j = col; j < CONSOLE_WIDTH; j++)
        {
            draw_char(i, j, ' ', terminal_color);
        }
    }
}

/**
 * @brief Helper function to "scroll" console by desired number of rows
 *
 * @param rows : number of rows to scroll by
 */
void scroll(int rows)
{
    void *new_addr = get_addr_from_index(rows, 0);
    int size = ((CONSOLE_HEIGHT - rows) * CONSOLE_WIDTH) * 2;
    /*Using memmove as it cases for overlapping addresses
     which is the case here */
    memmove((void *)CONSOLE_MEM_BASE, new_addr, size);
    // Have to clear rows after too.
    clear_console_from(CONSOLE_HEIGHT - rows, 0);
}

/**
 * @brief Helper function to case for scrolling/entering next row
 *
 * @param row : row to set cursor to
 * @param col : col to set cursor to
 * @return int : -1 if invalid inputs, 0 on success
 */
int set_cursor_helper(int row, int col)
{
    if (row < 0 || col < 0)
        return -1;

    if (col == CONSOLE_WIDTH)
    {
        row += 1;
        col = 0;
    }

    if (row == CONSOLE_HEIGHT)
    {
        scroll((int)(row - (CONSOLE_HEIGHT - 1)));
        row = CONSOLE_HEIGHT - 1;
    }
    set_cursor(row, col);
    return 0;
}

int putbyte(char ch)
{
    switch (ch)
    {
    case '\n':
        set_cursor_helper(cursor_row + 1, 0);
        break;
    case '\r':
        set_cursor_helper(cursor_row, 0);
        break;
    case '\b':
        if (cursor_col > 0)
        {
            draw_char(cursor_row, cursor_col - 1, ' ', terminal_color);
            set_cursor_helper(cursor_row, cursor_col - 1);
        }
        else
        {
            draw_char(cursor_row - 1, CONSOLE_WIDTH - 1, ' ', terminal_color);
            set_cursor_helper(cursor_row - 1, CONSOLE_WIDTH - 1);
        }
        break;
    default:
        if (isprint(ch))
        {
            draw_char(cursor_row, cursor_col, ch, terminal_color);
            set_cursor_helper(cursor_row, cursor_col + 1);
        }
        break;
    }
    if (isprint(ch))
        return ch;
    else
        return -1;
}

void putbytes(const char *s, int len)
{
    affirm(s == NULL || len <= 0);
    for (int i = 0; i < len; i++)
        putbyte(s[i]);
}

int set_term_color(int color)
{
    // Max fgnd colour is 0x70, max bgnd color is 0xF
    int max_color = 0x7F;
    if (color > max_color || color < 0)
        return -1;
    terminal_color = color;
    return 0;
}

void get_term_color(int *color)
{
    if (color == NULL)
        return;
    *color = terminal_color;
}

void get_cursor(int *row, int *col)
{
    if (row == NULL || col == NULL)
        return;

    *row = cursor_row;
    *col = cursor_row;
}

char get_char(int row, int col)
{
    if (row < 0 || row >= CONSOLE_HEIGHT || col < 0 || col >= CONSOLE_WIDTH)
        return 0;
    char *char_addr = (char *)get_addr_from_index(row, col);
    return *char_addr;
}

void set_crtc_reg(int row, int col)
{
    uint16_t cursor_pos = CONSOLE_WIDTH * row + col;
    outb(CRTC_IDX_REG, CRTC_CURSOR_LSB_IDX);
    outb(CRTC_DATA_REG, cursor_pos);

    outb(CRTC_IDX_REG, CRTC_CURSOR_MSB_IDX);
    outb(CRTC_DATA_REG, cursor_pos >> 8);
}
void hide_cursor(void)
{
    if (is_cursor_hidden)
        return;

    is_cursor_hidden = 1;
    // Move cursor outside console to "hide"
    set_crtc_reg(CONSOLE_HEIGHT, CONSOLE_WIDTH);
}

void show_cursor(void)
{
    if (!is_cursor_hidden)
        return;

    is_cursor_hidden = 0;
    set_crtc_reg(cursor_row, cursor_col);
}

void draw_char(int row, int col, int ch, int color)
{
    // validation check
    if (!isprint(ch) || row < 0 || row >= CONSOLE_HEIGHT ||
        col < 0 || col >= CONSOLE_WIDTH)
        return;
    char *char_addr = (char *)get_addr_from_index(row, col);
    *char_addr = ch;
    *(char_addr + 1) = color;
}

// clear console and set cursor to beginning
void clear_console(void)
{
    clear_console_from(0, 0);
    set_cursor_helper(0, 0);
}

int set_cursor(int row, int col)
{
    if (row < 0 || row >= CONSOLE_HEIGHT || col < 0 || col >= CONSOLE_WIDTH)
        return -1;
    cursor_row = row;
    cursor_col = col;
    set_crtc_reg(row, col);
    return 0;
}

int print(int size, char *buf)
{
    if (size <= 0 || buf == NULL)
        return -1;
    putbytes(buf, size);
    return 0;
}
