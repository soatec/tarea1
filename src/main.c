#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <pthread.h>
#include <semaphore.h>
#include <math.h>
#include <time.h>

//••••••••••••••••••••••••••••••••••••••••••••••••••••••••••••••••••••••GLOBALS

// Thread arguments
typedef struct threadData{ int threadID; }threadData;

//Bridge crossing directions, none equals no car on bridge
enum {None, toEast, toWest} crossingDirection;

sem_t semToEast, semToWest, mutex;
int crosingCounter, waitingToEastCounter, waitingToWestCounter;

//••••••••••••••••••••••••••••••••••••••••••••••••••••••••••••••••••••FUNCTIONS

//Implicit declarations
double exprand(double mean);
void *GoingWest(void *arg);
void *GoingEast(void *arg);

int main(int argc, char *argv[])
{
    int opt;
    int fromWest = -1;
    int fromEast  = -1;

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

    int totalCars = fromEast+fromWest;
    int remainingGoingEast = fromWest;
    int remainingGoingWest = fromEast;
	threadData tData[totalCars];
	pthread_t thread[totalCars];

	int errCheck;
    int i;
    for (i=0;i<(totalCars);++i)
    {
		tData[i].threadID = i;

    	void *thread_func;
		//Some chance to create a car GoingWest or GoingEast
		if (rand()%totalCars <= totalCars/2)
		{
			if (remainingGoingEast > 0)
			{
				thread_func = GoingEast;
				remainingGoingEast--;
			}
			else
			{
				thread_func = GoingWest;
				remainingGoingWest--;
			}
		} 
		else
		{
			if (remainingGoingWest > 0)
			{
				thread_func = GoingWest;
				remainingGoingWest--;
			}
			else
			{
				thread_func = GoingEast;
				remainingGoingEast--;
			}
		}

		//Create thread
		if ((errCheck = pthread_create(&thread[i], NULL, thread_func, &tData[i]))) 
		{
            fprintf(stderr, "error: pthread_create, %d\n", errCheck);
            return EXIT_FAILURE;
    	}

    	//Wait before creating next thread
    	sleep(exprand(1));
    }

    //Wait for threads to end
    for (int i = 0; i < totalCars; ++i)
    {
        if ((errCheck = pthread_join(thread[i], NULL)))
        {
            fprintf(stderr, "error: pthread_join, %d\n", errCheck);
        }
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

/*
 * Cars that will do checks going West
*/
void *GoingWest(void *arg)
{
	threadData *data = (threadData *)arg;

	int tID = data->threadID;
	printf("tID: %i GoingWest\n", tID);
	pthread_exit(NULL);
}

/*
 * Cars that will do checks going East
*/
void *GoingEast(void *arg)
{
	threadData *data = (threadData *)arg;

	int tID = data->threadID;
	printf("tID: %i GoingEast\n", tID);
	pthread_exit(NULL);
}
