#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <pthread.h>
#include <semaphore.h>
#include <math.h>
#include <time.h>
#include <stdbool.h>

//••••••••••••••••••••••••••••••••••••••••••••••••••••••••••••••••••••••GLOBALS\

#define EAST 	0
#define WEST	1

// Thread arguments
typedef struct threadData{ int threadID; int qty; int direction; }threadData;
typedef struct carData{ int threadID; int direction; }carData;

pthread_mutex_t lock;
pthread_mutex_t screen;
int Waiting[2];
sem_t Semaphore[2];
int CurrentDirection = -1;

int fromWest = -1;
int fromEast  = -1;
int carsInBridge = 0;
int mean = -1;

int totalCars = -1;
int remainingGoingEast = -1;
int remainingGoingWest = -1;

threadData tData[2];
pthread_t creator[2];

int errCheck;

//••••••••••••••••••••••••••••••••••••••••••••••••••••••••••••••••••••FUNCTIONS

//Implicit declarations
double exprand(double mean);
void *GoingWest(void *arg);
void *GoingEast(void *arg);
void *createEastCars(void *arg);
void *createWestCars(void *arg);


int main(int argc, char *argv[])
{
    int opt;

    while ((opt = getopt(argc, argv, "e:o:m:")) != -1) {
        switch (opt) {
        case 'e':
          fromEast = atoi(optarg);
          break;
        case 'o':
          fromWest = atoi(optarg);
          break;
        case 'm':
          mean = atoi(optarg);
          break;
        default:
            fprintf(stderr, "Uso: %s -e #CarrosDelEste -o #CarrosDelOeste -m #Media\n", argv[0]);
            exit(EXIT_FAILURE);
        }
    }

    if (fromEast == -1) {
      fprintf(stderr, "-e #CarrosDelEste es un parámetro obligatorio\n");
      exit(EXIT_FAILURE);
    }

    if (fromWest == -1) {
      fprintf(stderr, "-o #CarrosDelOeste es un parámetro obligatorio\n");
      exit(EXIT_FAILURE);
    }

    fprintf(stdout, "\n****************\n* SOA: TAREA 1 *\n****************\n\n");
    fprintf(stdout, "Cantidad de carros del este: %u\n", fromEast);
    fprintf(stdout, "Cantidad de carros del oeste: %u\n", fromWest);

    totalCars = fromEast+fromWest;
    remainingGoingEast = fromWest;
    remainingGoingWest = fromEast;
	threadData tData[totalCars];
	pthread_t thread[totalCars];


	

	sem_init(&Semaphore[0], 0, 0);
	sem_init(&Semaphore[1], 0, 1);

	tData[0].qty = fromEast;
	tData[0].direction = 1;
	tData[1].qty = fromWest;
	tData[1].direction = 0;


	if ((errCheck = pthread_create(&creator[0], NULL, createCars, &cData[0]))) 
	{
		fprintf(stderr, "error: pthread_create, %d\n", errCheck);
		return EXIT_FAILURE;
	}

	if ((errCheck = pthread_create(&creator[1], NULL, createCars, &cData[1]))) 
	{
		fprintf(stderr, "error: pthread_create, %d\n", errCheck);
		return EXIT_FAILURE;
	}

	if ((errCheck = pthread_join(creator[0], NULL)))
	{
		fprintf(stderr, "error: pthread_join, %d\n", errCheck);
	}

	if ((errCheck = pthread_join(creator[1], NULL)))
	{
		fprintf(stderr, "error: pthread_join, %d\n", errCheck);
	}

    return EXIT_SUCCESS;
}


/*
 * Applies inversion method to turn uniform distribution into exponential distribution.
 * Random uniform number will be between ]0,1[
 * Receives mean as input
*/
double exprand(double mean)
{
	double uniform;
	uniform = (rand() + 1) / (RAND_MAX + 2.0);	//Number in ]0,1[
	return -log(1-uniform) * mean;	//Inversion method
}

void *Going(void *arg){
	carData *data = (threadData *)arg;

	int tID = data->threadID;
	int dir = data->direction;

	pthread_mutex_lock(&screen);
		printf("tID: %i arrived at bridge %i\n", tID, dir);
	pthread_mutex_unlock(&screen);
	enterBridge(0, tID);
	pthread_mutex_lock(&screen);
		printf("tID: %i entered bridge %i\n", tID, dir);
	pthread_mutex_unlock(&screen);
	sleep(1);
	exitBridge(0);
	pthread_mutex_lock(&screen);
		printf("tID: %i exited bridge %i\n", tID, dir);
	pthread_mutex_unlock(&screen);
	pthread_exit(NULL);
}

int canCross(int Direction){
	if(carsInBridge == 0)
		return TRUE;
	else if(CurrentDirection == Direction)
		return TRUE;
	else
		return FALSE;
}

void enterBridge(int Direction, int tID){
	pthread_mutex_lock(&lock);
		if(!canCross(Direction)){
			Waiting[Direction]++;
			pthread_mutex_lock(&screen);
				printf("tID: %i waiting\n", tID);
			pthread_mutex_unlock(&screen);
			sem_wait(&Semaphore[Direction]);
			Waiting[Direction]--;
		}
		carsInBridge++;
		CurrentDirection = Direction;
	pthread_mutex_unlock(&lock);
}

void exitBridge(int Direction){
	pthread_mutex_lock(&lock);
		carsInBridge--;
		if(carsInBridge>0)
			sem_post(&Semaphore[Direction]);
		else{
			if( Waiting[1-Direction] != 0 ){
				sem_post(&Semaphore[1-Direction]);
			}
			else{
				sem_post(&Semaphore[Direction]);
			}
		}
	pthread_mutex_unlock(&lock);
}

void *createCars(void *arg){
	threadData *data =  (threadData *)arg;
	int qty = data->qty;
	int dir = data->direction;
	int errCheck;

	pthread_t thread[qty];
	carData cData[qty];

	for(int i=0;i<qty;i++){
		cData[i].threadID = i;
		cData[i].direction = dir;
		if(errCheck = pthread_create(&thread[i], NULL, Going, &cData[i])){
			fprintf(stderr, "error: pthread_create, %d\n", errCheck);
            return EXIT_FAILURE;
		}
		sleep(exprand(mean));
	}

	for (int i = 0; i < qty; i++)
    {
        if ((errCheck = pthread_join(thread[i], NULL)))
        {
            fprintf(stderr, "error: pthread_join, %d\n", errCheck);
        }
    }
}
