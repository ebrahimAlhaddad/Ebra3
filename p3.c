#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <time.h>
#include <pthread.h>

#define h  800 
#define w  800

#define input_file  "input.raw"
#define output_file "output.raw"
pthread_mutex_t lock;
struct Cluster{
    short mean;
    int *children;
    int iter;
};

struct thread_data{
    int thread_id;
    unsigned char *a;
    struct Cluster *clusters;
    int start;
    int end;
    int n;
    //char* message;
};

int min(int a, int b){
    if(a<b){
        return a;
    } else {
        return b;
    }
}

void *ElementAssigner(void *threadarg){
    struct  thread_data * my_data;
    my_data = (struct thread_data *) threadarg;
    int  thread_id = my_data->thread_id;
    unsigned char *a = my_data->a;
    int start = my_data->start;
    int end = my_data->end;
    struct Cluster *clusters = my_data->clusters;
    
    int k,i;
    short dist[6];
    short shortest;
    for(i=start; i < end; i++){
        //assign elements
        for(k=0;k<6;k++){
            dist[k] = abs(clusters[k].mean - a[i]);
        }
        shortest = min(dist[0],min(dist[1],min(dist[2],min(dist[3],min(dist[4],dist[5])))));
        for(k=0;k<6;k++){
            if(shortest == dist[k]){
                //lock
                pthread_mutex_lock(&lock);
                clusters[k].children[clusters[k].iter] = i;
                clusters[k].iter += 1;
                pthread_mutex_unlock(&lock);
                //unlock

            }
        }
        
    }
        pthread_exit(NULL);
}


int main(int argc, char** argv){
    const int ITER = 50;
    int i,k,rc;
    FILE *fp;
    struct timespec start, stop;
    double time;
  	unsigned char *a = (unsigned char*) malloc (sizeof(unsigned char)*h*w);
    
	// the matrix is stored in a linear array in row major fashion
	if (!(fp=fopen(input_file, "rb"))) {
		printf("can not opern file\n");
		return 1;
	}
	fread(a, sizeof(unsigned char), w*h, fp);
	fclose(fp);
    
    //****initialize clusters*****
    struct Cluster clusters[6];
    clusters[0].mean = 0;
    clusters[1].mean = 60;
    clusters[2].mean = 100;
    clusters[3].mean = 125;
    clusters[4].mean = 190;
    clusters[5].mean = 255;

    for(i=0;i<6;i++){
        clusters[i].iter=0;
        clusters[i].children = (int*) malloc(sizeof(int)*h*w);
        
    }
    short dist[6];
    unsigned char temp;
    short shortest;
    int sum;
    //**************Thread Setup****
    int NUM_THREADS = atoi(argv[1]);
    struct  thread_data  thread_data_array[NUM_THREADS];
    pthread_t threads[NUM_THREADS];
    for(i=0; i < NUM_THREADS; i++){
        thread_data_array[i].thread_id = i;
        thread_data_array[i].a = a;
        thread_data_array[i].clusters = &clusters;
        thread_data_array[i].start = i*(h*w/NUM_THREADS);
        thread_data_array[i].end = (i+1)*(h*w/NUM_THREADS);
    }
    if (pthread_mutex_init(&lock, NULL) != 0){
        printf("\n mutex init has failed\n");
        return 1;
    }
    //************Start algorithm**********
    if( clock_gettime(CLOCK_REALTIME, &start) == -1) { perror("clock gettime");}
	// measure the start time here
	//  Your code goes here
    
    for(int j = 0; j < ITER; j++){
        
        for(i=0;i<NUM_THREADS;i++){
            rc = pthread_create(&threads[i], NULL, ElementAssigner, (void *) &thread_data_array[i] );
            if (rc) { printf("ERROR; return code from pthread_create() is %d\n", rc); exit(-1);}
        }
        for(i=0; i <NUM_THREADS; i++){
            (void) pthread_join(threads[i], NULL);
        }
        
        for(k=0;k<6;k++){
            sum = 0;
            for(i=0;i<clusters[k].iter;i++){
                sum += a[clusters[k].children[i]];
            }
            clusters[k].mean = sum/clusters[k].iter;
            if(j!=ITER-1) clusters[k].iter = 0;
        }
        if(j==ITER-1){
            for(k=0;k<6;k++){
                for(i=0;i<clusters[k].iter;i++){
                    a[clusters[k].children[i]] = clusters[k].mean;
                }
            }
        }
    }
    
	//
	// measure the end time here
    if( clock_gettime( CLOCK_REALTIME, &stop) == -1 ) { perror("clock gettime");}
    time = (stop.tv_sec - start.tv_sec)+ (double)(stop.tv_nsec - start.tv_nsec)/1e9;
    
	// print out the execution time here
    printf("Execution time = %f sec\n", time);

	
	if (!(fp=fopen(output_file,"wb"))) {
		printf("can not opern file\n");
		return 1;
	}	
	fwrite(a, sizeof(unsigned char),w*h, fp);
    fclose(fp);
    
    return 0;
}
