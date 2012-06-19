/*
    myrill - bsp.c

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

/***************************************************
** bsp.c contains all function for creating a map **
** with a binary space paritioning algorithm	  **
**------------------------------------------------**
** FUNCTIONS:					  **
**	node *create_new_node()			  **
**	split(*node)				  **
**	int split_h(*node)			  **
**	int split_v(*node)			  **
**	copy_parent_to_child(*node, *node)	  **
**	put_room_in_dungeon(*node)		  **
**	create_surrounding_boxes(*node)		  **
**	int search_left(int, *node)		  **
**	int search_right(int, *node)		  **
**	int search_above(int, *node)		  **
**	int search_below(int, *node)		  **
**	create_paths(*node)			  **
**	tree_to_map(*node)			  **
**	init_map()				  **
**	place_exit_in_map()			  **
**	create_new_map()			  **
**	free_bin_tree(*node)			  **
***************************************************/
#include <stdio.h>
#include <stdlib.h>
#include "myrill.h"
#include "bsp.h"

map_t map;

/*****************************************
** create a new node and initializes it	**
**					**
** RETURN				**
**	nnode: pointer to the new node	**
*****************************************/
struct node *create_new_node(void)
{
    struct node *nnode;
    /*
     ** get memory for new node and exit when fail
     */
    nnode = malloc(sizeof(struct node));
    if (nnode == NULL) {
        printf("Cannot create new node...");
        getchar();
        exit(0);
    }
    /*
     ** initialize the nodes pointer
     */
    nnode->left = NULL;
    nnode->right = NULL;
    /*
     ** assign nodes type to leaf, because both children point to NULL
     */
    nnode->type = LEAF;
    nnode->number_of_wps = 0;
    return (nnode);
}

/*****************************************************************
** tries to split the space stored in *cnode horizontal		**
**								**
** RETURN							**
**	0: ERROR, if space cannot or need not be split anymore	**
**	1: split succeeded					**
*****************************************************************/
short split_h(struct node *cnode)
{
    int y = 0;
    /*
     ** do not split if dimension is too small or tree is deep enough
     */
    if (cnode->sub_dungeon.size.h < 2 * DUNGEON_H_MIN ||
        cnode->sub_dungeon.size.h * cnode->sub_dungeon.size.w <
        DUNGEON_FACTOR * DUNGEON_H_MIN * DUNGEON_W_MIN
        || cnode->depth > DEPTH) {
        return (0);
    }
    /*
     ** compute the vertical border
     */
    y = DUNGEON_H_MIN + rand() % (cnode->sub_dungeon.size.h -
                                  2 * DUNGEON_H_MIN + 1);
    /*
     ** create 2 sub nodes and link them to parent
     */
    cnode->left = create_new_node();
    cnode->right = create_new_node();
    cnode->left->parent = cnode;
    cnode->right->parent = cnode;
    /*
     ** copy parents information to child
     */
    copy_parent_to_child(cnode, cnode->left);
    copy_parent_to_child(cnode, cnode->right);
    /*
     ** assign new dimensions to the children
     */
    cnode->left->sub_dungeon.size.h = y;
    cnode->right->sub_dungeon.ul_corner.y =
        cnode->sub_dungeon.ul_corner.y + y;
    cnode->right->sub_dungeon.size.h = cnode->sub_dungeon.size.h - y;
    /*
     ** split both children
     */
    split(cnode->left);
    split(cnode->right);
    /*
     ** reassign cnodes type form leaf to node and save the split direction
     */
    cnode->type = NODE;
    cnode->split_direction = SD_HORIZONTAL;
    return (1);
}

