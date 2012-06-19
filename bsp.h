/*
    myrill - bsp.h

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

#ifndef _MYRILL_BSP_H
#define _MYRILL_BSP_H

#define	SD_HORIZONTAL		0
#define	SD_VERTICAL		1
#define	LEAF			1
#define	NODE			-1
#define	ERR_GENERAL		-1

#define	F_WALL			(0)
#define	F_ROOM			(1<<0)
#define	F_PATH			(1<<1)
#define	F_EXIT			(1<<2)
#define	F_MAPPED		(1<<3)
#define	F_VISIBLE		(1<<4)
#define	F_ACCESSIBLE		(F_ROOM | F_PATH | F_EXIT)

#define DEPTH			4
#define	ROOM_H_MIN		2
#define	ROOM_W_MIN		3
#define	WALL			1
#define	DUNGEON_H_MIN		(2 * WALL + ROOM_H_MIN)
#define	DUNGEON_W_MIN		(2 * WALL + ROOM_W_MIN)
#define	DUNGEON_FACTOR		4
#define	CELL_WALL		'#'
#define	CELL_ROOM		'.'
#define	CELL_PATH		'.'
#define	CELL_EXIT		'>'
#define	LX(room)		(room.ul_corner.x)
#define	RX(room)		(room.ul_corner.x + room.size.w - 1)
#define	TY(room)		(room.ul_corner.y)
#define	BY(room)		(room.ul_corner.y + room.size.h - 1)

typedef struct cell map_t[MAP_HEIGHT][MAP_WIDTH];

struct dimension {
    int h, w;
};

struct path {
    struct path *prev,          /* previous waypoint in path */
    *next;                      /* next wp in path */
    struct coordinate pos;      /* current wps coordinate */
    int value;
};

struct sub {
    struct coordinate ul_corner;        /* position of the upper left corner */
    struct dimension size;      /* contains the length of both sides, height and width */
};

struct wp {
    struct coordinate position; /* the waypoints position */
};

struct node {
    struct node *left,          /* left child node */
    *right,                     /* right child node */
    *parent;                    /* parent node */
    struct sub sub_dungeon,     /* whole space in which a room can be placed */
     room;                      /* space placed at random in the sub_dungeon */
    struct wp path[3];          /* array of waypoints which describe the path */
    int number_of_wps,          /* */
     split_direction,           /* */
     depth,                     /* depth in the bsp tree */
     type;                      /* nodes type, describes whether its really a node or or leaf */
};

extern struct node *create_new_node(void);
extern short split_h(struct node *currentnode);
extern short split_v(struct node *currentnode);
extern void split(struct node *currentnode);
extern void copy_parent_to_child(struct node *parentnode,
                                 struct node *childnode);
extern void put_room_in_dungeon(struct node *currentnode);
extern void create_surrounding_boxes(struct node *currentnode);
extern int search_left(int y, struct node *currentnode);
extern int search_right(int y, struct node *currentnode);
extern int search_above(int x, struct node *currentnode);
extern int search_below(int x, struct node *currentnode);
extern void create_paths(struct node *currentnode);
extern void tree_to_map(struct node *currentnode);
extern void init_map(void);
extern void place_exit_in_map(void);
extern void create_new_map(void);
extern void free_bin_tree(struct node *cnode);

#endif
