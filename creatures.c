/*
    myrill - creatures.c

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

#include <curses.h>
#include <string.h>
#include <stdlib.h>
#include "myrill.h"
#include "creatures.h"

/*****************************************************************
** creates a new creature list handle and initializes it        **
** IN                                                           **
**	-                                                       **
** OUT:                                                         **
**	struct creatureList * .. pointer to creatureList handle **
*****************************************************************/
struct creatureList *new_creature_list(void)
{
    struct creatureList *cList;
    cList = calloc(1,sizeof(struct creatureList));
    if(cList == NULL){
        printw("cannot create creatureList..\n");
        getch();
        exit(1);
    }
    cList->first = NULL;
    cList->last = NULL;
    return(cList);
}

/***********************************************************
** creates a new list item of type struct creature and    **
** initializes it                                         **
** IN                                                     **
**	-                                                 **
** OUT:                                                   **
**	struct creatureListItem * .. pointer to list item **
***********************************************************/
struct creatureListItem *new_creature_list_item(void)
{
    struct creatureListItem *cli;
    cli = calloc(1,sizeof(struct creatureListItem));
    if(cli == NULL){
        printw("cannot create creatureListItem..");
        getch();
        exit(1);
    }
    cli->next = NULL;
    cli->prev = NULL;
    cli->creature = NULL;
    return cli;
}

/**************************************************
** creates a new creature and initializes it     **
** IN                                            **
**	-                                        **
** OUT                                           **
**	struct creature * .. pointer to creature **
**************************************************/
struct creature *create_new_creature(void)
{
    struct creature *newc;
    newc = calloc(1, sizeof(struct creature));
    if (newc == NULL) {
        printw("cannot create creature..\n");
        getch();
        exit(1);
    }
    return (newc);
}

/*********************************************************
** creates a new list, containing <number> monster,     **
** creates those monster, fills them with data and      **
** sets the first and last element in the list handle   **
** IN                                                   **
**	int number .. defines how many monster          **
**	              populate the dungeon              **
** OUT                                                  **
**	struct creatureList * .. pointer to list handle **
*********************************************************/
struct creatureList *create_monster_list(int number)
{
    struct creatureList *cList;
    struct creatureListItem *first, *prev, *curr;
    int i = 0;
    char type[MAX_LENGTH_NAME];
    first = prev = curr = NULL;
    /*
     ** create list of monsters
     */
    cList = new_creature_list();
    for (first = prev = NULL, i = 1; i <= number; prev = curr, i++) {
        curr = new_creature_list_item();
        if (first == NULL)
            first = curr;
        if (prev != NULL) {
            prev->next = curr;
            curr->prev = prev;
        }
        curr->creature = create_new_creature();

        /*
         ** initialize monster
         */
        switch (1 + rand() % 4) {
        case 1:
            strcpy(type, "Bat");
            curr->creature->symbol = BAT_SYM;
            curr->creature->level = BAT_LVL;
            curr->creature->experience = BAT_EXP;
            curr->creature->strength = BAT_STR;
            curr->creature->hp_max = BAT_HP;
            curr->creature->range = BAT_RANGE;
            break;
        case 2:
            strcpy(type, "Goblin");
            curr->creature->symbol = GOBLIN_SYM;
            curr->creature->level = GOBLIN_LVL;
            curr->creature->experience = GOBLIN_EXP;
            curr->creature->strength = GOBLIN_STR;
            curr->creature->hp_max = GOBLIN_HP;
            curr->creature->range = GOBLIN_RANGE;
            break;
        case 3:
            strcpy(type, "Snake");
            curr->creature->symbol = SNAKE_SYM;
            curr->creature->level = SNAKE_LVL;
            curr->creature->experience = SNAKE_EXP;
            curr->creature->strength = SNAKE_STR;
            curr->creature->hp_max = SNAKE_HP;
            curr->creature->range = SNAKE_RANGE;
            break;
        case 4:
            strcpy(type, "Dragon");
            curr->creature->symbol = DRAGON_SYM;
            curr->creature->level = DRAGON_LVL;
            curr->creature->experience = DRAGON_EXP;
            curr->creature->strength = DRAGON_STR;
            curr->creature->hp_max = DRAGON_HP;
            curr->creature->range = DRAGON_RANGE;
            break;
        }
        curr->creature->id = i;
        strcpy(curr->creature->name, type);
        strcpy(curr->creature->race, type);
        strcpy(curr->creature->profession, type);
        curr->creature->hp_left = curr->creature->hp_max;
    }
    cList->first = first;
    cList->last = curr;
    return (cList);
}

