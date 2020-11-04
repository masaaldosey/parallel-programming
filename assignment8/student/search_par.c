#include <string.h>
#include <mpi.h>
#include <stdlib.h>
#include <stdio.h>
#include "search.h"
#include "helper.h"
#include <math.h>

void search_text (char* text, int num_lines, int line_length, char* search_string, int *occurences)
{
  /*
    Counts occurences of substring "search_string" in "text". "text" contains multiple lines and each line
    has been placed at text + line_length * num_lines since line length in the original text file can vary.
    "line_length" includes space for '\0'.

    Writes result at location pointed to by "occurences".


    *************************** PARALLEL VERSION **************************

    NOTE: For the parallel version, distribute the lines to each processor. You should only write
    to "occurences" from the root process and only access the text pointer from the root (all other processes
    call this function with text = NULL) 
  */

  // Write your parallel solution here

  int size, rank;

  MPI_Comm_size (MPI_COMM_WORLD, &size);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  int running_count = 0;
  
  int lines_per_proc;
  lines_per_proc = ceil(num_lines / size);  //since it is a floor function, it automatically takes care of the remainder

  int substr_size;
  substr_size = (lines_per_proc * line_length); //calculating a subset of 'text'

  char *substr = malloc(sizeof(char) *(substr_size));

  MPI_Scatter(text, substr_size, MPI_CHAR, substr, substr_size, MPI_CHAR, 0, MPI_COMM_WORLD);
 
  for (int i = 0; i < lines_per_proc; i++)
  {
      running_count += count_occurences(substr + i * line_length, search_string);
  }

  MPI_Allreduce(&running_count, &running_count, 1, MPI_INT, MPI_SUM, MPI_COMM_WORLD);
  *occurences = running_count;

}








/*
{
  int size, rank;

  MPI_Comm_size (MPI_COMM_WORLD, &size);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  int running_count = 0;
  int start, end;
  start = rank * (num_lines / size);
  end = (rank + 1) * (num_lines / size);
  //printf("\nBefore rank = %d  start = %d  end = %d num_lines = %d", rank, start, end, num_lines);
  if (rank == (size - 1))
  {
    end = end + (num_lines % size);
    //printf("\nAfter rank = %d  start = %d  end = %d num_lines = %d", rank, start, end, num_lines);
  }

  //printf("\nBefore rank = %d  start = %d  end = %d num_lines = %d line_length = %d", rank, start, end, num_lines, line_length);
  for (int i = start; i < end; i++)
  {
      printf("\nBefore rank = %d  start = %d  end = %d", rank, start, end);
      //printf("\nrank = %d\n", rank);
      if (rank == 1){
        printf("\n rank = %d i = %d", rank, i);
      }
      running_count += count_occurences(text + i * line_length, search_string);
      //printf("\nAfter rank = %d  start = %d  end = %d  num_lines = %d", rank, start, end, num_lines);
      //printf("\n\n\n");
  }
  //printf("\nrank = %d  running_count = %d\n", rank, running_count);
  //printf("\n running_count = %d  rank = %d", running_count, rank);

  //MPI_Allreduce(&running_count, &running_count, 1, MPI_INT, MPI_SUM, MPI_COMM_WORLD);
  *occurences = running_count;
  //MPI_Barrier(MPI_COMM_WORLD);
}
*/