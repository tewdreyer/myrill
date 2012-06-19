/*
    myrill - mechanics.c

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

#include <stdlib.h>
#include <math.h>
#include "myrill.h"
#include "mechanics.h"
#include "creatures.h"
#include "bsp.h"
#include "output.h"

void mark_mapped(int y, int x)
{
    level[y][x].flag |= F_MAPPED;
}

void DoFov(struct creature *cc, float x, float y)
{
    int i;
    float ox, oy;

    ox = (float) cc->position.x + 0.5f;
    oy = (float) cc->position.y + 0.5f;

    for(i = 0; i < cc->range; i++) {
        level[(int) oy][(int) ox].flag |= F_VISIBLE;

        if (level[(int) oy][(int) ox].type == CELL_WALL) {
            mark_mapped((int) oy, (int) ox);
            return;
        }

        if (level[(int) oy][(int) ox].type == CELL_EXIT)
            mark_mapped((int) oy, (int) ox);

        ox += x;
        oy += y;
    }
}

void FOV(struct creature *cc)
{
    float x, y;
    int i;

    for(i = 0; i < 360; i+=1) {
        x = cos((float) i * 0.01745f);
        y = sin((float) i * 0.01745f);
        DoFov(cc, x, y);
    }
}

void unmark_visibility(void)
{
    int i, j;
    for (i = 0; i < MAP_HEIGHT; i++)
	for (j = 0; j < MAP_WIDTH; j++)
	    level[i][j].flag &= ~F_VISIBLE;
}

void set_creature_in_level(struct creature *cc)
{
    level[cc->position.y][cc->position.x].type = cc->symbol;
    if (cc->symbol == PC_SYM) {
	unmark_visibility();
	FOV(cc);
    }
}

/********************************************
** get the fields type at coordinate (x|Y) **
** IN                                      **
**	int x,y .. the two coordinates     **
** OUT                                     **
**	char    .. characterof the fields  **
**	           type                    **
********************************************/
char get_type(int y, int x)
{
    return (level[y][x].type);
}

/************************************************
** perform a level up on the creature and      **
** increase stats                              **
** IN                                          **
**	struct creature .. pointer to creature **
** OUT                                         **
**	-                                      **
************************************************/
void level_up(struct creature *cc)
{
    cc->strength++;
    cc->hp_max += 5;
    cc->hp_left = cc->hp_max;
    cc->level++;
}

/******************************************************
** check whether the experience point are sufficient **
** for the next level                                **
** IN                                                **
**	struct cerature * .. pointer to creature     **
** OUT                                               **
**	int               .. current or next level   **
******************************************************/
int exp_to_lvl(struct creature *cc)
{
    int exp = cc->experience;
    int clvl = cc->level;
    if (exp <= EXP_BASE * clvl + EXP_STP_PER_LVL * (clvl - 1) * clvl / 2)
	return (clvl);
    return (clvl + 1);
}

/*************************************
** perform a die throw              **
** IN                               **
**	int .. dies number of sides **
** OUT                              **
**	int .. number between 1 and **
**	       <sides>              **
*************************************/
int throw_die(int sides)
{
    return (1 + rand() % sides);
}

/**************************************************
** fight against a creature standing on (x|y)    **
** IN                                            **
**	struct creature * .. attacking creature  **
**	int x,y           .. coordinate on which **
**	                     defender stands     **
** OUT                                           **
**	int               .. status of fight     **
**************************************************/
int fight(struct gamehandle *myrill, struct creature *att, int y, int x)
{
    int dam;
    struct creature *def;
    struct creatureListItem *cList;
    int found = 0;
    if(att == NULL)
        return FIGHT_ERROR;
    if (myrill->pc->position.y == y && myrill->pc->position.x == x) {
	found = 1;
	def = myrill->pc;
    } else {
	cList = myrill->bestiary->first;
	while (found == 0) {
	    if (cList->creature->position.y == y
		&& cList->creature->position.x == x)
		found = 1;
	    else
		cList = cList->next;
	}
	def = cList->creature;
    }
    if (throw_die(20) + att->strength - def->strength >= def->strength) {
	dam = rand() % (att->strength + 1);
	def->hp_left -= dam;
        if (def == myrill->pc)
            mvwprintw(fightmes, 1, 0, "%s hits. You lost %d hp.",
	              att->name, dam);
        if (att == myrill->pc)
            mvwprintw(fightmes, 0, 0, "You hit. %s lost %d hp.",
		      def->name, dam);
    } else {
	if (def == myrill->pc)
            mvwprintw(fightmes, 1, 0, "%s's hit fails.", att->name);
        if (att == myrill->pc)
	    mvwprintw(fightmes, 0, 0, "Your hit fails.");
    }
    wrefresh(fightmes);
    if (def->hp_left <= 0) {
	switch (def->symbol) {
	case '@':
	    return (PC_DEAD);
	    break;
	default:
	    att->experience += def->experience;
	    kill(myrill->bestiary,def);
	    while (att->level + 1 ==
		   exp_to_lvl(att)) {
		level_up(att);
	    }
	    return (MONSTER_DEAD);
	    break;
	}
    }
    return (0);
}

