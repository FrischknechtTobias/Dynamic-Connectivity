#include "dyncon.h"
#include "dllist.h"
#include <math.h>
#include <stdlib.h>
#include <time.h>

uint8_t curlvl;
static uint32_t DX;
static uint32_t DY;
static uint32_t DZ;
double timeq=0;
double timeI=0;
double timeD=0;
struct s_tree *ut, *vt;

uint32_t i,from;
/*********************************************************************
**********************************************************************/
/* Initialize dynamic connectivity data structures for a planar 2d square lattice
 * graph with periodic boundary conditions and one dimensional size @dl
 * Return 1 if successfull or 0 if unsuccessful. 
 * For each level save a pointer to the corresponding Euler-Tour data structures in 
 * the @levels array
 */
uint8_t init_dc(uint32_t dx,uint32_t dy,uint32_t dz,uint32_t oN) {
    
    DX=dlength = dx; 
    nvertices = dx*dy*dz;  // 2d square lattice
    nte=te=0;

    maxLevel = (uint8_t) floor(log(nvertices)/log(2)); //maybe change to foor(log2(dlength)) + 1 ?
#ifdef LEVEL_HEURISTIC
    maxLevel /= 2;
    printf("Reduced maxLevel = %u\n",maxLevel);
#endif /*LEVEL_HEURISTIC*/
    // allocate pointer to Euler-Tour structs for each level
    levels = malloc(sizeof(*levels)*(maxLevel+1));  
    if(!levels)
        return 0;
    uint32_t i=0;
    for(;i<=maxLevel;i++) {
        levels[i] = init_et(dx,dy,dz,oN,i); // create Euler-Tour for level i
        if(!levels[i]) {
            uint32_t i_;
            for(i_=0;i_<=i;++i_)
                levels[i_] = destroy_et(levels[i_]);
            free(levels);
            return 0;
        }
    }
    
    // allocate memory for edg epointer adjacency Matrix 
    adjMatrix = malloc(3*nvertices*sizeof(Edge));
    if(!adjMatrix) {
        for(i=0;i<=maxLevel;i++)
            levels[i] = destroy_et(levels[i]);
        free(levels);
        return 0;
    }
    //initalizing edges (undirected)
    for(i=0;i<nvertices;i++) {
      
        //x direction so increase is 1
        adjMatrix[3*i+1].type = 0; 
        adjMatrix[3*i+1].from = i;
        adjMatrix[3*i+1].to = (((i%(dy*dx)) +dx) >= (dy*dx) ? i -dx*(dy-1): i +dx); //right edge 
        adjMatrix[3*i+1].tnext = adjMatrix[3*i+1].tprev = NULL;
        adjMatrix[3*i+1].fnext = adjMatrix[3*i+1].fprev = NULL;
        adjMatrix[3*i+1].lvl = 0;

        //y direction so increase is dx
        adjMatrix[3*i].type = 0; 
        adjMatrix[3*i].from = i;
        adjMatrix[3*i].to = ((((i)%dx + 1)) >= (dx) ? i-dx+1: i +1); //right edge 
        adjMatrix[3*i].tnext = adjMatrix[3*i+2].tprev = NULL;
        adjMatrix[3*i].fnext = adjMatrix[3*i+2].fprev = NULL;
        adjMatrix[3*i].lvl = 0;
         
        //z direction so increase is dy*dx
        adjMatrix[3*i+2].type = 0; // inactive edge  
        adjMatrix[3*i+2].from = i; 
        adjMatrix[3*i+2].to =( (i+dy*dx)>=(dx*dy*dz) ? i-dx*dy*(dz-1) : i + dx*dy); //up edge
        adjMatrix[3*i+2].tnext = adjMatrix[3*i].tprev = NULL; // not part of any adjacency list
        adjMatrix[3*i+2].fnext = adjMatrix[3*i].fprev = NULL;
        adjMatrix[3*i+2].lvl = 0; // default level is 0
       
    }
    return 1;




}
void resetTimeD(){
	timeD=0;
}
void resetTimeI(){
	timeI=0;
}
double timeDD(){
	return timeD;
}
double timeII(){
	return timeI;
}
/**************************************************************************
 **************************************************************************
 */