/*****************************************************************
** tries to split the space stored in *cnode vertical		**
**								**
** RETURN							**
**	0: ERROR, if space cannot or need not be split anymore	**
**	1: split succeeded					**
*****************************************************************/
short split_v(struct node *cnode)
{
    int x = 0;
    /*
     ** do not split if dimension is too small or tree is deep enough
     */
    if (cnode->sub_dungeon.size.w < 2 * DUNGEON_W_MIN ||
        cnode->sub_dungeon.size.h * cnode->sub_dungeon.size.w <
        DUNGEON_FACTOR * DUNGEON_H_MIN * DUNGEON_W_MIN
        || cnode->depth > DEPTH) {
        return (0);
    }
    /*
     ** compute the horizontal border
     */
    x = DUNGEON_W_MIN + rand() % (cnode->sub_dungeon.size.w -
                                  2 * DUNGEON_W_MIN + 1);
    /*
     ** create 2 sub nodes and link them to parent
     */
    cnode->left = create_new_node();
    cnode->right = create_new_node();
    cnode->left->parent = cnode;
    cnode->right->parent = cnode;
    /*
     ** copy parents information to child
     */
    copy_parent_to_child(cnode, cnode->left);
    copy_parent_to_child(cnode, cnode->right);
    /*
     ** assign new dimensions to the children
     */
    cnode->left->sub_dungeon.size.w = x;
    cnode->right->sub_dungeon.ul_corner.x =
        cnode->sub_dungeon.ul_corner.x + x;
    cnode->right->sub_dungeon.size.w = cnode->sub_dungeon.size.w - x;
    /*
     ** split both children
     */
    split(cnode->left);
    split(cnode->right);
    /*
     ** reassign cnodes type form leaf to node and save the split direction
     */
    cnode->type = NODE;
    cnode->split_direction = SD_VERTICAL;
    return (1);
}

/****************************************************
** decides whether to split horizontal or vertical **
****************************************************/
void split(struct node *cnode)
{
    int direction = 0;
    /*
     ** decide whether to split horizontal or vertical
     */
    direction = rand() % 2;
    switch (direction) {
        /*
         ** horizontal split
         */
    case 0:
        /*
         ** if horizontal split returns 0, meaning ERROR,
         ** try splitting vertical
         */
        if (!split_h(cnode))
            split_v(cnode);
        break;
        /*
         ** vertical split
         */
    case 1:
        /*
         ** if vertical split returns 0, meaning ERROR,
         ** try splitting horizontal
         */
        if (!split_v(cnode))
            split_h(cnode);
        break;
    }
}

/***************************************************
** the parents information is copied to the child **
***************************************************/
void copy_parent_to_child(struct node *pnode, struct node *chnode)
{
    chnode->sub_dungeon.ul_corner.y = pnode->sub_dungeon.ul_corner.y;
    chnode->sub_dungeon.ul_corner.x = pnode->sub_dungeon.ul_corner.x;
    chnode->sub_dungeon.size.h = pnode->sub_dungeon.size.h;
    chnode->sub_dungeon.size.w = pnode->sub_dungeon.size.w;
    chnode->depth = pnode->depth + 1;
}

/***********************************************************
** go backwards through the tree and put a room in each	  **
** sub dungeon						  **
** FIRST *cnode is *root				  **
***********************************************************/
void put_room_in_dungeon(struct node *cnode)
{
    int uly = 0, ulx = 0, height = 0, width = 0;
    /*
     ** exit if cnode points to NULL
     */
    if (cnode == NULL)
        return;
    /*
     ** test whether node is a leaf
     **     if yes, build at random one room per sub_dungeon region
     **     if not, do this function with the nodes children
     */
    if (cnode->type == LEAF) {
        height =
            DUNGEON_H_MIN + rand() % (cnode->sub_dungeon.size.h -
                                      DUNGEON_H_MIN + 1);
        width =
            DUNGEON_W_MIN + rand() % (cnode->sub_dungeon.size.w -
                                      DUNGEON_W_MIN + 1);
        uly =
            cnode->sub_dungeon.ul_corner.y +
            rand() % (cnode->sub_dungeon.size.h - height + 1);
        ulx =
            cnode->sub_dungeon.ul_corner.x +
            rand() % (cnode->sub_dungeon.size.w - width + 1);
        cnode->room.ul_corner.y = uly + WALL;
        cnode->room.ul_corner.x = ulx + WALL;
        cnode->room.size.h = height - 2 * WALL;
        cnode->room.size.w = width - 2 * WALL;
    } else {
        put_room_in_dungeon(cnode->left);
        put_room_in_dungeon(cnode->right);
    }
}

/**********************************************************
** go backwards through the tree and create boxes around **
** the rooms stored in *cnodes childrens		 **
** FIRST *cnode is *root				 **
**********************************************************/
void create_surrounding_boxes(struct node *cnode)
{
    int ly = 0, lx = 0, lh = 0, lw = 0, ry = 0, rx = 0, rh = 0, rw = 0;
    if (cnode == NULL)
        return;
    if (cnode->type != LEAF) {
        create_surrounding_boxes(cnode->left);
        create_surrounding_boxes(cnode->right);
    }
    if (cnode->type == NODE) {
        ly = cnode->left->room.ul_corner.y;
        lx = cnode->left->room.ul_corner.x;
        lh = cnode->left->room.size.h;
        lw = cnode->left->room.size.w;
        ry = cnode->right->room.ul_corner.y;
        rx = cnode->right->room.ul_corner.x;
        rh = cnode->right->room.size.h;
        rw = cnode->right->room.size.w;
        cnode->room.ul_corner.y = MIN(ly, ry);
        cnode->room.ul_corner.x = MIN(lx, rx);
        cnode->room.size.h = MAX(ly + lh, ry + rh) - MIN(ly, ry);
        cnode->room.size.w = MAX(lx + lw, rx + rw) - MIN(lx, rx);
    }
}

