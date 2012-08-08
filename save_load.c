/*
    myrill - save_load.c

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

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "myrill.h"
#include "creatures.h"
#include "bsp.h"
#include "save_load.h"
#include "output.h"
#include "mechanics.h"

void save_game(struct gamehandle *myrill)
{
    struct creatureListItem *cListItem;

    int y = 0, x = 0;
    FILE *save;

    save = fopen(SAVE_GAME_FILE, "w");

    fprintf(save, "-map start-\n");
    for (y = 0; y < MAP_HEIGHT; y++) {
        for (x = 0; x < MAP_WIDTH; x++) {
            fprintf(save, "%02d", level[y][x].flag);
        }
        fprintf(save, "\n");
    }
    fprintf(save, "-map end-\n");
    fprintf(save, "-pc start-\n");
    fprintf(save, "%s:%s:%s:%c:%d:%d:%d:%d:%d:%d:%d:%d:%d:\n", myrill->pc->name,
            myrill->pc->race, myrill->pc->profession, myrill->pc->symbol, myrill->pc->level,
            myrill->pc->experience, myrill->pc->strength, myrill->pc->hp_max, myrill->pc->hp_left,
            myrill->pc->range, myrill->pc->turns_survived, myrill->pc->position.y,
            myrill->pc->position.x);
    fprintf(save, "-pc end-\n");
    fprintf(save, "-creatures start-\n");
    for (cListItem = myrill->bestiary->first; cListItem != NULL; cListItem = cListItem->next)
        fprintf(save, "%d:%s:%s:%c:%d:%d:%d:%d:%d:%d:%d:%d:\n",
                cListItem->creature->id, cListItem->creature->name, cListItem->creature->race,
                cListItem->creature->symbol, cListItem->creature->level, cListItem->creature->experience,
                cListItem->creature->strength, cListItem->creature->hp_max, cListItem->creature->hp_left,
                cListItem->creature->range, cListItem->creature->position.y,
                cListItem->creature->position.x);
    fprintf(save, "-creatures end-\n");
    fclose(save);
}

void read_pc(struct gamehandle *myrill,char line[80])
{
    char *temp;
    temp = strtok(line, ":");
    if (temp != NULL)
        strncpy(myrill->pc->name, temp, LENGTH(myrill->pc->name, char));
    temp = strtok(NULL, ":");
    if (temp != NULL)
        strncpy(myrill->pc->race, temp, LENGTH(myrill->pc->race, char));
    temp = strtok(NULL, ":");
    if (temp != NULL)
        strncpy(myrill->pc->profession, temp, LENGTH(myrill->pc->profession, char));
    temp = strtok(NULL, ":");
    if (temp != NULL)
        myrill->pc->symbol = *temp;
    temp = strtok(NULL, ":");
    if (temp != NULL)
        myrill->pc->level = atoi(temp);
    temp = strtok(NULL, ":");
    if (temp != NULL)
        myrill->pc->experience = atoi(temp);
    temp = strtok(NULL, ":");
    if (temp != NULL)
        myrill->pc->strength = atoi(temp);
    temp = strtok(NULL, ":");
    if (temp != NULL)
        myrill->pc->hp_max = atoi(temp);
    temp = strtok(NULL, ":");
    if (temp != NULL)
        myrill->pc->hp_left = atoi(temp);
    temp = strtok(NULL, ":");
    if (temp != NULL)
        myrill->pc->range = atoi(temp);
    temp = strtok(NULL, ":");
    if (temp != NULL)
        myrill->pc->turns_survived = atoi(temp);
    temp = strtok(NULL, ":");
    if (temp != NULL)
        myrill->pc->position.y = atoi(temp);
    temp = strtok(NULL, ":");
    if (temp != NULL)
        myrill->pc->position.x = atoi(temp);
}

void read_bestiary(struct creature *monster, char line[80])
{
    char *temp;
    temp = strtok(line, ":");
    if (temp != NULL)
        monster->id = atoi(temp);
    temp = strtok(NULL, ":");
    if (temp != NULL)
        strncpy(monster->name, temp, LENGTH(monster->name, char));
    temp = strtok(NULL, ":");
    if (temp != NULL)
        strncpy(monster->race, temp, LENGTH(monster->race, char));
    temp = strtok(NULL, ":");
    if (temp != NULL)
        monster->symbol = *temp;
    temp = strtok(NULL, ":");
    if (temp != NULL)
        monster->level = atoi(temp);
    temp = strtok(NULL, ":");
    if (temp != NULL)
        monster->experience = atoi(temp);
    temp = strtok(NULL, ":");
    if (temp != NULL)
        monster->strength = atoi(temp);
    temp = strtok(NULL, ":");
    if (temp != NULL)
        monster->hp_max = atoi(temp);
    temp = strtok(NULL, ":");
    if (temp != NULL)
        monster->hp_left = atoi(temp);
    temp = strtok(NULL, ":");
    if (temp != NULL)
        monster->range = atoi(temp);
    temp = strtok(NULL, ":");
    if (temp != NULL)
        monster->position.y = atoi(temp);
    temp = strtok(NULL, ":");
    if (temp != NULL)
        monster->position.x = atoi(temp);
}

void read_map(char *line, int y)
{
    int i = 0;
    int flag;
    for (i = 0; i < MAP_WIDTH; i++) {
        sscanf(line + 2 * i, "%02d", &flag);
        map[y][i].flag = flag;
        if ((flag & F_ACCESSIBLE) == 0)
            map[y][i].type = CELL_WALL;
        if ((flag & F_ROOM) != 0)
            map[y][i].type = CELL_ROOM;
        if ((flag & F_PATH) != 0)
            map[y][i].type = CELL_PATH;
        if ((flag & F_EXIT) != 0)
            map[y][i].type = CELL_EXIT;
    }
}

void load_map(FILE * stream)
{
    char line[MAP_WIDTH * 2 + 2];
    int i = 0;
    int done = 0;
    while (done == 0) {
        fgets(line, sizeof(line), stream);
        if (strstr(line, "-map start-") != NULL)
            done = 1;
    }
    done = 0;
    while (done == 0) {
        fgets(line, sizeof(line), stream);
        if (strstr(line, "-map end-") == NULL) {
            read_map(line, i);
            i++;
        } else {
            done = 1;
        }
    }
    memcpy(level, map, sizeof(map));
}

void load_pc(struct gamehandle *myrill, FILE * stream)
{
    char line[80];
    int done = 0;
    while (done == 0) {
        fgets(line, sizeof(line), stream);
        if (strstr(line, "-pc start-") != NULL)
            done = 1;
    }
    myrill->pc = create_new_creature();
    done = 0;
    while (done == 0) {
        fgets(line, sizeof(line), stream);
        if (strstr(line, "-pc end-") == NULL) {
            read_pc(myrill, line);
            set_creature_in_level(myrill->pc);
        } else {
            done = 1;
        }
    }
}



void load_creatures(struct gamehandle *myrill, FILE * stream)
{
    char line[80];
    int done = 0;
    struct creatureListItem *cListItem, *last = NULL, *first = NULL;
    
    while (done == 0) {
        fgets(line, sizeof(line), stream);
        if (strstr(line, "-creatures start-") != NULL)
            done = 1;
    }
    done = 0;
    myrill->bestiary = new_creature_list();
    while (done == 0) {
        fgets(line, sizeof(line), stream);
        if (strstr(line, "-creatures end-") == NULL) {
            cListItem = new_creature_list_item();
            cListItem->creature = create_new_creature();
            read_bestiary(cListItem->creature, line);
            set_creature_in_level(cListItem->creature);
            if (first == NULL)
                first = cListItem;
            if(last != NULL) {
                last->next = cListItem;
                cListItem->prev = last;
            }
            last = cListItem;
            cListItem = cListItem->next;
        } else {
            done = 1;
        }
    }
    myrill->bestiary->first = first;
    myrill->bestiary->last = last;
}

int load_game(struct gamehandle *myrill)
{
    FILE *load;
    load = fopen(SAVE_GAME_FILE, "r");
    if (load == NULL)
        return (0);

    load_map(load);
    load_pc(myrill, load);
    load_creatures(myrill, load);

    fclose(load);
    unlink(SAVE_GAME_FILE);
    return (1);
}

int is_available(const char *file)
{
    struct stat buffer;
    return (1 + stat(file, &buffer));
}

int start_game(struct gamehandle *myrill)
{
    int done = 0;
    while (done == 0) {
        switch (menue(myrill)) {
        case 2:
            done = 1;
            if (is_available(SAVE_GAME_FILE)) {
                load_game(myrill);
                break;
            }
        case 1:
            done = 1;
            myrill->pc = create_new_creature();
            create_pc(myrill);
            new_dungeon(myrill);
            break;
        case 3:
            help_screen();
            break;
        default:
            break;
        }
    }
    return (1);
}

void new_dungeon(struct gamehandle *myrill)
{
    int number_of_monsters = 0;
    struct creatureListItem *cListItem;
    int i;
    int start_pos_y = 0, start_pos_x = 0;
    erase();
    create_new_map();
    memcpy(level, map, sizeof(map));
    number_of_monsters = 1 + rand() % 10;
    if (myrill->bestiary != NULL){
        free_list(myrill->bestiary->first);
        free_list_handle(myrill->bestiary);
        myrill->bestiary = NULL;
    }        
    myrill->bestiary = create_monster_list(number_of_monsters);
    cListItem = myrill->bestiary->first;
    for (i = 1; i <= number_of_monsters; i++) {
        do {
            start_pos_y = rand() % MAP_HEIGHT;
            start_pos_x = rand() % MAP_WIDTH;
        } while ((level[start_pos_y][start_pos_x].flag & F_ROOM) == 0);
        cListItem->creature->position.y = start_pos_y;
        cListItem->creature->position.x = start_pos_x;
        set_creature_in_level(cListItem->creature);
        cListItem = cListItem->next;
    }
    do {
        start_pos_y = rand() % MAP_HEIGHT;
        start_pos_x = rand() % MAP_WIDTH;
    } while ((level[start_pos_y][start_pos_x].flag & F_ROOM) == 0);
    myrill->pc->position.y = start_pos_y;
    myrill->pc->position.x = start_pos_x;
    set_creature_in_level(myrill->pc);
}
