/*
    myrill - mechanics.h

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

#ifndef _MYRILL_MECHANICS_H
#define _MYRILL_MECHANICS_H

#include "myrill.h"
#define	PC_DEAD      2
#define	MONSTER_DEAD 1
#define FIGHT_ERROR -1

extern void mark_mapped(int y, int x);
extern void set_creature_in_level(struct creature *cc);
extern char get_type(int y, int x);
extern void level_up(struct creature *cc);
extern int exp_to_lvl(struct creature *cc);
extern int throw_die(int sides);
extern int fight(struct gamehandle *myrill, struct creature *att, int y, int x);
extern void pc_dies(struct gamehandle *myrill);
extern void kill(struct creatureList *cList, struct creature *creature);
extern struct coordinate move_creature(struct gamehandle *myrill, struct creature *cc, int key);

#endif
