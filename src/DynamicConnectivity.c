#include "DynamicConnectivity.h"
#include "dyncon.h"


s_node* getNode(int number){
    return &(levels[0]->vertices[number]);
}
Edge* getEdge(int number, int direction){
    return &adjMatrix[3*number+direction];
}

void initializeDC(int dx,int dy,int dz,int ONN){
    init_dc(dx,dy,dz,ONN);
}
void destroyDC(){
    destroy_dc();    
}

void insertEdge(int node, int direction){  
    Edge *edge=(getEdge(node,direction));
    insert_dc(edge);
}
void deleteEdge(int node, int direction){
    Edge *edge=(getEdge(node,direction));
    delete_dc(edge);
}

int connected(int nodeA,int nodeB){
    return connected_dc(nodeA, nodeB, 1);
}

int getNC(){return levels[0]->nccomp;}
int getNE(){return nte+te;}

void setON(int node,int index,int value){
    s_node *n=getNode(node);
    setONS(value,n,index);
}
int getONN(int node,int index){
    s_node *n=getNode(node);
    return getONn(n,index);
}
int getONC(int node,int index){
    s_node *n=getNode(node);
    return getON(n,index);
}

int isActive(int node,int direction){
     Edge *edge=(getEdge(node,direction));
     if(edge->type==0){return 0;}
     return 1;
}
