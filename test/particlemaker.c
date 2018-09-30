//
//  particlemaker.c
//  
//
//  Created by Zachary Berardo on 9/26/12.
//
//

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

main(int argc, char **argv)
{
	if(argc < 2 || argc > 2) {
		fprintf(stderr,"usage: particleMaker [numparticles]\n");
		exit(1);
	}
	
  FILE *o;
  int i,j;
  sscanf(argv[1],"%d",&j);
  o = fopen("particle.01.data","w");
  srand48(time(NULL));
  for(i = 0; i < j; i++){
    fprintf(o,"%f %f %f %f %f\n",drand48()*2-1,drand48()*2-1,drand48()*2-1,drand48()*2-1, drand48()*5);
  }
  
}
