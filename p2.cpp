#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <omp.h>
void swap(int *a, int *b)
{
    int temp;
    temp = *a;
    *a = *b;
    *b = temp;
}
 
int Partition(int *a, int start, int end)
{
    int pivot, index, i;
    index = start;
    pivot = end;
 
    for(i=start; i < end; i++)
    {
        if(a[i] < a[pivot])
        {
            swap(&a[i], &a[index]);
            index++;
        }
    }
    
    swap(&a[pivot], &a[index]);
    return index;
}
 
int RandomPivotPartition(int *a, int start, int end)
{
    int pvt, n, temp;
    n = rand();
    pvt = start + n%(end-start+1);
    swap(&a[end], &a[pvt]);
    return Partition(a, start, end);
}
 
int quickSort(int *a, int start, int end)
{
    int pindex;
    if(start < end)
    {
        pindex = RandomPivotPartition(a, start, end);
        quickSort(a, start, pindex-1);
        quickSort(a, pindex+1, end);
    }
    return 0;
}

int main(void){
	int i, j, tmp;
    omp_set_num_threads(2);
    int size = 16*1024*1024;
	struct timespec start, stop;
	double exe_time;
	srand(time(NULL)); 
	int * m = new int[size];
	for(i=0; i<size; i++){
		m[i]=size-i;
	}
    
	if( clock_gettime(CLOCK_REALTIME, &start) == -1) { perror("clock gettime");}
	////////**********Your code goes here***************//
    #pragma omp parallel shared(m)
    {
        #pragma omp sections
        {
            #pragma omp section
            {
                quickSort(m,0,(size/2) - 1);
            }
            #pragma omp section
            {
                quickSort(m,size/2,size-1);
            }
        }
    }
	///////******************************////
	
	if( clock_gettime( CLOCK_REALTIME, &stop) == -1 ) { perror("clock gettime");}		
	exe_time = (stop.tv_sec - start.tv_sec)+ (double)(stop.tv_nsec - start.tv_nsec)/1e9;
	
	for(i=0;i<16;i++) printf("%d ", m[i]);		
	printf("\nExecution time = %f sec\n",  exe_time);
}	
