#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <pthread.h>
#include <semaphore.h>
#include <math.h>
#include <time.h>

//••••••••••••••••••••••••••••••••••••••••••••••••••••••••••••••••••••••GLOBALS\

#define FALSE 	0
#define TRUE 	1
#define TO_EAST 0
#define TO_WEST 1

// Thread arguments
typedef struct threadData{ int threadID; int qty; }threadData;

//Bridge crossing directions, none equals no car on bridge
enum {None, toEast, toWest} crossingDirection;

sem_t semToEast, semToWest, mutex;
int crosingCounter, waitingToEastCounter, waitingToWestCounter;

pthread_mutex_t lock;
pthread_mutex_t screen;
int Waiting[2];
sem_t Semaphore[2];
int CurrentDirection = -1;

int fromWest = -1;
int fromEast  = -1;
int carsInBridge = 0;

int totalCars = -1;
int remainingGoingEast = -1;
int remainingGoingWest = -1;

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

    while ((opt = getopt(argc, argv, "e:o:")) != -1) {
        switch (opt) {
        case 'e':
          fromEast = atoi(optarg);
          break;
        case 'o':
          fromWest = atoi(optarg);
          break;
        default:
            fprintf(stderr, "Uso: %s -e #CarrosDelEste -o #CarrosDelOeste\n", argv[0]);
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


	threadData cData[2];
	pthread_t creator[2];

	sem_init(&Semaphore[0], 0, 0);
	sem_init(&Semaphore[1], 0, 1);

	int errCheck;
    int i;

	cData[0].qty = fromEast;
	cData[1].qty = fromWest;


	if ((errCheck = pthread_create(&creator[0], NULL, createEastCars, &cData[0])))
	{
		fprintf(stderr, "error: pthread_create, %d\n", errCheck);
		return EXIT_FAILURE;
	}

	if ((errCheck = pthread_create(&creator[1], NULL, createWestCars, &cData[1])))
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

    // for (i=0;i<(totalCars);++i)
    // {
	// 	tData[i].threadID = i;

    // 	void *thread_func;
	// 	//Some chance to create a car GoingWest or GoingEast
	// 	if (rand()%totalCars <= totalCars/2)
	// 	{
	// 		if (remainingGoingEast > 0)
	// 		{
	// 			thread_func = GoingEast;
	// 			remainingGoingEast--;
	// 		}
	// 		else
	// 		{
	// 			thread_func = GoingWest;
	// 			remainingGoingWest--;
	// 		}
	// 	}
	// 	else
	// 	{
	// 		if (remainingGoingWest > 0)
	// 		{
	// 			thread_func = GoingWest;
	// 			remainingGoingWest--;
	// 		}
	// 		else
	// 		{
	// 			thread_func = GoingEast;
	// 			remainingGoingEast--;
	// 		}
	// 	}

	// 	//Create thread
	// 	if ((errCheck = pthread_create(&thread[i], NULL, thread_func, &tData[i])))
	// 	{
    //         fprintf(stderr, "error: pthread_create, %d\n", errCheck);
    //         return EXIT_FAILURE;
    // 	}

    // 	//Wait before creating next thread
    // 	sleep(exprand(1));
    // }

    // //Wait for threads to end
    // for (int i = 0; i < totalCars; ++i)
    // {
    //     if ((errCheck = pthread_join(thread[i], NULL)))
    //     {
    //         fprintf(stderr, "error: pthread_join, %d\n", errCheck);
    //     }
    // }

    // return EXIT_SUCCESS;
// }

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

/*
 * Cars that will do checks going West
*/
void *GoingWest(void *arg)
{
	threadData *data = (threadData *)arg;

	int tID = data->threadID;
	pthread_mutex_lock(&screen);
		printf("East Car: %i arrived at bridge GoingWest\n", tID);
	pthread_mutex_unlock(&screen);
	enterBridge(TO_WEST, tID);
	pthread_mutex_lock(&screen);
		printf("East Car: %i entered bridge GoingWest\n", tID);
	pthread_mutex_unlock(&screen);
	sleep(1);
	exitBridge(TO_WEST);
	pthread_mutex_lock(&screen);
		printf("East Car: %i exited bridge GoingWest\n", tID);
	pthread_mutex_unlock(&screen);
	pthread_exit(NULL);
}

/*
 * Cars that will do checks going East
*/
void *GoingEast(void *arg)
{
	threadData *data = (threadData *)arg;

	int tID = data->threadID;

	pthread_mutex_lock(&screen);
		printf("West car: %i arrived at bridge GoingEast\n", tID);
	pthread_mutex_unlock(&screen);
	enterBridge(TO_EAST, tID);
	pthread_mutex_lock(&screen);
		printf("West car: %i entered bridge GoingEast\n", tID);
	pthread_mutex_unlock(&screen);
	sleep(1);
	exitBridge(TO_EAST);
	pthread_mutex_lock(&screen);
		printf("West car: %i exited bridge GoingEast\n", tID);
	pthread_mutex_unlock(&screen);
	pthread_exit(NULL);
}

int canCross(int Direction){
	if(carsInBridge == 0){
    printf("Bridge empty ....\n");
		return TRUE;
  }
	else if(CurrentDirection == Direction){
      printf("Same direction ....\n");
		return TRUE;
  }
	else{
    printf("cant cross ....\n");
		return FALSE;
  }
}

void enterBridge(int Direction, int tID){
	pthread_mutex_lock(&lock);
		if(!canCross(Direction)){
			Waiting[Direction]++;
			//pthread_mutex_lock(&screen);
        if (Direction == TO_WEST){
          printf("East car: %i is waiting ....\n\n", tID);
        }else{
          printf("West car: %i is waiting ....\n\n", tID);
        }
			//pthread_mutex_unlock(&screen);
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

void *createEastCars(void *arg){
	threadData *data =  (threadData *)arg;
	int qty = data->qty;
	int errCheck;
	pthread_t thread[qty];
	threadData cData[qty];
	for(int i=0;i<qty;i++){
		cData[i].threadID = i;
		if(errCheck = pthread_create(&thread[i], NULL, GoingWest, &cData[i])){
			fprintf(stderr, "error: pthread_create, %d\n", errCheck);
            return EXIT_FAILURE;
		}
		sleep(exprand(1));
	}

	for (int i = 0; i < qty; ++i)
    {
        if ((errCheck = pthread_join(thread[i], NULL)))
        {
            fprintf(stderr, "error: pthread_join, %d\n", errCheck);
        }
    }
}

void *createWestCars(void *arg){
	threadData *data =  (threadData *)arg;
	int qty = data->qty;
	int errCheck;

	printf("%i\n", qty);
	pthread_t thread[qty];
	threadData cData[qty];
	for(int i=0;i<qty;i++){
		cData[i].threadID = i;
		if(errCheck = pthread_create(&thread[i], NULL, GoingEast, &cData[i])){
			fprintf(stderr, "error: pthread_create, %d\n", errCheck);
            return EXIT_FAILURE;
		}
		sleep(exprand(1));
	}

	for (int i = 0; i < qty; i++)
    {
        if ((errCheck = pthread_join(thread[i], NULL)))
        {
            fprintf(stderr, "error: pthread_join, %d\n", errCheck);
        }
    }
}