/* Destroy the dynamic connectivity data structure*/
uint8_t destroy_dc(void) {
    uint32_t i;
    for(i=0;i<=maxLevel;i++) {
        levels[i] = destroy_et(levels[i]);
    }
    free(levels);
    free(adjMatrix);
    return 1;




}
/**************************************************************************
 **************************************************************************
 */
/*
 * Insert Edge @e into the dynamic connectivity datastructure. This function assumes that this edge 
 * is not already part of the graph! No test for that. Could be easily implemented (if desired) by checking
 * edge type with ACTIVE_EDGE macro*/
uint8_t insert_dc(Edge *e) {
	/*struct timeval  tv1, tv2;
	gettimeofday(&tv1, NULL);*/
    levels[0]->nedges++;
     
    if(connected_dc_e(e,1)) { /*Non-Tree-Edge*/
		
        nte++;
	    // insert edge into the two corresponding adjacency lists of non-tree-edges
        insert_N_treeEdge_to_dlls(&levels[0]->vertices[e->from],&levels[0]->vertices[e->to],e); //also updates flag E
    }

    else { /*Tree Edge*/
        te++;
      
	    // insert edge into the two corresponding adjacency lists of tree-edges
        insert_treeEdge_to_dlls(&levels[0]->vertices[e->from],&levels[0]->vertices[e->to],e); //also updates flag T
      
        // link the corresponding Euler-Tours two construct one EulerTour, i.e. one spanning tree @ level 0
	    link_et(e->from,e->to,levels[0]);
  
        

    }
	/*gettimeofday(&tv2, NULL);
	
    timeI+=(double) (tv2.tv_usec - tv1.tv_usec) / 1000000 +(double) (tv2.tv_sec - tv1.tv_sec);*/
 return 1;
}
/**************************************************************************
 **************************************************************************
 */
/* Delete Edge @e from the dynamic connectivity datastructure. Here it is assumed that the edge is
 * part of the graph. No test for that. If desired use ACTIVE_EDGE Macro.
 */ 
uint8_t delete_dc(Edge *e) {
    levels[e->lvl]->nedges--;
	/*struct timeval  tv1, tv2;
	gettimeofday(&tv1, NULL);*/
    if(IS_TE(e)) {
            
        te--;

        delete_treeEdge_from_dlls(&levels[e->lvl]->vertices[e->from],&levels[e->lvl]->vertices[e->to],e);
        
        // cut all corresponding Euler-Tours with level <= e->lvl
        for(i=0;i<=e->lvl;i++) {
         
            cut_et(e->from,e->to,levels[i]);
               
        }
       
        // recursively search for a replacement edge at levels <= e->lvl
     
        if(replace(e->from,e->to,e->lvl)) {
           
            e->lvl = 0;
			//return 0;
        }
        
        else {
          e->lvl = 0;
          //return 1;
         }
    }
    else {
      
        nte--;
        delete_N_treeEdge_from_dlls(&levels[e->lvl]->vertices[e->from],&levels[e->lvl]->vertices[e->to],e);
        e->lvl = 0;
        //return 0;
    }
	/*gettimeofday(&tv2, NULL);
	
    timeD+=(double) (tv2.tv_usec - tv1.tv_usec) / 1000000 +(double) (tv2.tv_sec - tv1.tv_sec);*/
	return 1;
}

/**************************************************************************
 **************************************************************************
 */
/* Search for a replacement edge connecting vertices @u and @v in levels 
 * <= @lvl. Return 1 if found and 0 if unsuccessful
 */
uint8_t replace(uint32_t u, uint32_t v, uint8_t lvl) {
     
    ut = getTree(&levels[lvl]->vertices[u],0);
    vt = getTree(&levels[lvl]->vertices[v],0);
  
    curlvl = lvl;
    // if current level is less then maxLevel move all tree edges of smaller spanning tree one level up
    if(lvl<maxLevel)

        moveTreeEdgesUp(ut->root->n <= vt->root->n ? ut->root:vt->root); 

    // start search for replacement edge in smaller spanning tree
    if(seekReplaceEdge(ut->root->n <= vt->root->n ? ut->root:vt->root))
        return 1;
    
    // if not successfull at level lvl go one down if possible
    else
    {
         if (lvl > 0) 
            return replace(u,v,lvl - 1);
        else    // no replacement edge found, i.e. components must keep split
            return 0;
    }


}
/**************************************************************************
 **************************************************************************
 */
