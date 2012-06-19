/*
    myrill - save_load.h

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

#ifndef _MYRILL_SAVE_LOAD_H
#define _MYRILL_SAVE_LOAD_H

#define	LENGTH(var, type)	(sizeof(var) / sizeof(type))
#define	SAVE_GAME_FILE	"myrill.save"

extern void save_game(struct gamehandle *myrill);
extern void read_pc(struct gamehandle *myrill,char line[80]);
extern void read_bestiary(struct creature *monster, char line[80]);
extern void read_map(char *line, int y);
extern void load_map(FILE * stream);
extern void load_pc(struct gamehandle *myrill,FILE * stream);
extern void load_creatures(struct gamehandle *myrill,FILE * stream);
extern int load_game(struct gamehandle *myrill);
extern int is_available(const char *file);
extern int start_game(struct gamehandle *myrill);
extern void new_dungeon(struct gamehandle *myrill);

#endif
