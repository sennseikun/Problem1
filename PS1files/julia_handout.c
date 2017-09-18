#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "julia_handout.h"
#include "bitmap.h"
#include "bitmap.c"
#include <mpi.h>

double x_start=-2.01;
double x_end=1;
double yupper;
double ylower;

double ycenter=1e-6;
double step;

int pixel[XSIZE*YSIZE];

// I suggest you implement these, however you can do fine without them if you'd rather operate
// on your complex number directly.
complex_t square_complex(complex_t c){
		complex_t c2;

		c2.real = c.real*c.real + -1*(c.imag*c.imag);
		c2.imag = c.real*c.imag*2;

		return c2;
}

complex_t add_complex(complex_t a, complex_t b){

  complex_t c;
	c.real = a.real + b.real;
	c.imag = a.imag + b.imag;

	return c;
}

complex_t add_real(complex_t a, int b){
  complex_t c;
	c.real = a.real + b;

	return c;
}



// add julia_c input arg here?
void calculate(complex_t julia_C, int rank, int worldsize) {

	printf("%s", "Gets here in process: ");
	printf("%d\n", rank);

	int portion = YSIZE/(worldsize);
	int pixelPiece[XSIZE*portion];

	//Loops through portions of the image
	//Gets result and stores it in a lesser array

	for(int i=0;i<XSIZE;i++) {
		for(int j=(rank)*portion;j<YSIZE/(worldsize) + (rank)*portion;j++) {

			/* Calculate the number of iterations until divergence for each pixel.
			   If divergence never happens, return MAXITER */
			complex_t c;
      complex_t z;
      complex_t temp;
			int iter=0;

      // find our starting complex number c
			c.real = (x_start + step*i);
			c.imag = (ylower + step*j);

      // our starting z is c
			z = c;

      // iterate until we escape
			while(z.real*z.real + z.imag*z.imag < 4) {
        // Each pixel in a julia set is calculated using z_n = (z_n-1)² + C
        // C is provided as user input, so we need to square z and add C until we
        // escape, or until we've reached MAXITER

        // z = z squared + C

				z = add_complex(square_complex(z),julia_C);

				if(++iter==MAXITER) break;
			}

			//Calculates the position in the subarray

			pixelPiece[PIXEL(i,j - (rank)*portion)]=iter;
		}
	}

	if(rank == 0){

		printf("%s\n", "Gets through in process 0");
		for(int i = 0; i < portion*XSIZE; i++){
			pixel[i] = pixelPiece[i];
		}
	}

	else{
		MPI_Send(&pixelPiece, (sizeof(pixelPiece)/sizeof(int)), MPI_INT, 0, 0, MPI_COMM_WORLD);
	}
}


int main(int argc,char **argv) {

	if(argc==1) {
		puts("Usage: JULIA\n");
		puts("Input real and imaginary part. ex: ./julia 0.0 -0.8");
		return 0;
	}

	MPI_Init(NULL,NULL);

	//Variable holding the world size aka number of processes (?)

  int world_size;
	MPI_Comm_size(MPI_COMM_WORLD, &world_size);

	/* Calculate the range in the y-axis such that we preserve the
	   aspect ratio */
	step=(x_end-x_start)/XSIZE;
	yupper=ycenter+(step*YSIZE)/2;
	ylower=ycenter-(step*YSIZE)/2;

  // Unlike the mandelbrot set where C is the coordinate being iterated, the
  // julia C is the same for all points and can be chosed arbitrarily
  complex_t julia_C;

  // Get the command line args
  julia_C.real = strtod(argv[1], NULL);
  julia_C.imag = strtod(argv[2], NULL);

	//My rank

	int world_rank;
  MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

	calculate(julia_C, world_rank, world_size);

	if(world_rank == 0){

		int receiveArray[XSIZE*(YSIZE/(world_size))];
		int part = sizeof(receiveArray)/sizeof(int);

		printf("%s\n", "Gets past first calculate");

		/*for(int x = 0; x < part; x++){
			pixel[x] = *(pointerToArray + x);
		}*/

		//printf("%lu\n", sizeof(receiveArray)/sizeof(int));

		for(int i = 1; i < world_size; i++){
			MPI_Recv(&receiveArray, (sizeof(receiveArray)/sizeof(int)), MPI_INT, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

			printf("%s", "Gets the answer from process: ");
			printf("%d\n", i);

			printf("%s", "Array length: ");
			printf("%d\n", part);

			printf("%s", "Received from process: ");
			printf("%d\n", i);

			for(int j = 0; j < part;j++){
				pixel[(i)*part + j] = receiveArray[j];
			}
		}

	  /* create nice image from iteration counts. take care to create it upside
	     down (bmp format) */
	  unsigned char *buffer=calloc(XSIZE*YSIZE*3,1);
	  for(int i=0;i<XSIZE;i++) {
	    for(int j=0;j<YSIZE;j++) {
	      int p=((YSIZE-j-1)*XSIZE+i)*3;
	      fancycolour(buffer+p,pixel[PIXEL(i,j)]);
	    }
	  }
	  /* write image to disk */
	  savebmp("julia.bmp",buffer,XSIZE,YSIZE);
		printf("%s\n", "Process 0 has saved the image");
	}

	MPI_Finalize();

	return 0;
}
