#include <stdio.h>
#include "eulertour.h"
#include "dllist.h"
static uint32_t DX;
static uint32_t DY;
static uint32_t DZ;
static uint32_t ON;
static s_tree tmp;

/* Return the node corresponding to the Euler Tour linearization of edge 
   (@u,@v)
*/
static inline s_node * getE_SN(const uint32_t u,const uint32_t v,  s_node *edges) {
//Note: 0:up, 1: right, 2:down, 3:left
        if((u - v == DX )|| (v - u == DX*(DY-1))) {
            return &edges[u*6]; //u to v up
        }
         if((v - u == DY*DX )|| (u - v == DY*DX*(DZ-1))) {
            return &edges[u*6+4]; //u to v up
        }
         if((u - v == DY*DX )|| (v - u == DY*DX*(DZ-1))) {
            return &edges[u*6+5]; //u to v up
        }

        if((v - u == DX )|| (u - v == DX*(DY - 1))){
            return &edges[u*6 + 2]; //u to v down
        }
        if((v - u + 1== DX) || (u - v == 1)) {
            return &edges[u*6 + 3]; //s's left edge
        }
        if((u- v + 1== DX) || (v - u == 1)) {
            return &edges[u*6 + 1]; //s's right edge
        }
                  
        return  NULL; //means not a valid edge
}
/**************************************************************************
 **************************************************************************
 */
/* Link spanning trees/euler Tours of vertices @u and @v which aren't connected 
 * in Spanning Forest @et*/
s_tree * link_et(uint32_t u, uint32_t v, struct EulerTour *et ) {
 
    s_node *un = &et->vertices[u], *vn = &et->vertices[v];
    s_tree *ut = getTree(un,1), *vt = getTree(vn,1), *ut1, *vt1;
    s_node *euv = getE_SN(u,v,et->edges),*evu = getE_SN(v,u,et->edges);
    //Checks if u and v are already in the same euler tour
    if(ut == vt)
        return ut;
    //Merge the two euler tours 

    //ut1 is a tree with everything smaller than un
    ut1 = split_temp(un,ut,&tmp,1); 

    //Rejoins ut1 with ut but now as a right tree so everything in ut1 is now bigger 
    //than anything in ut
    ut = join(ut,ut1,1);
  
    //Insert first directed edge
    insLast(ut,euv);
    
    //repeat steps for second tree
    vt1 = split_temp(vn,vt,&tmp,1);
    vt = join(vt,vt1,1);
   
    //Join the two tree ut is smaller one(has to be since it has the directed edge)
    ut = join(ut,vt,1);
   
    //delete tree from list of trees
    delete_tree_from_dll(et,vt);
  
   
    free(vt); 
  
  
    //Add the second directed edge
    insLast(ut,evu);
   
    
    return ut; 
    
}
/* Cut spanning tree caused by deletion of edge (@u,@v) into two
 * separate spanning trees. Return a pointer to the new one*/
s_tree * cut_et(uint32_t u , uint32_t v, struct EulerTour *et ) {
    s_node *euv = getE_SN(u,v,et->edges),*evu = getE_SN(v,u,et->edges),*tmp_n;
    s_tree *t = getTree(euv,1), *t1, *t2 = getTree(evu,1);
   
    /*smaller with respect to the order in the Euler Tour*/
    if(!smaller(euv,evu)) {
        tmp_n = euv;
        euv = evu;
        evu = tmp_n;
        
    }
    t1 = split_temp(euv,t,&tmp,0); //split before euv
    
    t2 = split(evu,t,1);
   
    //split after e2
    t = join(t1,t,0);
  
    t2 = deleteNode(euv, t2,0);

    reset_sn(euv);
    
    t2 = deleteNode(evu,t2,0);
    reset_sn(evu);
    insert_tree_to_dll(et,t2);
    return t2;
    







}


void initalizeEdge(EulerTour *et,uint32_t number,uint32_t on){
    //Edge properties related to Eulertour
    et->edges[number].key=(number%6);
    et->edges[number].msk=0;
    reset_sn(&et->edges[number]);

    //Related to occupation numbers.
    et->edges[number].oNS=malloc(sizeof(int)*on);
    et->edges[number].oN=malloc(sizeof(int)*on);
    et->edges[number].numberOfON=on;
}
/**************************************************************************
 **************************************************************************
 */
/*Initialize EulerTour datastructure for a two dimensional square lattice 
 * graph with periodic boundary conditions and one dimensional length @dlength 
 * with level @level 
 */
struct EulerTour * init_et(uint32_t dx,uint32_t dy,uint32_t dz,uint32_t on, uint8_t level) {
    DX = dx;
    DY = dy;
    DZ = dz;
    ON = on;
    struct EulerTour *et = malloc(sizeof(*et));
    if(!et)
        return NULL;
    et->head = et->tail = NULL; //empty double linked list
    et->nccomp = 0; 
    et->nedges = 0; // initially no active edges in this level
    et->vertices = malloc(dx*dy*dz*sizeof(s_node));
    if(!et->vertices)
        return NULL;
    et->level = level;
 
    et->edges = malloc(dx*dy*dz*6*sizeof(s_node));
    if(!et->edges) {
        free(et->vertices);
        return NULL;
    }
    uint32_t i=0;
    
    for(;i<dx*dy*dz;i++) {
        et->vertices[i].key = i;
        et->vertices[i].msk = 0b00100000; //Bit for Vertex
        reset_sn(&et->vertices[i]);
        
        et->vertices[i].parent = CAST_N malloc(sizeof(s_tree));
        SET_R_BIT((&et->vertices[i]));
        (CAST_T(et->vertices[i].parent))->root = &et->vertices[i];
        insert_tree_to_dll(et,CAST_T (et->vertices[i].parent));

        et->vertices[i].oNS=malloc(sizeof(int)*on);
        et->vertices[i].oN=malloc(sizeof(int)*on);
        et->vertices[i].numberOfON=on;
    }
    uint32_t j=0;
    for(i=0;i<dy*dz*dx;i++) {
        for(j=0;j<6;j++){
            initalizeEdge(et,6*i+j,on);
        }
    }
     

    return et;

}
void destroy_edges(struct EulerTour *et){
    int i=0;
     for(;i<DX*DY*DZ;i++) {

        free(et->vertices[i].oNS);
        free(et->vertices[i].oN);
    }
    uint32_t j=0;
    for(i=0;i<DX*DY*DZ;i++) {
        for(j=0;j<6;j++){
            free(et->edges[6*i+j].oNS);
            free(et->edges[6*i+j].oN);
        }
    }
}
/**************************************************************************
 **************************************************************************
 */
/* Destroy i.e. free all memory of EulerTour @et. Return NULL*/
struct EulerTour * destroy_et(struct EulerTour *et) {
    if(!et)
        return NULL;
    struct s_tree *ctree = et->head;
    uint32_t cnt=0;
    while(ctree) {
        if(ctree->prev) { 
            free(ctree->prev);
            cnt++;
        }
        ctree = ctree->next;
    }
    
    free(et->tail);
    free(et->vertices);
    destroy_edges(et);
    free(et->edges);
    free(et);
    return NULL;
}
/* Check if Vertices @u and @v are connected in EulerTour @et*/
 uint8_t connected_et(uint32_t u, uint32_t v, struct EulerTour *et) {
     return getTree(&et->vertices[u],0) == getTree(&et->vertices[v],0);
 }
