#include <stdlib.h>
#include <stdio.h>
#include <time.h>

void swap(int *a, int *b)
{
    int temp;
    temp = *a;
    *a = *b;
    *b = temp;
}
 
int Partition(int *array, int start, int end)
{
    int pivot, index, i;
    index = start;
    pivot = end;
 
    for(i=start; i < end; i++)
    {
        if(array[i] < array[pivot])
        {
            swap(&array[i], &array[index]);
            index++;
        }
    }
    
    swap(&array[pivot], &array[index]);
    return index;
}
 
int RandomPivotPartition(int *array, int start, int end)
{
    int pivot, num, temp;
    num = rand();
    pivot = start + num%(end-start+1);
    swap(&array[end], &array[pivot]);
    return Partition(array, start, end);
}
 
int quickSort(int *array, int start, int end)
{
    int pivot_index;
    if(start < end)
    {
        pivot_index = RandomPivotPartition(array, start, end);
        quickSort(array, start, pivot_index-1);
        quickSort(array, pivot_index+1, end);
    }
    return 0;
}

int main(void){
	int i, j, tmp;
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
    quickSort(m,0,size-1);
	///////******************************////
	
	if( clock_gettime( CLOCK_REALTIME, &stop) == -1 ) { perror("clock gettime");}		
	exe_time = (stop.tv_sec - start.tv_sec)+ (double)(stop.tv_nsec - start.tv_nsec)/1e9;
	
	for(i=0;i<16;i++) printf("%d ", m[i]);		
	printf("\nExecution time = %f sec\n",  exe_time);
}	