/**************************************************
** print some races and read input from keyboard **
** IN                                            **
**	-                                        **
** OUT                                           **
**	char .. character pressed on keyborad    **
**************************************************/
char choose_race(void)
{
    erase();
    mvprintw(0, 0, "a..Human");
    mvprintw(0, 20, "b..Dwarf");
    mvprintw(0, 60, "c..Gnome");
    mvprintw(2, 0, "Choose your race: ");
    refresh();
    return (getch());
}

/***********************************************
** print some professions and read input from **
** keyboard                                   **
** IN                                         **
**	-                                     **
** OUT                                        **
**	char .. character preesed on keyboard **
***********************************************/
char choose_profession(void)
{
    erase();
    mvprintw(0, 0, "a..Monk");
    mvprintw(0, 20, "b..Ranger");
    mvprintw(0, 60, "c..Mage");
    mvprintw(2, 0, "Choose your profession: ");
    refresh();
    return (getch());
}

/**********************************
** create a new player character **
** IN                            **
**	-                        **
** OUT                           **
**	-                        **
**********************************/
void create_pc(struct gamehandle *myrill)
{
    char race, profession;
    int done = 0;
    /*
     ** read players name and cut it if input is too long
     */
    nocbreak();
    echo();
    do {
        erase();
        printw("Please enter players name: ");
        refresh();
        getnstr(myrill->pc->name, MAX_LENGTH_NAME);
    } while (strlen(myrill->pc->name) == 0);
    myrill->pc->name[MAX_LENGTH_NAME] = '\0';
    cbreak();
    noecho();
    /*
     ** get players race
     */
    done = 0;
    while (done == 0) {
        race = choose_race();
        switch (race) {
        case 'a':
        case 'A':
            strcpy(myrill->pc->race, "Human");
            done = 1;
            break;
        case 'b':
        case 'B':
            strcpy(myrill->pc->race, "Dwarf");
            done = 1;
            break;
        case 'c':
        case 'C':
            strcpy(myrill->pc->race, "Gnome");
            done = 1;
            break;
        default:
            break;
        }
    }
    /*
     ** get players profession
     */
    done = 0;
    while (done == 0) {
        profession = choose_profession();
        switch (profession) {
        case 'a':
        case 'A':
            strcpy(myrill->pc->profession, "Monk");
            done = 1;
            break;
        case 'b':
        case 'B':
            strcpy(myrill->pc->profession, "Ranger");
            done = 1;
            break;
        case 'c':
        case 'C':
            strcpy(myrill->pc->profession, "Mage");
            done = 1;
            break;
        default:
            break;
        }
    }
    myrill->pc->symbol = PC_SYM;
    myrill->pc->level = 1;
    myrill->pc->experience = 0;
    myrill->pc->strength = 10;
    myrill->pc->hp_max = 10;
    myrill->pc->hp_left = myrill->pc->hp_max;
    myrill->pc->range = PC_RANGE;
    myrill->pc->turns_survived = 0;
}

/*********************************************************
** frees the memory pointed to by the list handle       **
** IN                                                   **
**	struct creatureList * .. pointer to list handle **
** OUT                                                  **
**	-                                               **
*********************************************************/
void free_list_handle(struct creatureList *handle)
{
    if(handle != NULL){
        free(handle);
        handle = NULL;
    }
}

/***********************************************************
** from current list item free recursivley the creature   **
** list                                                   **
** IN                                                     **
**	struct creatureListItem * .. pointer to list item **
** RETURN                                                 **
**	-                                                 **
***********************************************************/
void free_list(struct creatureListItem *cList)
{
    if (cList != NULL){
        free_list(cList->next);
        cList->next = NULL;
    }
    free_list_item(cList);
    cList = NULL;
}

/*********************************************************
** free memory of list item                             **
** IN                                                   **
**	struct creatureListItem .. pointer to list item **
** OUT                                                  **
**	-                                               **
*********************************************************/
void free_list_item(struct creatureListItem *item)
{
    if(item != NULL) {
        free_creature(item->creature);
        item->creature = NULL;
    }
    free(item);
    item = NULL;
}

/**************************************************
** free memory pointed to by creature            **
** IN                                            **
**	struct creature * .. pointer to creature **
** OUT                                           **
**	-                                        **
**************************************************/
void free_creature(struct creature *c)
{
    free(c);
    c = NULL;
}
