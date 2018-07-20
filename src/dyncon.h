#ifndef DYNCON_H
#define DYNCON_H
#include "eulertour.h"

/*Set Macro LEVEL_HEURISTIC in dyncon.h to use only half of all levels */
uint32_t te;
uint32_t nte;
uint8_t maxLevel;
uint32_t dlength;
uint32_t nvertices;

char collect_stat;
/*__u64 del_nte;
__u64 del_te;
__u64 ins_nte;
__u64 ins_te;
__u64 del_red_bonds;
__u64 del_dangle_bonds; */



EulerTour **levels;
Edge *adjMatrix;
double timeDD();
double timeII();
void resetTimeI();
void resetTimeD();
uint8_t init_dc(uint32_t,uint32_t,uint32_t,uint32_t); 
uint8_t destroy_dc(void);
uint8_t delete_dc(Edge *);
uint8_t insert_dc(Edge *);
uint8_t replace(uint32_t, uint32_t , uint8_t) ;
static inline uint8_t connected_dc(uint32_t u, uint32_t v, uint8_t withSplay) {
    return getTree(&levels[0]->vertices[u],withSplay) == getTree(&levels[0]->vertices[v],\
    withSplay);
}
static inline uint8_t connected_dc_e(Edge *e, uint8_t withSplay) {
    if(e)
        return connected_dc(e->from,e->to,withSplay);
    else
        return 0;
}
uint8_t moveTreeEdgesUp(struct s_node *); 
uint8_t seekReplaceEdge(struct s_node *);
#endif /*DYNCON_H*/
