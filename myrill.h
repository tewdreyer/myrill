/*
    myrill - myrill.h

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

#ifndef _MYRILL_MYRILL_H
#define _MYRILL_MYRILL_H

#define	MAP_HEIGHT		20
#define	MAP_WIDTH		49

#define	MAX(x,y)		((x > y) ? (x) : (y))
#define	MIN(x,y)		((x < y) ? (x) : (y))

struct cell {
    char type;                  /* */
    int flag;                   /* */
};

struct coordinate {
    int y, x;
};

struct items {
    struct items *prev, *next;
    int id;

};

struct gamehandle {
    struct creature *pc;
    struct creatureList *bestiary;
};

extern struct cell map[MAP_HEIGHT][MAP_WIDTH],
    level[MAP_HEIGHT][MAP_WIDTH];
extern int steps[MAP_HEIGHT][MAP_WIDTH];

#endif
