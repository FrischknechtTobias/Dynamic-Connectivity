#include "DynamicConnectivity.h"
#include <stdio.h>

/*
Expected output:
1,0,0,0,0
47, 2
0,14,7,21,21
1, 1, 0
*/
int main (){
    initializeDC(2,4,6,3);
    insertEdge(0,0);
    insertEdge(1,0);  
    printf("%d,%d,%d,%d,%d \n",connected(0,1),connected(0,2),connected(0,8),connected(2,8),connected(2,5));
    printf("%d, %d \n",getNC(),getNE());
    setON(0,2,14);
    setON(1,2,7);
    printf("%d,%d,%d,%d,%d \n",getONC(0,0),getONN(0,2),getONN(1,2),getONC(0,2),getONC(1,2));
    printf("%d, %d, %d \n",isActive(0,0),isActive(1,0),isActive(0,1));
    destroyDC();
    return 0;
}