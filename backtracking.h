/*
    myrill - backtracking.h

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

#ifndef _MYRILL_BACKTRACKING_H
#define _MYRILL_BACKTRACKING_H

#include "myrill.h"

#define	INFINITE	-1

typedef int int_arr_mapsize_t[MAP_HEIGHT][MAP_WIDTH];

extern int_arr_mapsize_t *compute_steps(int y, int x, int_arr_mapsize_t *);
extern int find_cheapest_way(struct coordinate from, struct coordinate to);

#endif
