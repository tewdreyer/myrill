/*
    myrill - output.h

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

#ifndef _MYRILL_OUTPUT_H
#define _MYRILL_OUTPUT_H

#include <curses.h>

extern WINDOW *statuswin, *fightmes, *mapwin, *debugwin;

extern void draw_array(struct gamehandle *myrill, struct cell *array, int height, int width);
extern void initialize(void);
extern void draw_status_line(struct gamehandle *myrill);
extern int menue(struct gamehandle *myrill);
extern void quit_game(struct gamehandle *myrill);
extern void help_screen(void);

#endif
