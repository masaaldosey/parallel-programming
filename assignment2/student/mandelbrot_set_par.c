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
	int palette_shift;
	int start;		//to define start of either 'x' or 'y' for each thread
	int end;		//to define end of either 'x' or 'y' for each thread
	int width;
	double view_x0;
	double view_x1;
	double view_y0;
	double view_y1;
	double x_stepsize;
	double y_stepsize;
	void* image;	
};

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
int num_threads_global;
int buffer = 0;
int counter = 0;
int *scheduler;
int totalchunks = 0;
pthread_cond_t condconsume, condproduce;

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

void* producer(void *ptr) 
{
	for (int i = 0; i < totalchunks; i++) 
	{
    	pthread_mutex_lock(&mutex);

    	while (buffer >= num_threads_global)
		{
			pthread_cond_wait(&condproduce, &mutex);
		}
    	scheduler[buffer] = i;
    	buffer = buffer+1;
    	counter=counter+1;
    	pthread_cond_signal(&condconsume);	
    	pthread_mutex_unlock(&mutex);
	}
	pthread_mutex_lock(&mutex);
	pthread_cond_broadcast(&condconsume);
	pthread_mutex_unlock(&mutex);
	pthread_exit(0);
}

void* consumer(void *ptr) 
{
	struct pthread_args *arg = (struct pthread_args*) ptr;
  	for (int i = 0; i < totalchunks; i++) 
	{
    	pthread_mutex_lock(&mutex);
    	while (buffer == 0)
    	{
    		if (counter >= (totalchunks)) 
			{
				pthread_mutex_unlock(&mutex);
				pthread_exit(0);
			}
    		pthread_cond_wait(&condconsume, &mutex);
    	}
    	int buffer_value = scheduler[buffer-1];
    	buffer = buffer-1;
		arg->start = buffer_value * arg->width;
		arg->end = (buffer_value+1) * arg->width;
		if (buffer_value == totalchunks-1) 
		{
			arg->end = arg->y_resolution;
		}
    	pthread_cond_signal(&condproduce);
    	pthread_mutex_unlock(&mutex);
    	kernel_mandelbrot(arg);
 	}
	pthread_exit(0);
}


void mandelbrot_draw(int x_resolution, int y_resolution, int max_iter,
			                double view_x0, double view_x1, double view_y0, double view_y1,
						                double x_stepsize, double y_stepsize,
									                int palette_shift, unsigned char (*image)[x_resolution][3],
																	 int num_threads) {
	// TODO: implement your solution in this file.

	num_threads_global = num_threads-1;	//only three threads now as one thread is a producer

	
	//allocate the number of threads and respective args
	pthread_t *threads = (pthread_t*) malloc (num_threads_global * sizeof(pthread_t));
	struct pthread_args* args = (struct pthread_args*) malloc (num_threads_global * sizeof(struct pthread_args));
	scheduler = (int *) malloc(sizeof(int)*num_threads_global);	//assigning memory for scheduler 
	
	pthread_t pro_ducer;	//producer thread

	pthread_cond_init(&condconsume, NULL);
	pthread_cond_init(&condproduce, NULL);
	
	totalchunks = 8 * num_threads_global;
	int width = y_resolution / num_threads_global;

	pthread_create(&pro_ducer, NULL, producer, NULL);	//creating the chunks

	for(int i=0;i<num_threads_global; ++i) 
	{
		args[i].x_resolution = x_resolution;
		args[i].y_resolution = y_resolution;
		args[i].max_iter = max_iter;
		args[i].view_x0 = view_x0;
		args[i].view_y1 = view_y1;
		args[i].x_stepsize = x_stepsize;
		args[i].y_stepsize = y_stepsize;
		args[i].palette_shift = palette_shift;
		args[i].image = (void*) image;
		args[i].width = y_resolution / totalchunks;
		pthread_create(&threads[i], NULL, consumer, args+i);
	}

	//wait for all threads to finish executing kernel
	for(int i=0;i<num_threads_global; ++i) 
	{
		pthread_join(threads[i], NULL);
	}
	
	//join the producer thread
	pthread_join(pro_ducer, NULL);

	//destroy the mutex locks and condition variables
	pthread_mutex_destroy(&mutex);
	pthread_cond_destroy(&condconsume);
	pthread_cond_destroy(&condproduce);
	
	//free-up the dynamically allocated memory
	free(threads);
	free(args);
	free(scheduler);
}
