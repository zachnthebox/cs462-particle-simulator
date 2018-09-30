#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <omp.h>
#include <unistd.h>
#include <sys/types.h>
#include "parseFlags.h"

#define maxlength 1024

typedef struct {
	double x;
  double y;
} coordinates;

typedef struct {
  coordinates position;
  coordinates velocity;
	double mass;
	double size;
} particle;

void usage ( char *progName )
{
  printf ( "usage: %s input particle.01.data dt 0.01 numSteps 50 output tmp/timestep time [0|1]\n", progName );
}

int main(int argc, char **argv)
{
  int i,j,k,size,numsteps,filesize;
	int numthreads,timecheck,numFlags;
	double dt,tmp;
	double start,end;
	double fx, fy;
	double dx, dy, dist, f;
	float px,py,vx,vy,mass;
	char *input, *output, *outputnumbered;
	particle *p;
	coordinates *force;
	FILE *infile,*outfile;
	
	
	/* get all the command-line arguments and put them on the books */
  commandLineFlagType flag[] = {
    /* stringToBeMatched, variableType, pointerToVariable */
    {"input",              _string,      &input					 },
    {"dt",                 _double,      &dt						 },
    {"time",							 _int,		 		 &timecheck			 },
    {"numSteps",           _int,         &numsteps			 },
    {"output",             _string,      &output				 }
  };
  numFlags = sizeof ( flag ) / sizeof ( commandLineFlagType );
  
  usageErrorType parseErrorCode = parseArgs ( argc, argv, numFlags, flag ) ;
  if ( parseErrorCode == argError  || parseErrorCode == parseError || argc < 7)
  {
    usage( argv[0] );
    return ( 1 );
	}
	
	infile = fopen(input,"r");
  if(infile == NULL){
    perror(input);
    exit(1);
  }
  
  filesize = lseek(fileno(infile),0,SEEK_END);
  lseek(fileno(infile),-filesize,SEEK_CUR);
  
  p = malloc(sizeof(particle)*(filesize/44));
  
  size = 0;
  while(fscanf(infile,"%f %f %f %f %f",&px,&py,&vx,&vy,&mass) > 0) {
    p[size].position.x = px;
		p[size].position.y = py;
		p[size].velocity.x = vx;
		p[size].velocity.y = vy;
		p[size].mass =  mass;
		p[size].size = sqrt(p[size].mass);
    size++;
  };
	
	outputnumbered = malloc(sizeof(char)*maxlength);
	force = malloc(sizeof(coordinates)*size);
	
	if(timecheck) start = omp_get_wtime();

	for(i = 0; i < numsteps; i++) { /* Each timeStep */
		for(j = 0; j < size; j++){ /* Each Particle */
			fx = 0;
			fy = 0;
			for(k = 0; k < size; k++){ /* calculate the forces of each kth particle on the jth particle */
				if (j != k) {
					dx = (p[j].position.x - p[k].position.x);
					dy = (p[j].position.y - p[k].position.y);
					dist = hypot(dx,dy);
					if(dist > .03){
						f = -.02 * p[j].mass * p[k].mass;
						fx += dx/dist*f;
						fy += dy/dist*f;
					}
				}
			}
			force[j].x = fx;
			force[j].y = fy;
		}

		for(j = 0; j < size; j++){
			p[j].velocity.x += force[j].x/p[j].mass * dt;
			p[j].velocity.y += force[j].y/p[j].mass * dt;
			
			tmp = p[j].position.x + p[j].velocity.x * dt;
			if(tmp <= -3 || tmp >= 3) {
				p[j].velocity.x /= -300;
				tmp = p[j].position.x + p[j].velocity.x * dt;
			}
			p[j].position.x = tmp;
			
			tmp = p[j].position.y + p[j].velocity.y * dt;
			if(tmp <= -3|| tmp >= 3) {
				p[j].velocity.y /= -300;
				tmp = p[j].position.y + p[j].velocity.y * dt;
			}
			p[j].position.y = tmp;
		}
		
		snprintf(outputnumbered,maxlength,"%s.%d.txt",output,i);
		outfile = fopen(outputnumbered,"w");
		for(j = 0; j < size; j++){
			fprintf(outfile,"%f %f %f %f %f\n",p[j].position.x,p[j].position.y,p[j].velocity.x,p[j].velocity.y,p[j].size);
		}
		fclose(outfile);
	}
	
	if(timecheck){
		end = omp_get_wtime();
		printf("%d %f\n",numthreads,end-start);
	}
	
}

