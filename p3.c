#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <time.h>
#include <pthread.h>

#define h  800 
#define w  800
#define ITER  50
#define input_file  "input.raw"
#define output_file "output.raw"
pthread_mutex_t lock;
pthread_cond_t cv;

struct thread_data{
    int thread_id;
    unsigned char *a;
    unsigned char *assignment;
//    struct Cluster *clusters;
    int start;
    int end;
    int *sum;
    int *no;
    int *mean;
    int *r;
    int p;
};

int min(int a1, int a2, int a3, int a4, int a5, int a6){
    if(a1 < a2 && a1 < a3 && a1 < a4 && a1 < a5 && a1 < a6){
        return 0;
    }
    if(a2 < a1 && a2 < a3 && a2 < a4 && a2 < a5 && a2 < a6){
        return 1;
    }
    if(a3 < a1 && a3 < a2 && a3 < a4 && a3 < a5 && a3 < a6){
        return 2;
    }
    if(a4 < a1 && a4 < a2 && a4 < a3 && a4 < a5 && a4 < a6){
        return 3;
    }
    if(a5 < a1 && a5 < a2 && a5 < a3 && a5 < a4 && a5 < a6){
        return 4;
    }
    return 5;
}

void *ElementAssigner(void *threadarg){
    struct  thread_data * my_data;
    my_data = (struct thread_data *) threadarg;
    int  thread_id = my_data->thread_id;
    unsigned char *a = my_data->a;
    unsigned char *assignment = my_data->assignment;
    int start = my_data->start;
    int end = my_data->end;
    int *no = my_data->no;
    int *mean = my_data->mean;
    int *sum = my_data->sum;
    int *r = my_data->r;
    int p = my_data->p;
    int k,i;
    short dist[6];
    short shortest;
    
    for(int j = 0; j < ITER; j++){
        for(i=start; i < end; i++){
            //assign elements
            for(k=0;k<6;k++){
                dist[k] = abs(mean[k] - a[i]);
            }
            shortest = min(dist[0],dist[1],dist[2],dist[3],dist[4],dist[5]);
            pthread_mutex_lock(&lock);
            no[shortest] = no[shortest] + 1;
            sum[shortest] = sum[shortest] + a[i];
            assignment[i] = shortest;
            pthread_mutex_unlock(&lock);
        }

        
        pthread_mutex_lock(&lock);

        if(r[0] < p - 1){
            r[0] = r[0] + 1;
            pthread_cond_wait(&cv,&lock);
        } else if(r[0] == p - 1){
            //printf("%d) thread%d r  = %d\n",j,thread_id,r[0]);

            r[0] = 0;
            for(i=0;i<6;i++){
                mean[i] = sum[i] / no[i];
                no[i] = 0;
                sum[i] = 0;
            }
            pthread_cond_broadcast(&cv);
        }
        pthread_mutex_unlock(&lock);

    }
    
    
    
    pthread_exit(NULL);
}


int main(int argc, char** argv){
//    const int ITER = 50;
    int i,k,rc;
    FILE *fp;
    struct timespec start, stop;
    double time;
  	unsigned char *a = (unsigned char*) malloc (sizeof(unsigned char)*h*w);
    unsigned char *assignment = (unsigned char*) malloc (sizeof(unsigned char)*h*w);

	// the matrix is stored in a linear array in row major fashion
	if (!(fp=fopen(input_file, "rb"))) {
		printf("can not opern file\n");
		return 1;
	}
	fread(a, sizeof(unsigned char), w*h, fp);
	fclose(fp);
    
    //****initialize clusters*****
//    struct Cluster clusters[6];
    int *sum = (int*) malloc (sizeof(int)*6);
    int *no = (int*) malloc (sizeof(int)*6);
    int *mean = (int*) malloc (sizeof(int)*6);
    int *r = (int*) malloc (sizeof(int)*1);
    *r = 0;
    mean[0] = 0;
    mean[1] = 60;
    mean[2] = 100;
    mean[3] = 125;
    mean[4] = 190;
    mean[5] = 255;
    for(i = 0; i < 6; i++){
        sum[i] = 0;
        no[i] = 0;
    }

    //**************Initialize Thread****
    int NUM_THREADS = atoi(argv[1]);
    struct  thread_data  thread_data_array[NUM_THREADS];
    pthread_t threads[NUM_THREADS];
    pthread_attr_t  attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
    //************Initialize thread args***********
    for(i=0; i < NUM_THREADS; i++){
        thread_data_array[i].thread_id = i;
        thread_data_array[i].a = a;
        thread_data_array[i].assignment = assignment;
        thread_data_array[i].start = i*(h*w/NUM_THREADS);
        thread_data_array[i].end = (i+1)*(h*w/NUM_THREADS);
        thread_data_array[i].sum = sum;
        thread_data_array[i].no = no;
        thread_data_array[i].mean = mean;
        thread_data_array[i].r = r;
        thread_data_array[i].p = NUM_THREADS;

    }
    if (pthread_mutex_init(&lock, NULL) != 0){
        printf("\n mutex init has failed\n");
        return 1;
    }
    pthread_cond_init(&cv,&lock);

    //************Start algorithm**********
    if( clock_gettime(CLOCK_REALTIME, &start) == -1) { perror("clock gettime");}
	// measure the start time here
	//  Your code goes here
    
        for(i=0;i<NUM_THREADS;i++){
            rc = pthread_create(&threads[i], &attr, ElementAssigner, (void *) &thread_data_array[i] );
            if (rc) { printf("ERROR; return code from pthread_create() is %d\n", rc); exit(-1);}
        }
        for(i=0; i <NUM_THREADS; i++){
            (void) pthread_join(threads[i], NULL);
        }
        

	//
	// measure the end time here
    if( clock_gettime( CLOCK_REALTIME, &stop) == -1 ) { perror("clock gettime");}
    time = (stop.tv_sec - start.tv_sec)+ (double)(stop.tv_nsec - start.tv_nsec)/1e9;
    
	// print out the execution time here
    printf("Execution time = %f sec\n", time);

    for(i = 0; i < h*w; i++){
        a[i] = mean[assignment[i]];
    }
	if (!(fp=fopen(output_file,"wb"))) {
		printf("can not opern file\n");
		return 1;
	}	
	fwrite(a, sizeof(unsigned char),w*h, fp);
    fclose(fp);
    
    return 0;
}