/*********************************************************
** search in *cnodes left sub tree for the most right x	**
** in the y-th row					**
** RETURN						**
**	ERR_GENERAL:	no possible x is found		**
**	x:		nearest room to y-th row	**
*********************************************************/
int search_left(int y, struct node *cnode)
{
    struct sub leftroom, rightroom;
    /*
     ** if *cnode is a leaf, the most right x is the right
     ** border of the room in *cnode
     */
    if (cnode->type == LEAF)
        return (RX(cnode->room) + 1);
    /*
     ** for better readability leftroom and rightroom are assigned
     ** leftroom is the bounding box stored in cnode->left
     ** rightroom analog to leftroom
     */
    leftroom = cnode->left->room;
    rightroom = cnode->right->room;
    /*
     ** check if the y-th row hits the rightroom
     */
    if (TY(rightroom) <= y && y <= BY(rightroom))
        return (search_left(y, cnode->right));
    /*
     ** check if the y-th row hits the leftroom but not
     ** one line over or under the rightroom
     */
    if (TY(leftroom) <= y && y <= BY(leftroom)
        && ((y < TY(rightroom)) ? (y < TY(rightroom) - 1) : (1))
        && ((y > BY(rightroom)) ? (y > BY(rightroom) + 1) : (1)))
        return (search_left(y, cnode->left));
    /*
     ** check if the y-th row is between left- and rightroom
     ** and hits a path
     */
    if ((BY(leftroom) + 1 < y && y < TY(rightroom) - 1) ||
        (BY(rightroom) + 1 < y && y < TY(leftroom) - 1)) {
        if (cnode->path[0].position.y <= y
            && y <= cnode->path[1].position.y)
            return (MAX
                    (cnode->path[0].position.x,
                     cnode->path[1].position.x) + 1);
        if (cnode->path[1].position.y <= y
            && y <= cnode->path[2].position.y)
            return (MAX
                    (cnode->path[1].position.x,
                     cnode->path[2].position.x) + 1);
    }
    return (ERR_GENERAL);
}

/*********************************************************
** search in *cnodes right sub tree for the most left x	**
** in the y-th row					**
** RETURN						**
**	ERR_GENERAL:	no possible x is found		**
**	x:		nearest room to y-th row	**
*********************************************************/
int search_right(int y, struct node *cnode)
{
    struct sub leftroom, rightroom;
    /*
     ** if *cnode is a leaf, the most left x is the left
     ** border of the room in *cnode
     */
    if (cnode->type == LEAF)
        return (LX(cnode->room) - 1);
    /*
     ** for better readability leftroom and rightroom are assigned
     ** leftroom is the bounding box stored in cnode->left
     ** rightroom analog to leftroom
     */
    leftroom = cnode->left->room;
    rightroom = cnode->right->room;
    /*
     ** check if the y-th row hits the leftroom
     */
    if (TY(leftroom) <= y && y <= BY(leftroom))
        return (search_right(y, cnode->left));
    /*
     ** check if the y-th row hits the rightroom
     ** but not one line over or under leftroom
     */
    if (TY(rightroom) <= y && y <= BY(rightroom)
        && ((y < TY(leftroom)) ? (y < TY(leftroom) - 1) : (1))
        && ((y > BY(leftroom)) ? (y > BY(leftroom) + 1) : (1)))
        return (search_right(y, cnode->right));
    /*
     ** check if the y-th row is between left- and rightroom
     ** and hits a path
     */
    if (BY(leftroom) + 1 < y && y < TY(rightroom) - 1) {
        if (cnode->path[0].position.y <= y
            && y <= cnode->path[1].position.y)
            return (cnode->path[0].position.x - 1);
        if (cnode->path[1].position.y <= y
            && y <= cnode->path[2].position.y)
            return (cnode->path[1].position.x - 1);
    }
    if (BY(rightroom) + 1 < y && y < TY(leftroom) - 1) {
        if (cnode->path[0].position.y <= y
            && y <= cnode->path[1].position.y)
            return (cnode->path[0].position.x - 1);
        if (cnode->path[1].position.y <= y
            && y <= cnode->path[2].position.y)
            return (cnode->path[1].position.x - 1);
    }
    return (ERR_GENERAL);
}

