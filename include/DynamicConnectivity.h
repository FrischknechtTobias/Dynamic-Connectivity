#ifndef DYNAMICCONNECTIVITY_H
#define DYNAMICCONNECTIVITY_H

//Initializes a 3d cubic graph (6 neighbors) periodic boundary conditions
// dx,dy,dz are the dimensions of the lattice
//ONN states how many different occupation numbers exist.
void initializeDC(int dx,int dy,int dz,int ONN);

//Frees all memory used by the previous initializeDC
void destroyDC();

//inserts Edge that goes from that node to if direction is:
//0:node+1, x direction
//1:node+dx, y direction
//2:node+dx*dy, z direction
//All of them with periodic boundaries 
//no check on whether that edge is already inserted or not.
void insertEdge(int node, int direction);

//Deletes Edge that goes from that node to if direction is:
//0:node+1, x direction
//1:node+dx, y direction
//2:node+dx*dy, z direction
//All of them with periodic boundaries 
//no check on whether that edge is deleted or not.
void deleteEdge(int node, int direction);

//Checks if these two nodes are conencted
//0=no 1=yes
int connected(int nodeA,int nodeB);

//Returns the number of unconnected components in our graph
int getNC();

//Returns the amount of active edges in our graph
int getNE();

//States wheter the edge starting from that node is active direction mapping:
//0:node+1, x direction
//1:node+dx, y direction
//2:node+dx*dy, z direction
//0=no 1=yes 
int isActive(int node,int direction);

//Sets the index Occupation Number of the node to value
void setON(int node,int index,int value);

//Node version: Gets the index occupation number of that node
int getONN(int node,int index);

//Component version: Returns the summed index occupation number of the connected component containing node
int getONC(int node,int index);


#endif