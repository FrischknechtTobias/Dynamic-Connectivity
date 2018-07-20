#include <stdio.h>
#include "DynamicConnectivity.h"
#include <math.h>
#include <stdlib.h>

static double w;
static double q=3;
static int nvertices;
double weightRatio(int neOld,int neNew,int ncOld,int ncNew){
	
 return pow(w,(neNew-neOld))*pow(q,(ncNew-ncOld));
	
}
int randomDirection(){

    return rand()%3;
}
int randomNode(){

    return rand()%(nvertices);
}
int accept(double weightRatio){
    double c=(rand()*1.0/RAND_MAX);
    return c<(weightRatio);
}
void flipEdge(int node,int direction){
	//if inactive activate Edge otherwise flip it
     if(!isActive(node,direction)){
        
        insertEdge(node,direction);
    }
    else{
   
        deleteEdge(node,direction);
    }

}
void doStepMonteCarlo(){
	int direction=randomDirection();
    int node=randomNode();
	int oldNc=getNC();
	int oldNe=getNE();
    flipEdge(node,direction);
    if(!accept(weightRatio(oldNe,getNE(),oldNc,getNC()))){
          
            flipEdge(node,direction);
    }
}
void monteCarlo(int steps,int measurements){
	//fixed random seed 
    srand(0);
	double y;
	printf("y,nc/max,ne/max \n");
	for(double y=0.01;y<=1;y+=0.01){
		w=pow(M_E,(y))-1;
		//Thermalisation steps;
		for(int b=0;b<1000;b++){
			doStepMonteCarlo();
		}
    
		double sum=0;
		double valueClusters=0;
		double valueEdges=0;
		int a ;
		for(a=0;a<steps;a++){
			doStepMonteCarlo();
			if(a%(int)(steps/(double)measurements)==0){
			valueClusters+=(getNC());
			valueEdges+=(getNE());}
		}
		printf("%f;%f;%f \n",y,valueClusters/(measurements*1.0*nvertices),valueEdges/(measurements*1.0*nvertices*3));
       
    }
}


int main(int length,char**args){
	int dimensionX=2;
    int dimensionY=2;
    int dimensionZ=2;
    int ON=0;
	int steps=10000;
	int measurements=100;
	
	
	if(length>5){
		dimensionX=atoi(args[1]);
        dimensionY=atoi(args[2]);
        dimensionZ=atoi(args[3]);
        steps=atoi(args[4]);
		measurements=atoi(args[5]);
	}
	nvertices=dimensionX*dimensionY*dimensionZ;
	initializeDC(dimensionX,dimensionY,dimensionZ,ON);
	printf("Dimension: x:%d y:%d z:%d \n",dimensionX,dimensionY,dimensionZ);
    monteCarlo(steps,measurements);
	destroyDC();

   return 0;
 
}