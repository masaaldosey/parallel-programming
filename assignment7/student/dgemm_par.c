#include "dgemm.h"
#include <immintrin.h>
#include <inttypes.h>
#include <stdio.h>


void dgemm(float *a, float *b, float *c, int n)
{
    // TODO: insert your solution here
    __m256 va, vb, tmp;

    int lim = n - (n % 8);

    for(int i = 0; i < n; i++)
    {
        for(int j = 0; j < n; j++)
        {
            for(int k = 0; k < lim; k+=8)     //to iterate in 8*float chunks
            { 
                //c[i * n + j] += a[i * n + k] * b[j * n + k];

                va = _mm256_loadu_ps(&a[i*n+k]);

                vb = _mm256_loadu_ps(&b[j*n+k]);

                tmp = _mm256_mul_ps(va,vb);
            
                //printf("%f %f %f %f %f %f %f %f", tmp[0], tmp[1], tmp[2], tmp[3], tmp[4], tmp[5], tmp[6], tmp[7]);
                //printf("\n");

                /*if(n-k < 8)
                {
                    //printf("%s", "Entered");
                    //printf("\n");
                    for(int g=0; g<n-k; g++)
                    {
                        c[i * n + j] += tmp[g];
                    }
                } */
                    tmp = _mm256_hadd_ps(tmp, _mm256_permute2f128_ps(tmp,tmp,1));
                    //printf("First\n");
                    //printf("%f %f %f %f %f %f %f %f", tmp[0], tmp[1], tmp[2], tmp[3], tmp[4], tmp[5], tmp[6], tmp[7]);
                    //printf("\n");
                    
                    tmp = _mm256_hadd_ps(tmp, tmp);
                    //printf("Second\n");
                    //printf("%f %f %f %f %f %f %f %f", tmp[0], tmp[1], tmp[2], tmp[3], tmp[4], tmp[5], tmp[6], tmp[7]);
                    //printf("\n");
                    
                    tmp = _mm256_hadd_ps(tmp, tmp);
                    //printf("Third\n");
                    //printf("%f %f %f %f %f %f %f %f", tmp[0], tmp[1], tmp[2], tmp[3], tmp[4], tmp[5], tmp[6], tmp[7]);
                    //printf("\n");

                    //printf("%f", tmp[0]);

                    c[i * n + j] = c[i * n + j] + tmp[0];
                //printf("%s", "---------\n");
                //printf("%f", c[i * n + j]);
                //printf("\n");
                
            }
            //break;

        }
        //break;
    }

    for(int i = 0; i < n; i++)
    {
        for(int j = 0; j < n; j++)
        {
            for (int k = lim; k < n; k++)
            {

                c[i * n + j] += a[i * n  + k] * b[j * n  + k];

            }
            //break;
        }
        //break;
    }



}







































































/*
void dgemm(float *a, float *b, float *c, int n)
{
    // TODO: insert your solution here
    __m256 va, vb, tmp;

    for(int i = 0; i < n; i++)
    {
        for(int j = 0; j < n; j++)
        {
            for(int k = 0; k < n; k+=8)     //to iterate in 8*float chunks
            { 
                //c[i * n + j] += a[i * n + k] * b[j * n + k];

                va = _mm256_loadu_ps(&a[i*n+k]);

                vb = _mm256_loadu_ps(&b[j*n+k]);

                tmp = _mm256_mul_ps(va,vb);

                //tmp = _mm256_add_ps(tmp, _mm256_shuffle_ps(tmp,tmp,1));

                //tmp = _mm256_hadd_ps(tmp, tmp);
            
                //_mm256_storeu_ps(&c[i * n + j], tmp);
                //printf("%f %f %f %f %f %f %f %f", tmp[0], tmp[1], tmp[2], tmp[3], tmp[4], tmp[5], tmp[6], tmp[7]);
                if(n-k < 8){
                    //printf("%s", "Entered");
                    for(int g=0; g<n-k; g++){
                    c[i * n + j] += tmp[g];
                    }
                } else {
                    c[i * n + j] += tmp[0] + tmp[1] +  tmp[2] + tmp[3] + tmp[4] + tmp[5] + tmp[6] + tmp[7];
                }
                //printf("%s", "---------");
                //printf("%f", c[i * n + j]);
                
            }
           // break;

        }
        //break;
    }
}*/


