#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "parseFlags.h"

void usage ( char *progName )
{
  printf ( "usage: %s particleToPgm input [file] xmin -1 xmax 1 ymin -1 ymax 1 xres 1280 yres 800 output [file]\n", progName );
}

void setPixel(int x, int y, int xres,int area,int *pixels)
{
	int tmp = xres*y + x;
	if(tmp < area && tmp >= 0) pixels[y*xres + x] = 255;
}

void drawHorizontalLine(int x0, int y0, int x1, int xres, int area, int *pixels)
{
	int i,tmp;
	
	tmp = x1 - x0;
	if(tmp < 0) tmp *= -1;
	for(i = 0; i < tmp; i++){
		setPixel(x0+i,y0,xres,area,pixels);
	}
}

void drawVerticleLine(int x0, int y0, int y1, int xres, int area, int *pixels)
{
	int i,length;
	
	length = y1 - y0;
	if(length < 0) length *= -1;
	for(i = 0; i < length; i++){
		setPixel(x0,y0+i,xres,area,pixels);
	}
}

void makeCircle(int x0, int y0, int radius, int xres,int area, int *pixels)
{
	int f, ddFx, ddFy, x, y;
	
	f = 1 - radius;
	ddFx = 1;
	ddFy = -2*radius;
	x = 0;
	y = radius;
	drawVerticleLine(x0,y0-radius,y0+radius,xres,area,pixels);
	drawHorizontalLine(x0-radius,y0,x0+radius,xres,area,pixels);
	while(x < y){
		if(f >= 0){
			y--;
			ddFy += 2;
			f += ddFy;
		}
		x++;
		ddFx += 2;
		f += ddFx;
		drawHorizontalLine(x0-x,y0+y,x0+x,xres,area,pixels);
		drawHorizontalLine(x0-x,y0-y,x0+x,xres,area,pixels);
		drawHorizontalLine(x0-y,y0+x,x0+y,xres,area,pixels);
		drawHorizontalLine(x0-y,y0-x,x0+y,xres,area,pixels);
	}
}

main(int argc, char **argv)
{
  int numFlags, area, i, j;
  int xmin, xmax, xres;
  int ymin, ymax, yres;
  int xNew, yNew;
	int tmp;
  float xOld, yOld, mass;
  char *input, *output;
  char s[1001];
  int *pixels;
  FILE *inputFile, *outputFile;
  
  /* get all the command-line arguments and put them on the books */
  commandLineFlagType flag[] = {
    /* stringToBeMatched, variableType, pointerToVariable */
    {"input",              _string,      &input  },
    {"xmin",               _int,         &xmin  },
    {"xmax",               _int,         &xmax  },
    {"ymin",               _int,         &ymin  },
    {"ymax",               _int,         &ymax  },
    {"xres",               _int,         &xres  },
    {"yres",               _int,         &yres  },
    {"output",             _string,      &output  }
  };
  numFlags = sizeof( flag ) / sizeof( commandLineFlagType );
  
  usageErrorType parseErrorCode = parseArgs ( argc, argv, numFlags, flag ) ;
  if ( parseErrorCode == argError  || parseErrorCode == parseError )
  {
    usage( argv[0] );
    return ( 1 );
  }
  
  area = xres * yres;
  pixels = malloc(sizeof(int)*area);
  for(i = 0; i < area; i++) pixels[i] = 0; /* set picture to black */

  
  inputFile = fopen(input,"r");
  if(inputFile == NULL){
    perror(input);
    exit(1);
  }
  while(fgets(s,1000,inputFile) != NULL){
    sscanf(s,"%f %f %*f %*f %f",&xOld,&yOld,&mass);
    xNew = xres * ((double)(xOld-xmin) / (double)(xmax-xmin));
    yNew = yres * ((double)(yOld - ymin) / (double)(ymax - ymin));
		mass = ceil(mass);
		if(!(xOld < xmin || xOld > xmax || yOld < xmin || yOld > xmax)){
			makeCircle(xNew,yNew,floor(mass),xres,area,pixels);
		}
  }
  fclose(inputFile);
  
  outputFile = fopen(output,"w");
  if(outputFile == NULL){
    perror(output);
    exit(1);
  }
  
  fprintf(outputFile,"P2\n%d %d\n255\n",xres,yres);
  for(i = 0; i < yres; i++){
		tmp = pixels[i*xres];
		fprintf(outputFile,"%d",tmp);
    for(j = 1; j < xres; j++){
      /* Although fprintf is buffered, is this a good idea? */
			tmp = pixels[i*xres + j];
      fprintf(outputFile,"  %d",tmp);
    }
    fprintf(outputFile,"\n");
  }
  fclose(outputFile);
}
