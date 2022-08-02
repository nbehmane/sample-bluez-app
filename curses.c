/**
 * @file curses.c
 * @brief Menu/Ncurses related functions. 
 * @author Nima Behmanesh
 */
#include "curses.h"
#define DEBUG 1

static WINDOW *menu_win = NULL;
static WINDOW *term_win = NULL; 

int curses_initialize()
{
   int height = 0;
   int width = 0;

   // Initialize screen
   initscr();

   // Allows exit by ctrl-c
   cbreak(); 
   noecho();
   getmaxyx(stdscr, height, width);

   menu_win = newwin(height >> 1, width, 0, 0);
   term_win = newwin(height >> 1, width, height >> 1, 0);
   refresh();

   box(menu_win, 0, 0);
   mvwprintw(menu_win, 0, 0, "Menu");

   box(term_win, 0, 0);
   mvwprintw(term_win, 0, 0, "Stat");

   wrefresh(menu_win);
   wrefresh(term_win);

   return 0;
}

void curses_end()
{
   // deallocates memory
   endwin();
}

int curses_start()
{
   char input = 0;

   while (1)
   {
      input = wgetch(menu_win);
      mvwprintw(menu_win, 1, 1, "%c", input);
      wrefresh(menu_win);
      wrefresh(term_win);
   }
   return 0;
}
