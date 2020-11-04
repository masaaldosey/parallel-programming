#include "familytree.h"
#include <omp.h>

int parallel_traverse(tree *node)
{
	
	// TODO implement your solution in here.
    //omp_set_num_threads(numThreads);
    //omp_set_nested(1);
    //omp_set_max_active_levels(2);
    int father_iq = 0, mother_iq = 0;

    if (node == NULL) 
    {
        return 0;
    }

    else
    {
        #pragma omp task
        {
            father_iq = parallel_traverse(node->father);
        }            
        
        #pragma omp task
        {
            mother_iq = parallel_traverse(node->mother);
        }

        node->IQ = compute_IQ(node->data, father_iq, mother_iq);
        genius[node->id] = node->IQ;
    }

    return node->IQ;    
}

int traverse(tree *node, int numThreads)
{
    omp_set_num_threads(numThreads);
    int final;
    //int father_iq = 0, mother_iq = 0;
    #pragma omp parallel num_threads(numThreads)
    {
        #pragma omp single
        {
            final = parallel_traverse(node);
        }
    }
    return final;
}