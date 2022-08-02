/**
 * @file curses.c
 * @brief Menu/Ncurses related functions. 
 * @author Nima Behmanesh
 */
#ifndef CURSES_H
#define CURSES_H
#include <stdio.h>
#include <stdlib.h>
#include <ncurses.h>

/** Ncurses Macros **/
#define X_POS_DEFAULT 10
#define Y_POS_DEFAULT 10
#define HEIGHT_DEFAULT 50
#define WIDTH_DEFAULT 50

/** Ncurses Functions **/
int curses_initialize();
void curses_end();
int curses_start();

#endif 