/*********************************************************
** search in *cnodes left sub tree for the lowest y	**
** in the x-th collumn					**
** RETURN						**
**	ERR_GENERAL:	no possible y is found		**
**	x:		nearest room to x-th collumn	**
*********************************************************/
int search_above(int x, struct node *cnode)
{
    struct sub leftroom, rightroom;
    /*
     ** if *cnode is a leaf, the lowest y is the bottom
     ** border of the room in *cnode
     */
    if (cnode->type == LEAF)
        return (BY(cnode->room) + 1);
    /*
     ** for better readability leftroom and rightroom are assigned
     ** leftroom is the bounding box stored in cnode->left
     ** rightroom analog to leftroom
     */
    leftroom = cnode->left->room;
    rightroom = cnode->right->room;
    /*
     ** check if the x-th collumn hits rightroom
     */
    if (LX(rightroom) <= x && x <= RX(rightroom))
        return (search_above(x, cnode->right));
    /*
     ** check if the x-th collumn hits the leftroom
     ** but not one collumn left or right of rightroom
     */
    if (LX(leftroom) <= x && x <= RX(leftroom)
        && ((x < LX(rightroom)) ? (x < LX(rightroom) - 1) : (1))
        && ((x > RX(rightroom)) ? (x > RX(rightroom) + 1) : (1)))
        return (search_above(x, cnode->left));
    /*
     ** check if the x-th collumn is between left- and rightroom
     ** and hits a path
     */
    if (RX(leftroom) + 1 < x && x < LX(rightroom) - 1) {
        if (cnode->path[0].position.x <= x
            && x <= cnode->path[1].position.x)
            return (cnode->path[0].position.y + 1);
        if (cnode->path[1].position.x <= x
            && x <= cnode->path[2].position.x)
            return (cnode->path[1].position.y + 1);
    }
    if (RX(rightroom) + 1 < x && x < LX(leftroom) - 1) {
        if (cnode->path[0].position.x <= x
            && x <= cnode->path[1].position.x)
            return (cnode->path[0].position.y + 1);
        if (cnode->path[1].position.x <= x
            && x <= cnode->path[2].position.x)
            return (cnode->path[1].position.y + 1);
    }
    return (ERR_GENERAL);
}

/*********************************************************
** search in *cnodes right sub tree for the highest y	**
** in the x-th collumn					**
** RETURN						**
**	ERR_GENERAL:	no possible y is found		**
**	x:		nearest room to x-th collumn	**
*********************************************************/
int search_below(int x, struct node *cnode)
{
    struct sub leftroom, rightroom;
    /*
     ** if *cnode is a leaf, the highest y is the top
     ** border of the room in *cnode
     */
    if (cnode->type == LEAF)
        return (TY(cnode->room) - 1);
    /*
     ** for better readability leftroom and rightroom are assigned
     ** leftroom is the bounding box stored in cnode->left
     ** rightroom analog to leftroom
     */
    leftroom = cnode->left->room;
    rightroom = cnode->right->room;
    /*
     ** check if the x-th collumn hits leftroom
     */
    if (LX(leftroom) <= x && x <= RX(leftroom))
        return (search_below(x, cnode->left));
    /*
     ** check if the x-th collumn hits rightroom
     ** but not one collumn left or right of leftroom
     */
    if (LX(rightroom) <= x && x <= RX(rightroom)
        && ((x < LX(leftroom)) ? (x < LX(leftroom) - 1) : (1))
        && ((x > RX(leftroom)) ? (x > RX(leftroom) + 1) : (1)))
        return (search_below(x, cnode->right));
    /*
     ** check if the x-th collumn is between left- and rightroom
     ** and hits a path
     */
    if (RX(leftroom) + 1 < x && x < LX(rightroom) - 1) {
        if (cnode->path[0].position.x <= x
            && x <= cnode->path[1].position.x)
            return (cnode->path[0].position.y - 1);
        if (cnode->path[1].position.x <= x
            && x <= cnode->path[2].position.x)
            return (cnode->path[1].position.y - 1);
    }
    if (RX(rightroom) + 1 < x && x < LX(leftroom) - 1) {
        if (cnode->path[0].position.x <= x
            && x <= cnode->path[1].position.x)
            return (cnode->path[0].position.y - 1);
        if (cnode->path[1].position.x <= x
            && x <= cnode->path[2].position.x)
            return (cnode->path[1].position.y - 1);
    }
    return (ERR_GENERAL);
}

