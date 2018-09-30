#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "parseFlags.h"

typedef struct {
	int red;
	int green;
	int blue;
} pixel;

pixel *colors;

void usage ( char *progName )
{
  printf ( "usage: %s particleToPgm input [file] xmin -1 xmax 1 ymin -1 ymax 1 xres 1280 yres 800 output [file]\n", progName );
}

void setPixel(int x, int y, int xres, int area, pixel *pixels,int color)
{
	int tmp = xres*y + x;
	if(tmp < area && tmp >= 0){
		pixels[y*xres + x].red = colors[color%4].red;
		pixels[y*xres + x].green = colors[color%4].green;
		pixels[y*xres + x].blue = colors[color%4].blue;
	}
}

void drawHorizontalLine(int x0, int y0, int x1, int xres, int area, pixel *pixels, int color)
{
	int i,tmp;
	
	tmp = x1 - x0;
	if(tmp < 0) tmp *= -1;
	for(i = 0; i < tmp; i++){
		setPixel(x0+i,y0,xres,area,pixels,color);
	}
}

void drawVerticleLine(int x0, int y0, int y1, int xres, int area, pixel *pixels, int color)
{
	int i,length;
	
	length = y1 - y0;
	if(length < 0) length *= -1;
	for(i = 0; i < length; i++){
		setPixel(x0,y0+i,xres,area,pixels,color);
	}
}

void makeCircle(int x0, int y0, int radius, int xres,int area, pixel *pixels, int color)
{
	int f, ddFx, ddFy, x, y;
	
	f = 1 - radius;
	ddFx = 1;
	ddFy = -2*radius;
	x = 0;
	y = radius;
	drawVerticleLine(x0,y0-radius,y0+radius,xres,area,pixels,color);
	drawHorizontalLine(x0-radius,y0,x0+radius,xres,area,pixels,color);
	while(x < y){
		if(f >= 0){
			y--;
			ddFy += 2;
			f += ddFy;
		}
		x++;
		ddFx += 2;
		f += ddFx;
		drawHorizontalLine(x0-x,y0+y,x0+x,xres,area,pixels,color);
		drawHorizontalLine(x0-x,y0-y,x0+x,xres,area,pixels,color);
		drawHorizontalLine(x0-y,y0+x,x0+y,xres,area,pixels,color);
		drawHorizontalLine(x0-y,y0-x,x0+y,xres,area,pixels,color);
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
  pixel *pixels;
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
	
	/* setup the predefine pixel colors */
	colors = malloc(sizeof(pixel)*4);
	colors[2].red = 127; /* 1st color Green */
	colors[2].green = 255;
	colors[2].blue = 0;
	
	colors[3].red = 0; /* 2nd color Blue */
	colors[3].green = 255;
	colors[3].blue = 255;
	
	colors[1].red = 255; /* 3rd color Yellow */
	colors[1].green = 255;
	colors[1].blue = 0;
	
	colors[0].red = 255; /* 4th color Orange */
	colors[0].green = 165;
	colors[0].blue = 0;
  
  area = xres * yres;
  pixels = malloc(sizeof(pixel)*area);
  for(i = 0; i < area; i++) {
		pixels[i].red = 0;
		pixels[i].green = 0;
		pixels[i].blue = 0;
	}

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
			makeCircle(xNew,yNew,floor(mass),xres,area,pixels,mass);
		}
  }
  fclose(inputFile);
  
  outputFile = fopen(output,"w");
  if(outputFile == NULL){
    perror(output);
    exit(1);
  }
  
  fprintf(outputFile,"P3\n%d %d\n255\n",xres,yres);
  for(i = 0; i < yres; i++){
		fprintf(outputFile,"%d %d %d",pixels[i*xres].red, pixels[i*xres].green, pixels[i*xres].blue);
    for(j = 1; j < xres; j++){
      /* Although fprintf is buffered, is this a good idea? */
			fprintf(outputFile," %d %d %d",pixels[i*xres + j].red, pixels[i*xres + j].green, pixels[i*xres + j].blue);
		}
    fprintf(outputFile,"\n");
  }
  fclose(outputFile);
}
