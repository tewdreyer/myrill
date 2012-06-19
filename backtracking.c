/*
    myrill - backtracking.c

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

/*************************************************
** backtracking.c contains all function for	**
** creating a virtual path to a target		**
**----------------------------------------------**
** FUNCTIONS:					**
**	compute_steps(int, int)			**
**	clear_steps()				**
**	mark_target(*creature)			**
**	find_cheapest_way(*coordinate)		**
*************************************************/
#include <string.h>
#include <stdlib.h>
#include <curses.h>
#include "myrill.h"
#include "backtracking.h"
#include "creatures.h"
#include "bsp.h"

/*************************************************
** compute the needed steps to reach the target **
** and save them to steps[][]			**
*************************************************/
int_arr_mapsize_t *compute_steps(int y, int x, int_arr_mapsize_t * array)
{
    int i, j;
    for (i = -1; i <= 1; i++)
        for (j = -1; j <= 1; j++)
            if ((level[y + i][x + j].flag & F_ACCESSIBLE) != 0
                && ((*array)[y + i][x + j] == INFINITE
                    || (*array)[y + i][x + j] > (*array)[y][x] + 1)
                && abs(i + j) == 1) {
                (*array)[y + i][x + j] = (*array)[y][x] + 1;
                compute_steps(y + i, x + j, array);
            }
    return (array);
}

/**************************************************
** find the cheapest way from position to target **
** RETURN:					 **
**	direction of the first step		 **
**************************************************/
int find_cheapest_way(struct coordinate from, struct coordinate to)
{
    int_arr_mapsize_t steps;
    int y = from.y, x = from.x;
    int i, j;
    
    memset(steps, INFINITE, sizeof(steps));
    
    /*
     ** mark target
     */
    steps[to.y][to.x] = 0;
    /*
     ** compute steps
     */
    compute_steps(to.y, to.x, &steps);

    for (i = -1; i <= 1; i++)
        for (j = -1; j <= 1; j++)
            if (abs(i + j) == 1
                && steps[from.y + i][from.x + j] < steps[y][x]
                && steps[from.y + i][from.x + j] != INFINITE) {
                y += i;
                x += j;
            }
    if (y < from.y)
        return (KEY_UP);
    if (y > from.y)
        return (KEY_DOWN);
    if (x < from.x)
        return (KEY_LEFT);
    return (KEY_RIGHT);
}