/*******************************************************
** go backwards through the tree an connect the rooms **
** FIRST *cnode is *root			      **
*******************************************************/
void create_paths(struct node *cnode)
{
    /*
     ** rand_x is a possible collumn for path connecting two rooms, next_y is the nearest line to rnad_x where the path will end
     ** same with rand_y and next_x
     */
    int rand_x = 0, next_y = 0, rand_y = 0, next_x = 0;
    int ly = 0, lx = 0, lh = 0, lw = 0, ry = 0, rx = 0, rh = 0, rw = 0;
    /*
     ** if a path is possible, the 4 ok_*s are greater then 0 and contain the nearest x or y positions
     */
    int ok_left = 0, ok_right = 0, ok_above = 0, ok_below = 0;
    /*
     ** go back to leafs parent
     */
    if (cnode->type == LEAF)
        return;
    /*
     ** go through tree
     */
    create_paths(cnode->left);
    create_paths(cnode->right);
    /*
     ** for better readability new variables are assigned
     */
    ly = cnode->left->room.ul_corner.y;
    lx = cnode->left->room.ul_corner.x;
    lh = cnode->left->room.size.h;
    lw = cnode->left->room.size.w;
    ry = cnode->right->room.ul_corner.y;
    rx = cnode->right->room.ul_corner.x;
    rh = cnode->right->room.size.h;
    rw = cnode->right->room.size.w;
    /*
     ** create paths, depends on split direction
     */
    switch (cnode->split_direction) {
    case SD_HORIZONTAL:
        /*
         ** check if left- and rightroom overlap
         **     yes:    straight path (2 waypoints)
         */
        if (MIN(lx + lw, rx + rw) - MAX(lx, rx) > 0) {
            do {
                rand_x =
                    MAX(lx,
                        rx) + rand() % (MIN(lx + lw, rx + rw) - MAX(lx,
                                                                    rx));
                ok_left = search_above(rand_x, cnode->left);
                ok_right = search_below(rand_x, cnode->right);
            } while (!(ok_left > 0 && ok_right > 0));
            next_y = search_above(rand_x, cnode->left);
            cnode->path[0].position.y = next_y;
            cnode->path[0].position.x = rand_x;
            next_y = search_below(rand_x, cnode->right);
            cnode->path[1].position.y = next_y;
            cnode->path[1].position.x = cnode->path[0].position.x;
            cnode->number_of_wps = 2;
            /*
             **     no:     create path from leftroom to the first row of rightroom
             **             and search for rightroom on the left or right
             **             now the path has 3 waypoints
             */
        } else {
            cnode->path[0].position.y = ly + lh;
            cnode->path[0].position.x =
                (MAX(lx, rx) == rx) ? (lx + lw - 1) : (lx);
            cnode->path[1].position.y = ry;
            cnode->path[1].position.x = cnode->path[0].position.x;
            cnode->path[2].position.y = cnode->path[1].position.y;
            cnode->path[2].position.x = (MAX(lx, rx) == rx)
                ? (search_right(cnode->path[1].position.y, cnode->right))
                : (search_left(cnode->path[1].position.y, cnode->right));
            cnode->number_of_wps = 3;
        }
        break;
    case SD_VERTICAL:
        /*
         ** check if left- and rightroom overlap
         **     yes:    straight path (2 waypoints)
         */
        if (MIN(ly + lh, ry + rh) - MAX(ly, ry) > 0) {
            do {
                rand_y =
                    MAX(ly,
                        ry) + rand() % (MIN(ly + lh, ry + rh) - MAX(ly,
                                                                    ry));
                ok_above = search_left(rand_y, cnode->left);
                ok_below = search_right(rand_y, cnode->right);
            } while (!(ok_above > 0 && ok_below > 0));
            next_x = search_left(rand_y, cnode->left);
            cnode->path[0].position.y = rand_y;
            cnode->path[0].position.x = next_x;
            next_x = search_right(rand_y, cnode->right);
            cnode->path[1].position.y = cnode->path[0].position.y;
            cnode->path[1].position.x = next_x;
            cnode->number_of_wps = 2;
            /*
             **     no:     create path from leftroom to first collumn of rightroom
             **             and search above or below for rightroom
             **             path has 3 waypoints
             */
        } else {
            cnode->path[0].position.y =
                (MAX(ly, ry) == ry) ? (ly + lh - 1) : (ly);
            cnode->path[0].position.x = lx + lw;
            cnode->path[1].position.y = cnode->path[0].position.y;
            cnode->path[1].position.x = rx;
            cnode->path[2].position.y = (MAX(ly, ry) == ry)
                ? (search_below(cnode->path[1].position.x, cnode->right))
                : (search_above(cnode->path[1].position.x, cnode->right));
            cnode->path[2].position.x = cnode->path[1].position.x;
            cnode->number_of_wps = 3;
        }
        break;
    }
}

