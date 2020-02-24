#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include <math.h>
#include <time.h>
#include "interface.h"

// Preprocessor macros

#define FALSE 0
#define TRUE 1
#define TO_EAST 0
#define TO_WEST 1

// Thread arguments
typedef struct thread_data
{
  int thread_id;
  int quantity;
} thread_data;

sem_t semaphore;
pthread_mutex_t screen;

int current_direction = -1;
int cars_in_bridge = 0;
int west_to_east_mean = 0;
int east_to_west_mean = 0;

int bridge_can_be_crossed(int Direction){
  if(cars_in_bridge == 0){
    printf("Bridge empty ....\n");
    return TRUE;
  } else if(current_direction == Direction){
    printf("Same direction ....\n");
    return TRUE;
  } else{
    printf("can't cross ....\n");
    return FALSE;
  }
}

void enter_bridge(int direction, int thread_id){
  int error_check;
  if(!bridge_can_be_crossed(direction)){
    if (direction == TO_WEST){
      printf("TO_WEST: %i is waiting ....\n\n", thread_id);
    }else{
      printf("TO_EAST: %i is waiting ....\n\n", thread_id);
    }
    error_check = sem_wait(&semaphore);
    if (error_check != 0) {
      printf("There is an error for sem_wait: %i ....\n", error_check);
    }
  }
  cars_in_bridge++;
  current_direction = direction;
  printf("\033[1;31m");
  if (direction == TO_WEST){
    printf("Car is coming into the bridge. BRIDGE Total cars %i TO_WEST \n", cars_in_bridge);
  }else{
    printf("Car is coming into the bridge. BRIDGE Total cars %i TO_EAST\n", cars_in_bridge);
  }
  printf("\033[0m");
}

/*
 * Applies inversion method to turn uniform distribution into exponential distribution.
 * Random uniform number will be between ]0,1[
 * Receives mean as input
*/
double exponential_random(double mean)
{
  double uniform;
  uniform = (rand() + 1) / (RAND_MAX + 2.0);	//Number in ]0,1[
  return -log(1-uniform) * mean;	//Inversion method
}

void exit_bridge(int direction){
  int error_check;
  if(cars_in_bridge > 0){
    cars_in_bridge--;
    error_check = sem_post(&semaphore);
    if (error_check != 0) {
      printf("There is an error for sem_post: %i ....\n", error_check);
    }
    printf("\033[1;31m");
    if (direction == TO_WEST){
      printf("Car leaving the bridge. BRIDGE Total cars %i TO_WEST\n", cars_in_bridge);
    }else{
      printf("Car leaving the bridge. BRIDGE Total cars %i TO_EAST\n", cars_in_bridge);
    }
    printf("\033[0m");
  } else{
    printf("The bridge is empty\n");
  }
}

void continue_road_before_bridge(int direction, int thread_id)
{
  int km_before_bridge = 4;
  for (int km = km_before_bridge; km > 0; --km ){
    pthread_mutex_lock(&screen);
    printf("\033[0;33m");
    if (direction == TO_WEST){
      printf("        New car TO_WEST: %i is %ikm before bridge \n", thread_id , km);
    }else{
      printf("        New car TO_EAST: %i is %ikm before bridge \n", thread_id , km);
    }
    printf("\033[0m");
    sleep(1);
    pthread_mutex_unlock(&screen);
  }
}


/*
 * Cars that will do checks going West
*/
void* go_east_to_west(void *arg)
{
  thread_data *data = (thread_data*)arg;
  int thread_id = data->thread_id;

  continue_road_before_bridge(TO_WEST, thread_id);
  printf("\n\nTO_WEST: %i arrived at bridge go_east_to_west\n", thread_id);

  enter_bridge(TO_WEST, thread_id);
  printf("TO_WEST: %i entered bridge go_east_to_west\n\n", thread_id);

  sleep(10);
  exit_bridge(TO_WEST);
  pthread_mutex_lock(&screen);
  printf(".... TO_WEST: %i exited bridge go_east_to_west\n\n", thread_id);
  pthread_mutex_unlock(&screen);
  pthread_exit(NULL);
}

