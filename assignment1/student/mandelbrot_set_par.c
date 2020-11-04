#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <complex.h>
#include <pthread.h>

#include "mandelbrot_set.h"

struct pthread_args
{
	int x_resolution;
	int y_resolution;
	int max_iter;
	double view_x0;
	double view_x1;
	double view_y0;
	double view_y1;
	double x_stepsize;
	double y_stepsize;
	int palette_shift;
	void* image;
	int num_threads;
	int start;		//to define start of either 'x' or 'y' for each thread
	int end;		//to define end of either 'x' or 'y' for each thread
};

void* kernel_mandelbrot (void* args)
{
	struct pthread_args *arg = (struct pthread_args*) args;
	
	double y;
	double x;
	complex double Z;
	complex double C;
	int k;

	//assigning the image explicitly
	unsigned char (*img)[arg->x_resolution][3] = (unsigned char (*) [arg->x_resolution][3])arg->image;

	for (int i = arg->start; i < arg->end; i++)
	{
		for (int j = 0; j < arg->x_resolution; j++)
		{
			y = arg->view_y1 - i * arg->y_stepsize;
			x = arg->view_x0 + j * arg->x_stepsize;

			Z = 0 + 0 * I;
			C = x + y * I;

			k = 0;

			do
			{
				Z = Z * Z + C;
				k++;
			} while (cabs(Z) < 2 && k < arg->max_iter);

			if (k == arg->max_iter)
			{
				memcpy(img[i][j], "\0\0\0", 3);
			}
			else
			{
				int index = (k + arg->palette_shift)
				            % (sizeof(colors) / sizeof(colors[0]));
				memcpy(img[i][j], colors[index], 3);
			}
		}
	}
}


void mandelbrot_draw(int x_resolution, int y_resolution, int max_iter, double view_x0, double view_x1, double view_y0, double view_y1,
	                double x_stepsize, double y_stepsize,
	                int palette_shift, unsigned char (*image)[x_resolution][3],
						 int num_threads) {
	// TODO:
	// implement your solution in this file.

	//allocate the number of threads and respective args
	pthread_t *threads = (pthread_t*) malloc (num_threads * sizeof(pthread_t));
	struct pthread_args* args = (struct pthread_args*) malloc (num_threads * sizeof(struct pthread_args));

	//assign the values to each parameter of each thread[i] and start executing kernel in parallel
	for (int p = 0; p < num_threads; ++p)
	{
		args[p].x_resolution = x_resolution;
		args[p].y_resolution = y_resolution;
		args[p].max_iter = max_iter;
		args[p].view_x0 = view_x0;
		args[p].view_x1 = view_x1;
		args[p].view_y0 = view_y0;
		args[p].view_y1 = view_y1;
		args[p].x_stepsize = x_stepsize;
		args[p].y_stepsize = y_stepsize;
		args[p].palette_shift = palette_shift;
		args[p].image = (void*) image; 
		args[p].num_threads = num_threads;
		args[p].start = p * (y_resolution/num_threads);		//divide the 'y_resolution' into equal parts for 4 threads 
															//does it matter is we use 'x_resolution' or 'y_resolution'?
		args[p].end = (p < num_threads-1) ? ((y_resolution/num_threads) * (p+1)) \
			: ((y_resolution/num_threads) * (p+1)) + (y_resolution % num_threads);		//specify the stop value for each thread

		pthread_create(&threads[p], NULL, kernel_mandelbrot, &args[p] ); 
	}

	//wait for all threads to finish executing kernel
	for (int p = 0; p < num_threads; ++p)
	{
		pthread_join(threads[p], NULL);
	}

	//free-up the dynamically allocated memory
	free(threads);
	free(args);
}