/*************************************
** shows kill screen and quits game **
** IN                               **
**	-                           **
** OUT                              **
**	-                           **
*************************************/
void pc_dies(struct gamehandle *myrill)
{
    erase();
    mvprintw(0, 0, "You were killed");
    refresh();
    getch();
    quit_game(myrill);
}

/********************************************************
** removes a creature from the list and frees related  **
** memory                                              **
** IN                                                  **
**	struct creature *     .. pointer to creature   **
**	                         which will be removed **
**	struct creatureList * .. pointer to list from  **
**	                         which creature will   **
**	                         be removed            **
** OUT                                                 **
**	-                                              **
********************************************************/
void kill(struct creatureList *cList, struct creature *creature)
{
    struct creatureListItem *curr;
    if(cList == NULL
    || cList->first == NULL
    || creature == NULL)
        return;
    curr = cList->first;
    while(curr != NULL) {
        if(curr->creature->id == creature->id)
            break;
        curr = curr->next;
    }
    if (curr->prev == NULL) {
        cList->first = curr->next;
	if (curr->next == NULL) {
            cList->last = NULL;
	} else {
	    curr->next->prev = NULL;
            curr->next = NULL;
	}
    } else {
	if (curr->next == NULL) {
	    curr->prev->next = NULL;
            curr->prev = NULL;
	} else {
	    curr->prev->next = curr->next;
	    curr->next->prev = curr->prev;
            curr->next = NULL;
            curr->prev = NULL;
	}
    }
    free_list_item(curr);
}

/*********************************************************
** moves the creature into a specific direction         **
** IN                                                   **
**	struct creature * .. pointer to moving creature **
**	int key           .. direction key              **
** OUT                                                  **
**	struct coordinate .. creatures new coordinate   **
*********************************************************/
struct coordinate move_creature(struct gamehandle *myrill, struct creature *cc, int key)
{
    struct coordinate direction;
    int y = cc->position.y, x = cc->position.x;

    direction.y = direction.x = 0;
    switch (key) {
    case KEY_UP:
	direction.y = -1;
	break;
    case KEY_DOWN:
	direction.y = 1;
	break;
    case KEY_LEFT:
	direction.x = -1;
	break;
    case KEY_RIGHT:
	direction.x = 1;
	break;
    }
    if ((level[y + direction.y][x + direction.x].flag & F_ACCESSIBLE) != 0) {
	switch (get_type(y + direction.y, x + direction.x)) {
	case CELL_ROOM:
	case CELL_EXIT:
	    level[y][x].type = map[y][x].type;
	    y += direction.y;
	    x += direction.x;
	    break;
	case BAT_SYM:
	case GOBLIN_SYM:
	case SNAKE_SYM:
	case DRAGON_SYM:
	case PC_SYM:
	    switch (fight(myrill, cc, y + direction.y, x + direction.x)) {
	    case MONSTER_DEAD:
		level[y][x].type = map[y][x].type;
		y += direction.y;
		x += direction.x;
		break;
	    case 0:
		break;
	    case PC_DEAD:
		pc_dies(myrill);
		break;
	    }
	    break;
	}
    }
    cc->position.y = y;
    cc->position.x = x;
    return (cc->position);
}
