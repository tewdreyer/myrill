/*
    myrill - myrill.c

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

/********************************************
** myrill.c is the main program for myrill **
********************************************/
#include <curses.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include "mechanics.h"
#include "myrill.h"
#include "backtracking.h"
#include "creatures.h"
#include "bsp.h"
#include "save_load.h"
#include "output.h"

#define DEBUG

struct cell level[MAP_HEIGHT][MAP_WIDTH];

int can_reach(struct creature *source, struct creature *target)
{
    int i, j, fx, fy, tx, ty, dx, dy, range;
    if(source == NULL
    || target == NULL)
        return (0);
    fx = source->position.x;
    fy = source->position.y;
    tx = target->position.x;
    ty = target->position.y;
    dx = fx - tx;
    dy = fy - ty;
    range = source->range;
    
    for (i = -1 * range; i <= range; i++)
        for (j = -1 * range; j <= range; j++)
            return (sqrt(dx*dx + dy*dy) <= range);
    return (0);
}

void all_mapped()
{
    int i, j;
    
    for(i = 0; i < MAP_HEIGHT; i++)
        for(j = 0; j < MAP_WIDTH; j++)
            mark_mapped(i, j);
}

int main(void)
{
    int y, x;
    struct gamehandle *myrill;
    struct creatureListItem *cList;
    int turn = 1;
    int k;                      /* keyboard input */
    int direction = 0;
    
    myrill = calloc(1,sizeof(struct gamehandle *));
    if(myrill == NULL){
        perror("cannot allocate memory for gamehandle ..\n");
        exit(0);
    }
    
    initialize();
    start_game(myrill);
    /*
     ** turn is set to pc's 'lifetime'
     */
    turn = myrill->pc->turns_survived;
    erase();
    while (1) {
        myrill->pc->turns_survived = turn;
        draw_array(myrill, *level, MAP_HEIGHT, MAP_WIDTH);
        k = getch();
        /*
         ** clear the fight message window
         */
        werase(fightmes);
        wrefresh(fightmes);
        switch (k) {
            case KEY_UP:
            case KEY_DOWN:
            case KEY_LEFT:
            case KEY_RIGHT:
                myrill->pc->position = move_creature(myrill, myrill->pc, k);
                break;
            case 'Q':
                quit_game(myrill);
                free(myrill);
                exit(0);
                break;
            case 'S':
                save_game(myrill);
                quit_game(myrill);
                free(myrill);
                exit(0);
                break;
            case '>':
                if ((level[myrill->pc->position.y][myrill->pc->position.x].flag &
                     F_EXIT) == F_EXIT) {
                    new_dungeon(myrill);
                }
                break;
#ifdef DEBUG
            case 'h':                                       /* just for testing the game */
                myrill->pc->hp_left = myrill->pc->hp_max;   /* definitely removed in myrill-1.0 ;-) */
                continue;
                break;
            case 'm':
                all_mapped();
                continue;
                break;
            case 'r':
                break;
#endif
            default:
                continue;
                break;
        }
        set_creature_in_level(myrill->pc);
        cList = myrill->bestiary->first;
        while (cList != NULL && cList->creature != NULL) {
            if (can_reach(cList->creature,myrill->pc) == 1) {
                direction = find_cheapest_way(cList->creature->position, myrill->pc->position);
            } else {
                if (rand() % 2 == 1) {
                    y = -1 + 2 * (rand() % 2);
                    if (y == -1)
                        direction = KEY_UP;
                    if (y == 1)
                        direction = KEY_DOWN;
                } else {
                    x = -1 + 2 * (rand() % 2);
                    if (x == -1)
                        direction = KEY_LEFT;
                    if (x == 1)
                        direction = KEY_RIGHT;
                }
            }
            cList->creature->position = move_creature(myrill, cList->creature, direction);
            set_creature_in_level(cList->creature);
            cList = cList->next;
        }
        turn++;

    }
    quit_game(myrill);
    free(myrill);
    return (0);
}