/************************************************************
** put the information stored in the tree to the map array **
************************************************************/
void tree_to_map(struct node *cnode)
{
    int y, x;
    int r;
    /*
     ** test whether cnode is a leaf
     **     if yes, ...
     **     if not, do this again with cnodes children
     */
    if (cnode->type == NODE) {
        tree_to_map(cnode->left);
        tree_to_map(cnode->right);
        /*
         ** draw paths to map
         */
        for (r = 0; r + 1 < cnode->number_of_wps; r++)
            for (y = cnode->path[r].position.y;
                 ((cnode->path[r].position.y <=
                   cnode->path[r + 1].position.y)
                  ? (y <= cnode->path[r + 1].position.y)
                  : (y >= cnode->path[r + 1].position.y));
                 ((cnode->path[r].position.y <=
                   cnode->path[r + 1].position.y)
                  ? (y++)
                  : (y--)))
                for (x = cnode->path[r].position.x;
                     ((cnode->path[r].position.x <=
                       cnode->path[r + 1].position.x)
                      ? (x <= cnode->path[r + 1].position.x)
                      : (x >= cnode->path[r + 1].position.x));
                     ((cnode->path[r].position.x <=
                       cnode->path[r + 1].position.x)
                      ? (x++)
                      : (x--))) {
                    /*
                     ** set symbol for path
                     */
                    map[y][x].type = CELL_PATH;
                    /*
                     ** set flag for path
                     */
                    map[y][x].flag = F_PATH;
                }
    } else {
        /*
         ** draw rooms to map
         */
        for (y = TY(cnode->room); y <= BY(cnode->room); y++)
            for (x = LX(cnode->room); x <= RX(cnode->room); x++) {
                /*
                 ** set symbol for room
                 */
                map[y][x].type = CELL_ROOM;
                /*
                 ** set flag for room
                 */
                map[y][x].flag = F_ROOM;
            }
    }
}

/*********************************************
** initializes the map array		    **
** set all cells type to WALL and flag to 0 **
*********************************************/
void init_map(void)
{
    int y, x;
    for (y = 0; y < MAP_HEIGHT; y++)
        for (x = 0; x < MAP_WIDTH; x++) {
            map[y][x].type = CELL_WALL;
            map[y][x].flag = 0;
        }
}

/***************************
** put an exit in the map **
***************************/
void place_exit_in_map(void)
{
    int y, x;
    do {
        y = rand() % MAP_HEIGHT;
        x = rand() % MAP_WIDTH;
    } while ((map[y][x].flag & F_ROOM) != 1);
    map[y][x].flag |= F_EXIT;
    map[y][x].type = CELL_EXIT;
}

/******************************************
** create new map by			 **
**	splitting			 **
**	creating rooms			 **
**	creating paths			 **
**	putting information into map[][] **
******************************************/
void create_new_map(void)
{
    struct node *root = NULL;
    /*
     ** build bsp tree
     */
    /*
     ** initialize *root
     */
    root = create_new_node();
    root->sub_dungeon.ul_corner.y = 0;
    root->sub_dungeon.ul_corner.x = 0;
    root->sub_dungeon.size.h = MAP_HEIGHT;
    root->sub_dungeon.size.w = MAP_WIDTH;
    root->depth = 0;
    split(root);
    put_room_in_dungeon(root);
    create_surrounding_boxes(root);
    create_paths(root);
    /*
     ** copy trees information to map[][]
     */
    init_map();
    tree_to_map(root);
    place_exit_in_map();
    free_bin_tree(root);
}

/*******************************************
** free the memory allocated for bsp tree **
*******************************************/
void free_bin_tree(struct node *cnode)
{
    if (cnode != NULL) {
        if (cnode->left != NULL)
            free_bin_tree(cnode->left);
        if (cnode->right != NULL)
            free_bin_tree(cnode->right);
        free(cnode);
    }
}
