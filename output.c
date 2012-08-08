/*
    myrill - output.c

    Copyright (C) 2011 Tobias Dreyer <dreyer@molgen.mpg.de>
    Copyright (C) 2011 Max Planck Institut for Molecular Genetics

    myrill is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    myrill is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

/*****************************************************
** output.c contains all functions which handle the **
** graphical output of myrill			    **
**--------------------------------------------------**
** FUNCTIONS					    **
**	draw_array(*array_ptr, height, width)	    **
**	initialize()				    **
**	draw_status_line(int ypos)		    **
**	help_screen(void)			    **
**	menue(void)				    **
**	quit_game(void)				    **
*****************************************************/
#include <time.h>
#include <stdlib.h>
#include "myrill.h"
#include "creatures.h"
#include "bsp.h"
#include "output.h"

WINDOW *statuswin,              /* all important char infos are here */
*fightmes,                      /* messages who hit whom for how much hp */
*mapwin,                        /* the map is displayed in here */
*debugwin;

/***********************************************************************
** draws an array passed to draw_array(...) from 0,0 to height,width  **
***********************************************************************/
void draw_array(struct gamehandle *myrill, struct cell *array, int height, int width)
{
    int y, x;
    werase(mapwin);
    werase(statuswin);
    /*
     ** draw the array
     */
    for (y = 0; y < height; y++) {
        for (x = 0; x < width; x++) {
            if ((array[y * width + x].flag & (F_MAPPED | F_VISIBLE)) != 0)
                mvwprintw(mapwin, y, x, "%c", array[y * width + x].type);
        }
    }
    /*
     ** draw status line
     */
    draw_status_line(myrill);
    /*
     ** update the curses window
     */
    move(0, 0);
    refresh();
    wrefresh(mapwin);
    wrefresh(statuswin);
}

/******************************************************
** function initializes the screen for curses output **
******************************************************/
void initialize(void)
{
    initscr();
    keypad(stdscr, TRUE);
    cbreak();
    noecho();
    srand(time(NULL));
    curs_set(0);
    mapwin = newwin(MAP_HEIGHT, MAP_WIDTH, 0, 0);
    statuswin = newwin(MAP_HEIGHT, 30, 0, MAP_WIDTH + 1);
    fightmes = newwin(2, MAP_WIDTH, MAP_HEIGHT, 0);
    debugwin = newwin(5, MAP_WIDTH + 30, MAP_HEIGHT + 1, 0);
}

/*************************************
** prints the character information **
*************************************/
void draw_status_line(struct gamehandle *myrill)
{
    mvwprintw(statuswin, 0, 0, "Name: %s", myrill->pc->name);
    mvwprintw(statuswin, 1, 0, "Race: %s", myrill->pc->race);
    mvwprintw(statuswin, 2, 0, "Profession: %s", myrill->pc->profession);
    mvwprintw(statuswin, 3, 0, "Char-Level: %d", myrill->pc->level);
    mvwprintw(statuswin, 4, 0, "Experience: %d", myrill->pc->experience);
    mvwprintw(statuswin, 5, 0, "Strength: %d", myrill->pc->strength);
    mvwprintw(statuswin, 6, 0, "HP: %d/%d", myrill->pc->hp_left, myrill->pc->hp_max);
    mvwprintw(statuswin, 8, 0, "Turn: %d", myrill->pc->turns_survived);
}

/**************************
** prints the main menue **
**************************/
int menue(struct gamehandle *myrill)
{
    int input, again = 1;
    erase();
    printw("(N)ew Game\n");
    printw("(L)oad Game\n");
    printw("(H)elp\n");
    printw("(Q)uit\n");
    printw("What do you want to do?");
    refresh();
    while (again) {
        input = getch();
        switch (input) {
        case 'n':
        case 'N':
            again = 0;
            return (1);
            break;
        case 'l':
        case 'L':
            again = 0;
            return (2);
            break;
        case 'h':
        case 'H':
            again = 0;
            return (3);
            break;
        case 'q':
        case 'Q':
            again = 0;
            quit_game(myrill);
            break;
        default:
            continue;
        }
    }
    return (0);
}

/***********************************************
** exit from ncurses window mode and free all **
** allocated memory			      **
***********************************************/
void quit_game(struct gamehandle *myrill)
{
    /*
     ** free all memory
     */
    if (myrill->pc != NULL)
        free_creature(myrill->pc);
    myrill->pc = NULL;
    if (myrill->bestiary != NULL){
        free_list(myrill->bestiary->first);
        free_list_handle(myrill->bestiary);
    }
    delwin(statuswin);
    delwin(mapwin);
    delwin(fightmes);
    delwin(debugwin);
    delwin(stdscr);
    endwin();
}

/***********************************
** prints some hints for the game **
***********************************/
void help_screen(void)
{
    erase();
    mvprintw(0, 0, "- use the arrow/direction keys for movement");
    mvprintw(1, 0, "- melee attack by moving in monsters direction");
    mvprintw(2, 0, "- save with <S>");
    mvprintw(3, 0, "- quit with <Q>");
    refresh();
    getch();
}