/*
 * Cars that will do checks going East
*/
void* go_west_to_east(void *arg)
{
  thread_data *data = (thread_data *)arg;

  int tID = data->thread_id;

  continue_road_before_bridge(TO_EAST, tID);
  printf("TO_EAST: %i arrived at bridge go_west_to_east\n\n", tID);

  enter_bridge(TO_EAST, tID);
  printf("TO_EAST: %i entered bridge go_west_to_east\n\n", tID);

  sleep(10);
  exit_bridge(TO_EAST);
  pthread_mutex_lock(&screen);
  printf(".... TO_EAST: %i exited bridge go_west_to_east\n\n", tID);
  pthread_mutex_unlock(&screen);
  pthread_exit(NULL);
}

void* create_cars_going_east_to_west(void *arg){
  thread_data *data = (thread_data *)arg;
  int qty = data->quantity;
  int errCheck;
  pthread_t thread[qty];
  thread_data cData[qty];
  for(int i=0;i<qty;i++){
    cData[i].thread_id = i;
    if((errCheck = pthread_create(&thread[i], NULL, go_east_to_west, &cData[i]))){
      fprintf(stderr, "error: pthread_create, %d\n", errCheck);
    }
    sleep(exponential_random(east_to_west_mean));
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

void* create_cars_going_west_to_east(void *arg){
  thread_data *data =  (thread_data *)arg;
  int qty = data->quantity;
  int errCheck;
  printf("%i\n", qty);
  pthread_t thread[qty];
  thread_data cData[qty];
  for(int i=0;i<qty;i++){
    cData[i].thread_id = i;
    if((errCheck = pthread_create(&thread[i], NULL, go_west_to_east, &cData[i]))){
      fprintf(stderr, "error: pthread_create, %d\n", errCheck);
    }
    sleep(exponential_random(west_to_east_mean));
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
  animation();
  /*
  int error_check;
  int opt;
  int from_west_to_east = -1;
  int from_east_to_west = -1;

  while ((opt = getopt(argc, argv, "e:o:")) != -1) {
    switch (opt) {
      case 'e':
        from_east_to_west = atoi(optarg);
        break;
      case 'o':
        from_west_to_east = atoi(optarg);
        break;
      default:
        fprintf(stderr, "Uso: %s -e #CarrosDelEste -o #CarrosDelOeste\n", argv[0]);
        exit(EXIT_FAILURE);
    }
  }

  if (from_east_to_west == -1) {
    fprintf(stderr, "-e #CarrosDelEste es un parámetro obligatorio\n");
    exit(EXIT_FAILURE);
  }

  if (from_west_to_east == -1) {
    fprintf(stderr, "-o #CarrosDelOeste es un parámetro obligatorio\n");
    exit(EXIT_FAILURE);
  }

  fprintf(stdout, "\n****************\n* SOA: TAREA 1 *\n****************\n\n");
  fprintf(stdout, "Cantidad de carros del este: %u\n", from_east_to_west);
  fprintf(stdout, "Cantidad de carros del oeste: %u\n", from_west_to_east);

  east_to_west_mean = 5;
  west_to_east_mean = 10;

  thread_data threads_data[2];
  pthread_t creator[2];

  threads_data[0].quantity = from_east_to_west;
  threads_data[1].quantity = from_west_to_east;

  if ((error_check = pthread_create(&creator[0], NULL, create_cars_going_east_to_west, &threads_data[0])))
  {
    fprintf(stderr, "error: pthread_create, %d\n", error_check);
    return EXIT_FAILURE;
  }

  if ((error_check = pthread_create(&creator[1], NULL, create_cars_going_west_to_east, &threads_data[1])))
  {
    fprintf(stderr, "error: pthread_create, %d\n", error_check);
    return EXIT_FAILURE;
  }

  if ((error_check = pthread_join(creator[0], NULL)))
  {
    fprintf(stderr, "error: pthread_join, %d\n", error_check);
  }

  if ((error_check = pthread_join(creator[1], NULL)))
  {
    fprintf(stderr, "error: pthread_join, %d\n", error_check);
  }

  return EXIT_SUCCESS;
  */
}