/* Move all tree edges stored in @n's subtree one level up*/
uint8_t moveTreeEdgesUp(struct s_node *n) {

  //check if n is non-empty and has tree edges in its subtree (including n)
    if(!n || !(TEST_T_BIT(n)) ) return 0;
    // move tree edges of left sub tree up
    moveTreeEdgesUp(n->sn_left);

    // if current splay-tree node represents a vertex move all tree edges
    // saved in its adjacency list one level up
    if(vertexType(n) && n->pt1) { //Only if this is a vertex AND has edges
 
        Edge *ce;
        //while there are more tree edges attached to node n
        while((ce = n->pt1)) {

            //figuring out if it was the to or the from node of the edge
            if(n->key == ce->from) {
                delete_treeEdge_from_dlls(n,&levels[ce->lvl]->vertices[ce->to],ce);
            }
            else {
                delete_treeEdge_from_dlls(&levels[ce->lvl]->vertices[ce->from],n,ce);
            }
            //Reducing amount of edges on that level
            levels[ce->lvl]->nedges-=1; 
            
            ce->lvl +=1;
            //inserting the edge to tree edge list on new level
            insert_treeEdge_to_dlls(&levels[ce->lvl]->vertices[ce->from],&levels[ce->lvl]->vertices[ce->to],ce);
	        
            //increasing edge amount by one 
            levels[ce->lvl]->nedges++; 


            //linking the Euler tours on the new level
            link_et(ce->from,ce->to,levels[ce->lvl]);
        }
    }
   //moving right subtree up
   moveTreeEdgesUp(n->sn_right);
   return 0;
}
/**************************************************************************
 **************************************************************************
 */
/* Check all non-tree edges in @n's subtree for replacement edge
 * If replacement edge rejoin euler Tours if not move edge one level up
 */
uint8_t seekReplaceEdge(struct s_node *n) {

  // check if n is non-empty and if it has non-tree edges in its subtree (including n)
  
    if(!n || !(TEST_E_BIT(n))) return 0;
   
    //search in left subtree
        if(seekReplaceEdge(n->sn_left))
            return 1;
  
    if(vertexType(n) && n->pt2) {
        Edge *ce = n->pt2;
        struct s_node *tmp;
        // search through adjacency list of nontree-edges of current vertex n
	while(ce) {
            //get the splay-tree-node of the other vertex adjacent to ce
            tmp = &levels[ce->lvl]->vertices[(n->key == ce->from? ce->to: ce->from)];
            //if this edge reconnects the previously splitted spanning trees use it as replacement edge 
           
            if(getTree(tmp,0) == (ut->root->n <= vt->root->n ? vt:ut)) { //Replacement Edge found
                // delete it from the corresponding non-tree edge adj.lists
                if(n->key == ce->from) delete_N_treeEdge_from_dlls(n,tmp,ce);
                else delete_N_treeEdge_from_dlls(tmp,n,ce);
            
                nte--;
                te++;
                for(i=0;i<=ce->lvl;i++) {
              
                    
                    link_et(ce->from,ce->to,levels[i]);
               
                    }

             
                // insert edge to corresponding tree-edge adj.lists
                insert_treeEdge_to_dlls(&levels[ce->lvl]->vertices[ce->from],&levels[ce->lvl]->vertices[ce->to],ce);
                return 1;         
            }
            
            // if this edge is not a repl. edge then move it one level up, because its in more dense
            // region of our graph (if possible)
            else { 
            
	      if(ce->lvl < maxLevel) { /*If not move non-tree edge one level up (if possible)*/
                if(n->key == ce->from) delete_N_treeEdge_from_dlls(n,tmp,ce);
                else delete_N_treeEdge_from_dlls(tmp,n,ce);
		levels[ce->lvl]->nedges--;
                ce->lvl +=1;
                insert_N_treeEdge_to_dlls(&levels[ce->lvl]->vertices[ce->from],&levels[ce->lvl]->vertices[ce->to],ce);
		levels[ce->lvl]->nedges++;
		ce = n->pt2;
	      }
	      else {
		if(ce->from == n->key)
		  ce = ce->fnext;
		else
		  ce = ce->tnext;
	      }
	    }
            
        }
    }
         
    // search in right subtree
    if(seekReplaceEdge(n->sn_right))
        return 1;
    return 0;
}


