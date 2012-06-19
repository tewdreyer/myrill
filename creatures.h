/*
    myrill - creatures.h

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

#ifndef _MYRILL_CREATURES_H
#define _MYRILL_CREATURES_H

#define	PC_SYM			'@'
#define	PC_RANGE		4
#define	BAT_SYM			'b'
#define	BAT_EXP			1
#define	BAT_STR			1
#define	BAT_HP			5
#define	BAT_RANGE		4
#define	BAT_LVL			1
#define	GOBLIN_SYM		'g'
#define	GOBLIN_EXP		5
#define	GOBLIN_STR		5
#define	GOBLIN_HP		10
#define	GOBLIN_RANGE		3
#define	GOBLIN_LVL		2
#define	SNAKE_SYM		's'
#define	SNAKE_EXP		5
#define	SNAKE_STR		5
#define	SNAKE_HP		10
#define	SNAKE_RANGE		2
#define	SNAKE_LVL		2
#define	DRAGON_SYM		'D'
#define	DRAGON_EXP		10
#define	DRAGON_STR		10
#define	DRAGON_HP		20
#define	DRAGON_RANGE		10
#define	DRAGON_LVL		4

#define	EXP_BASE		10
#define	EXP_STP_PER_LVL		5

#define	MAX_LENGTH_NAME		10

/**********************************************
** list handle used to present the creatures **
** populating each dungeon                   **
**********************************************/
struct creatureList {
    struct creatureListItem *first; /* first item of creatureList */
    struct creatureListItem *last;  /* last item of creatureList */
};

/*********************************************
** list item which represents a creature in **
** the creature list                        **
*********************************************/
struct creatureListItem {
    struct creatureListItem *next;
    struct creatureListItem *prev;
    struct creature *creature;
};

/*********************************************
** creature, same struct is used for player **
** and monsters                             **
*********************************************/
struct creature {
    int id;                       /* internal identifier */
    char name[MAX_LENGTH_NAME + 1],   /* creatures name */
     race[MAX_LENGTH_NAME + 1],       /* creatures race, a monsters race is its name */
     profession[MAX_LENGTH_NAME + 1], /* creatures profession, only for pc */
     symbol;                      /* creatures symbol displayed on the map */
    int level,                    /* creatures level */
     experience,                  /* creatures experience, if creatures is killed attacker gains whole
                                   * amount of ep creature gained during game*/
     strength,                    /* creatures strength */
     hp_max,                      /* creatures maximum amount of hp, increases with levelup */
     hp_left,                     /* creatures left hp, hp_left <= hp_max */
     range,                       /* range of view, radius of the circle in which creatures respond to
                                   * their environment */
     turns_survived;              /* number of turns the creature survived in the game, the games current
                                   * turn is the same like the players survived turns */
    struct coordinate position;   /* creatures position on the map */
};

extern struct creatureList *new_creature_list(void);
extern struct creatureListItem *new_creature_list_item(void);
extern struct creature *create_new_creature(void);
extern struct creatureList *create_monster_list(int number);
extern void create_pc(struct gamehandle *myrill);
extern void free_list_handle(struct creatureList *handle);
extern void free_list(struct creatureListItem *List);
extern void free_list_item(struct creatureListItem *item);
extern void free_creature(struct creature *c);

#endif
