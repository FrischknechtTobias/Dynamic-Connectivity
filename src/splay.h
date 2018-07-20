#ifndef SPLAY_H
#define SPLAY_H

//#define DEBUG
#include <stdlib.h>
#include "mytypes.h"

void rotate_left(struct s_node *) ;

void rotate_right(struct s_node *);

/*Splay node x*/
void splay(struct s_node *);


struct s_tree * split_temp(struct s_node *, struct s_tree *, struct s_tree *,
 uint8_t);

struct s_tree * join(struct s_tree *, struct s_tree *, uint8_t);

void insFirst(struct s_tree *, struct s_node *);

void insLast(struct s_tree *, struct s_node *);

struct s_tree *  deleteNode(struct s_node *, struct s_tree *,uint8_t);

void deleteTree(struct s_tree *);

uint8_t smaller(struct s_node *, struct s_node *);

//struct s_node * insert(unsigned int , void *, void *,struct s_tree * );

struct s_tree * getTree(struct s_node *, uint8_t);


void updateTBit(s_node *);

void updateEBit(s_node *);

void updateTBit_Del(s_node *);
void updateEBit_Del(s_node *);
void updateEBit_Ins(s_node *);
void updateTBit_Ins(s_node *);

void calcON(s_node *);
void updateON(s_node *);

void setONS(int,s_node *,int);
int getON(s_node* ,int);
int getONn(s_node*,int);
/**
 * This function sets both pointer of n and updates the mask!
 * Important: This function should only be used for pre-inserted/isolated nodes,
 * because no tree augmentation update is done
 **/

static inline void setDat(struct s_node *n, void *p1,  void *p2) {
    if(n) {
        n->pt1 = p1;
        n->pt2 = p2;
        if(p1)
            SET_E_BIT(n);
        if(p2)
            SET_T_BIT(n);


    }


}
/*******************************************************************************
 *******************************************************************************
  */
static inline struct s_node * s_first(struct s_node *c) {
    if(!c)
        return NULL;

    while(c->sn_left) {
        c = c->sn_left;
    }
    return c;

}
/*******************************************************************************
 *******************************************************************************
  */
static inline struct s_node * s_last( struct s_node *c) {
    if(!c)
        return NULL;

    while(c->sn_right) {
        c = c->sn_right;
    }
    return c;


}

/*******************************************************************************
 *******************************************************************************
 */
 static inline struct s_tree * split(struct s_node *n,struct s_tree *tr, 
 uint8_t insLeft)
{
    
    return split_temp(n,tr,malloc(sizeof(*tr)),insLeft);
}
/*******************************************************************************
 *******************************************************************************
 */


#endif /*SPLAY_H*/


