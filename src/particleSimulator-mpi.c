#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <omp.h>
#include <math.h>
#include <mpi.h>
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
  printf ( "usage: %s input particle.01.data dt 0.01 numSteps 50 output tmp/timestep threads 10 time [0|1]\n", progName );
}

int main(int argc, char **argv)
{
  int i,j,k,size,numsteps,rank,hostcount;
	int numthreads,timecheck,numFlags;
	int num, wsize,displacement,filesize;
	double dt,tmp;
	double start,end;
	double fx, fy;
	double dx, dy, dist, f;
	float px,py,vx,vy,mass;
	char *input, *output, *outputnumbered;
	particle *p,*sp,*wp;;
	coordinates *force;
	FILE *infile,*outfile;
	int error;

	int blockcounts[2];
	MPI_Aint offsets[2], extent;
	MPI_Datatype coordinateType, particleType, oldtypes[2];
	int *displs;
	int *sendcounts;

	MPI_Init(&argc,&argv);
	MPI_Comm_size(MPI_COMM_WORLD,&hostcount);
	MPI_Comm_rank(MPI_COMM_WORLD,&rank);
	
	/* Set up particle structure MPI data type */
	
	offsets[0] = 0;
	oldtypes[0] = MPI_DOUBLE;
	blockcounts[0] = 2;
	MPI_Type_struct(1,blockcounts,offsets,oldtypes,&coordinateType);
	MPI_Type_commit(&coordinateType);
	
	offsets[0] = 0;
	oldtypes[0] = coordinateType;
	blockcounts[0] = 2;
	MPI_Type_extent(coordinateType,&extent);
	offsets[1] = 2 * extent;
	oldtypes[1] = MPI_DOUBLE;
	blockcounts[1] = 2;
	MPI_Type_struct(2,blockcounts,offsets,oldtypes,&particleType);
	MPI_Type_commit(&particleType);
	
	/* setup done */
	
	/* get all the command-line arguments and put them on the books */
	commandLineFlagType flag[] = {
		/* stringToBeMatched, variableType, pointerToVariable */
		{"input",              _string,      &input					 },
		{"dt",                 _double,      &dt						 },
		{"time",							 _int,		 		 &timecheck			 },
		{"numSteps",           _int,         &numsteps			 },
		{"output",             _string,      &output				 },
		{"threads",            _int,         &numthreads     }
	};
	numFlags = sizeof ( flag ) / sizeof ( commandLineFlagType );
	
	usageErrorType parseErrorCode = parseArgs ( argc, argv, numFlags, flag ) ;
	if ( parseErrorCode == argError  || parseErrorCode == parseError || argc < 7)
	{
		usage( argv[0] );
		MPI_Finalize();
		exit(1);
	}

	if(rank == 0){
		infile = fopen(input,"r");
		if(infile == NULL){
			perror(input);
			error = 1;
		} else error = 0;
	}
	MPI_Bcast(&error,1,MPI_INT,0,MPI_COMM_WORLD);
	if(error == 1){
		MPI_Finalize();
		exit(1);
	}
	
	if(rank == 0){
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
		fclose(infile);
		if(timecheck) start = omp_get_wtime();
		wsize = size;
		
		sendcounts = malloc(sizeof(int)*hostcount);
		displs = malloc(sizeof(int)*hostcount);
		
		size = wsize/hostcount;
		for(i = 0; i < hostcount; i++){
			sendcounts[i] = size;
		}
		if(size*hostcount < wsize) sendcounts[hostcount-1] += wsize - (size * hostcount);
		for(i = 0; i < hostcount; i++){
			displs[i] = size*i;
		}
	}
	
	/* Send the size of each subarray to the respective host */
	MPI_Scatter(sendcounts,1,MPI_INT,&size,1,MPI_INT,0,MPI_COMM_WORLD);
	/* Send the total number of particles to every host */
	MPI_Bcast(&wsize,1,MPI_INT,0,MPI_COMM_WORLD);
	
	outputnumbered = malloc(sizeof(char)*maxlength);
	force = malloc(sizeof(coordinates)*size);
	wp = malloc(sizeof(particle)*wsize);
	sp = malloc(sizeof(particle)*size);
	omp_set_num_threads(numthreads);
	
	/* send the all the particles to each host */
	MPI_Scatter(p,size,particleType,wp,size,particleType,0,MPI_COMM_WORLD);
	/* give the displacement amount to every host */
	MPI_Scatter(displs,1,MPI_INT,&displacement,1,MPI_INT,0,MPI_COMM_WORLD);
	sp = wp+displacement; /* get position of sublist from the displacement */
	for(i = 0; i < numsteps; i++) {
		#pragma omp parallel for shared(wp,force,wsize,sp) private(fx,fy)
		for(j = 0; j < size; j++){ /* Each Particle in sublist */
			fx = 0;
			fy = 0;
			#pragma omp parallel for shared(wp,j,sp) private(dist,f,dx,dy) reduction(+:fx,fy)
			for(k = 0; k < wsize; k++){ /* calculate the forces of each kth particle on the jth particle */
				if (j != k) {
					dx = (sp[j].position.x - wp[k].position.x);
					dy = (sp[j].position.y - wp[k].position.y);
					dist = hypot(dx,dy);
					if(dist > .03){
						f = -.02 * sp[j].mass * wp[k].mass;
						fx += dx/dist*f;
						fy += dy/dist*f;
					}
				}
			}
			force[j].x = fx;
			force[j].y = fy;
		}
		#pragma omp parallel for shared(sp,force,dt) private(tmp)
		for(j = 0; j < size; j++){
			sp[j].velocity.x += force[j].x/sp[j].mass * dt;
			sp[j].velocity.y += force[j].y/sp[j].mass * dt;
			
			tmp = sp[j].position.x + sp[j].velocity.x * dt;
			if(tmp <= -3 || tmp >= 3) {
				sp[j].velocity.x /= -300;
				tmp = sp[j].position.x + sp[j].velocity.x * dt;
			}
			sp[j].position.x = tmp;
			
			tmp = sp[j].position.y + sp[j].velocity.y * dt;
			if(tmp <= -3|| tmp >= 3) {
				sp[j].velocity.y /= -300;
				tmp = sp[j].position.y + sp[j].velocity.y * dt;
			}
			sp[j].position.y = tmp;
		}
		
		/* on each host, gather the new working particle list from all other hosts */
		MPI_Gatherv(sp,size,particleType,wp,sendcounts,displs,particleType,0,MPI_COMM_WORLD);
		MPI_Bcast(wp,wsize,particleType,0,MPI_COMM_WORLD);
		
		/* The the root finds an error with opening file, tell every host to shutdown with an error. */
		if(rank == 0){
			snprintf(outputnumbered,maxlength,"%s.%d.txt",output,i);
			outfile = fopen(outputnumbered,"w");
			if(outfile == NULL){
				perror(outputnumbered);
				error = 1;
			} else error = 0;
		}
		MPI_Bcast(&error,1,MPI_INT,0,MPI_COMM_WORLD);
		if(error == 1){
			MPI_Finalize();
			exit(1);
		}
		
		if(rank == 0){ /* Print the timestep */
			for(j = 0; j < size; j++){
				fprintf(outfile,"%f %f %f %f %f\n",wp[j].position.x,wp[j].position.y,wp[j].velocity.x,wp[j].velocity.y,wp[j].mass);
			}
			fclose(outfile);
		}
		
	} /* numsteps for-loop end */
	
	if(timecheck && rank == 0){
		end = omp_get_wtime();
		printf("%d %f\n",hostcount,end-start);
	}
	
	MPI_Finalize();
}

