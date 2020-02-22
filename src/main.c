#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <pthread.h>
#include <semaphore.h>
#include <math.h>
#include <time.h>

// Preprocessor macros

#define FALSE 	0
#define TRUE 	1
#define TO_EAST 0
#define TO_WEST 1

// Thread arguments
typedef struct threadData{ int threadID; int qty; }threadData;

//Bridge crossing directions, none equals no car on bridge
enum {None, toEast, toWest} crossingDirection;

sem_t semToEast, semToWest, mutex;
sem_t sem;

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

int mediaWest = 0;
int mediaEast = 0;

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
  //pthread_mutex_lock(&lock);
  if(!canCross(Direction)){
    Waiting[Direction]++;
    //pthread_mutex_lock(&screen);
    if (Direction == TO_WEST){
      printf("TO_WEST: %i is waiting ....\n\n", tID);
    }else{
      printf("TO_EAST: %i is waiting ....\n\n", tID);
    }
    //pthread_mutex_unlock(&screen);
    //sem_wait(&Semaphore[Direction]);
    sem_wait(&sem);
    Waiting[Direction]--;
  }
  carsInBridge++;
  CurrentDirection = Direction;
  printf("\033[1;31m");
  if (Direction == TO_WEST){
    printf("Car is comming into the bridge . BRIDGE Total cars %i TO_WEST \n", carsInBridge);
  }else{
    printf("Car is comming into the bridge . BRIDGE Total cars %i TO_EAST\n", carsInBridge);
  }
  printf("\033[0m");
  //pthread_mutex_unlock(&lock);
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

void exitBridge(int Direction){
  //pthread_mutex_lock(&lock);
  //carsInBridge--;
  if(carsInBridge>0){
    //sem_post(&Semaphore[Direction]);
    carsInBridge--;
    sem_post(&sem);
    printf("\033[1;31m");
    if (Direction == TO_WEST){
      printf("Car leaving the bridge. BRIDGE Total cars %i TO_WEST\n", carsInBridge);
    }else{
      printf("Car leaving the bridge. BRIDGE Total cars %i TO_EAST\n", carsInBridge);
    }
    printf("\033[0m");
  } else{
    printf("BRIDGE Empty\n");

    //if( Waiting[1-Direction] != 0 ){
    //sem_post(&Semaphore[1-Direction]);
    //}
    //else{
    //fsem_post(&Semaphore[Direction]);
    //}

  }
  //pthread_mutex_unlock(&lock);
}

void beforeEnterBridge(int Direction, int tID)
{
  int km_before_bridge = 4;
  for (int km = km_before_bridge; km > 0; --km  ){
    pthread_mutex_lock(&screen);
    printf("\033[0;33m");
    if (Direction == TO_WEST){
      printf("                                               New car TO_WEST: %i is %ikm before bridge \n", tID ,km);
    }else{
      printf("                                              New car TO_EAST: %i is %ikm before bridge \n", tID ,km);
    }
    printf("\033[0m");
    sleep(1);
    pthread_mutex_unlock(&screen);
  }
}


/*
 * Cars that will do checks going West
*/
void *GoingWest(void *arg)
{
  threadData *data = (threadData *)arg;

  int tID = data->threadID;
  beforeEnterBridge(TO_WEST,tID);
  //pthread_mutex_lock(&screen);
  printf("\n\nTO_WEST: %i arrived at bridge GoingWest\n", tID);
  //pthread_mutex_unlock(&screen);

  enterBridge(TO_WEST, tID);
  //pthread_mutex_lock(&screen);
  printf("TO_WEST: %i entered bridge GoingWest\n\n", tID);
  //pthread_mutex_unlock(&screen);

  sleep(1);
  exitBridge(TO_WEST);
  pthread_mutex_lock(&screen);
  printf(".... TO_WEST: %i exited bridge GoingWest\n\n", tID);
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

  beforeEnterBridge(TO_EAST,tID);
  //pthread_mutex_lock(&screen);
  printf("TO_EAST: %i arrived at bridge GoingEast\n\n", tID);
  //pthread_mutex_unlock(&screen);

  enterBridge(TO_EAST, tID);
  //pthread_mutex_lock(&screen);
  printf("TO_EAST: %i entered bridge GoingEast\n\n", tID);
  //pthread_mutex_unlock(&screen);

  sleep(1);
  exitBridge(TO_EAST);
  pthread_mutex_lock(&screen);
  printf(".... TO_EAST: %i exited bridge GoingEast\n\n", tID);
  pthread_mutex_unlock(&screen);
  pthread_exit(NULL);
}

void* createEastCars(void *arg){
  threadData *data =  (threadData *)arg;
  int qty = data->qty;
  int errCheck;
  pthread_t thread[qty];
  threadData cData[qty];
  for(int i=0;i<qty;i++){
    cData[i].threadID = i;
    if((errCheck = pthread_create(&thread[i], NULL, GoingWest, &cData[i]))){
      fprintf(stderr, "error: pthread_create, %d\n", errCheck);
    }
    sleep(exprand(mediaEast));
  }

  for (int i = 0; i < qty; ++i)
  {
    if ((errCheck = pthread_join(thread[i], NULL)))
    {
      fprintf(stderr, "error: pthread_join, %d\n", errCheck);
    }
  }
  return NULL;
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
    if((errCheck = pthread_create(&thread[i], NULL, GoingEast, &cData[i]))){
      fprintf(stderr, "error: pthread_create, %d\n", errCheck);
    }
    sleep(exprand(mediaWest));
  }

  for (int i = 0; i < qty; i++)
  {
    if ((errCheck = pthread_join(thread[i], NULL)))
    {
      fprintf(stderr, "error: pthread_join, %d\n", errCheck);
    }
  }
  return NULL;
}

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

  mediaEast = 5;
  mediaWest = 10;


  threadData cData[2];
  pthread_t creator[2];

  sem_init(&Semaphore[0], 0, 0);
  sem_init(&Semaphore[1], 0, 1);

  int errCheck;

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
