#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <math.h>
#include <string.h>
#include "../include/road.h"
#include "../include/interface.h"

// Preprocessor macros
#define FALSE 0
#define TRUE 1
#define TIME_SLEEPING 20000

// Structs
typedef struct thread_data_car
{
  int thread_id;
  int direction;
  int interface_car_id;
  int position;
  int stop_point;
  int end_bridge_point;
  int end_road_point;
} thread_data_car;

// Global variables
pthread_cond_t bridge_crossing_done[2];
pthread_mutex_t mutex;
int current_direction = -1;
int cars_in_bridge = 0;

//Private functions
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

//Public functions
int init_road_with_bridge(){
  int error_check;
  error_check = pthread_mutex_init(&mutex, NULL);
  if (error_check != 0) {
    return error_check;
  }
  error_check = pthread_cond_init(&bridge_crossing_done[TO_WEST], NULL);
  if (error_check != 0) {
    return error_check;
  }
  error_check = pthread_cond_init(&bridge_crossing_done[TO_EAST], NULL);
  return error_check;
}

int uninit_road_with_bridge(){
  int error_check;
  error_check = pthread_mutex_destroy(&mutex);
  if (error_check != 0) {
    return error_check;
  }
  error_check = pthread_cond_destroy(&bridge_crossing_done[TO_WEST]);
  if (error_check != 0) {
    return error_check;
  }
  error_check = pthread_cond_destroy(&bridge_crossing_done[TO_EAST]);
  return error_check;
}

int bridge_can_be_crossed(thread_data_car *car){
  char message[512];

  if(cars_in_bridge == 0){
    sprintf(message, "Car %i.%i found that the bridge is empty.\n", car->direction, car->thread_id);
    write_log(message, false);
    return TRUE;
  } else if(current_direction == car->direction){
    sprintf(message,"Car %i.%i found that cars inside the bridge follow the same direction.\n", car->direction, car->thread_id);
    write_log(message, false);
    return TRUE;
  } else{
    sprintf(message, "Car %i.%i found that cars inside the bridge don't follow the same direction, so it can't cross.\n", car->direction, car->thread_id);
    write_log(message, false);
    return FALSE;
  }
}

void enter_bridge(thread_data_car *car){
  char message[512];
  int error_check;
  pthread_mutex_lock(&mutex);
  if (car->direction == TO_WEST){
    sprintf(message, "Car %i.%i going to west is trying to enter the bridge.\n", car->direction, car->thread_id);
  }else{
    sprintf(message, "Car %i.%i going to east is trying to enter the bridge.\n", car->direction, car->thread_id);
  }
  write_log(message, false);

  if(!bridge_can_be_crossed(car)){
    if (car->direction == TO_WEST){
      sprintf(message,"Car %i.%i going to west needs to wait ", car->direction, car->thread_id);
    }else{
      sprintf(message,"Car %i.%i going to east needs to wait ", car->direction, car->thread_id);
    }
    write_log(message, false);

    if (current_direction == TO_WEST){
      sprintf(message,"since current direction is east to west and there are %i cars.\n", cars_in_bridge);
    } else {
      sprintf(message,"since current direction is west to east and there are %i cars.\n", cars_in_bridge);
    }
    write_log(message, false);

    error_check = pthread_cond_wait(&bridge_crossing_done[car->direction], &mutex);
    if (error_check != 0) {
      sprintf(message, "error: pthread_cond_wait, %i\n", error_check);
      write_log(message, false);
    }

    if (car->direction == TO_WEST){
      sprintf(message, "Car %i.%i going to west has awaken: ", car->direction, car->thread_id);
    }else{
      sprintf(message, "Car %i.%i going to east has awaken: ", car->direction, car->thread_id);
    }
    write_log(message, false);
    if (current_direction == TO_WEST){
      sprintf(message, "currently there are %i cars going to west\n", cars_in_bridge);
    } else {
      sprintf(message, "currently there are %i cars going to east\n", cars_in_bridge);
    }
    write_log(message, false);
  }

  cars_in_bridge++;
  current_direction = car->direction;
  if (car->direction == TO_WEST){
    sprintf(message,"---> Car %i.%i going to west has entered the bridge. Total cars in bridge is %i <---\n", car->direction, car->thread_id, cars_in_bridge);
  }else{
    sprintf(message,"---> Car %i.%i going to east has entered the bridge. Total cars in bridge is %i <---\n", car->direction, car->thread_id, cars_in_bridge);
  }
  write_log(message, true);

  pthread_mutex_unlock(&mutex);
}

void exit_bridge(thread_data_car *car){
  char message[512];
  int error_check;
  pthread_mutex_lock(&mutex);
  cars_in_bridge--;
  if (car->direction == TO_WEST){
    sprintf(message,"---> Car %i.%i going to west leaving the bridge. Total cars in bridge %i. <---\n", car->direction, car->thread_id, cars_in_bridge);
  }else{
    sprintf(message,"---> Car %i.%i going to east leaving the bridge. Total cars in bridge %i. <---\n", car->direction, car->thread_id, cars_in_bridge);
  }
  write_log(message, true);
  if (cars_in_bridge == 0){
    if (car->direction == TO_WEST){
      error_check = pthread_cond_broadcast(&bridge_crossing_done[TO_EAST]);
    } else {
      error_check =  pthread_cond_broadcast(&bridge_crossing_done[TO_WEST]);
    }
    if (error_check != 0) {
      sprintf(message, "error: pthread_cond_wait, %i\n", error_check);
      write_log(message, false);
    }
  }
  pthread_mutex_unlock(&mutex);
}

void start_road(thread_data_car *car)
{
  if (car->direction == TO_WEST){
    for (int km = 0; km < car->stop_point; km++){
      draw_moving_car(car->interface_car_id, car->position++, 1);
      usleep(TIME_SLEEPING);
    }
  }else{
    for (int km = 0; km < car->stop_point; km++){
      draw_moving_car(car->interface_car_id, car->position--, 0);
      usleep(TIME_SLEEPING);
    }
  }
}

void cross_bridge(thread_data_car *car)
{
  if (car->direction == TO_WEST){
    for (int km = 0; km < car->end_bridge_point; km++){
      draw_moving_car(car->interface_car_id, car->position++, 1);
      usleep(TIME_SLEEPING*10);
    }
  }else{
    for (int km = 0; km < car->end_bridge_point; km++){
      draw_moving_car(car->interface_car_id, car->position--, 0);
      usleep(TIME_SLEEPING*10);
    }
  }
}

void continue_road(thread_data_car *car)
{
  if (car->direction == TO_WEST){
    for (int km = 0; km < car->end_road_point; km++){
      draw_moving_car(car->interface_car_id, car->position++, 1);
      usleep(TIME_SLEEPING);
    }
  }else{
    for (int km = 0; km < car->end_road_point; km++){
      draw_moving_car(car->interface_car_id, car->position--, 0);
      usleep(TIME_SLEEPING);
    }
  }
}

/*
 * Cars that will do checks going West
*/
void* go_east_to_west(void *arg)
{
  thread_data_car *car = (thread_data_car*)arg;
  start_road(car);
  enter_bridge(car);
  cross_bridge(car);
  exit_bridge(car);
  continue_road(car);
  pthread_exit(NULL);
}

/*
 * Cars that will do checks going East
*/
void* go_west_to_east(void *arg)
{
  thread_data_car *car = (thread_data_car *)arg;
  start_road(car);
  enter_bridge(car);
  cross_bridge(car);
  exit_bridge(car);
  continue_road(car);
  pthread_exit(NULL);
}

void* create_cars_going_east_to_west(void *arg){
  char message[512];
  thread_car_creator *data = (thread_car_creator*)arg;
  screen_size_data screen_data;
  screen_data = get_screen_size_data();
  int quantity = data->quantity;
  int error_check;
  pthread_t thread[quantity];
  thread_data_car cars[quantity];
  for(int i = 0; i < quantity; i++){
    cars[i].thread_id = i;
    cars[i].direction = TO_WEST;
    cars[i].interface_car_id = get_car_id();
    cars[i].position = 0;
    cars[i].stop_point = screen_data.before_bridge;
    cars[i].end_road_point = screen_data.max_x - screen_data.after_bridge + 20;
    cars[i].end_bridge_point = screen_data.after_bridge - screen_data.before_bridge;
    error_check = pthread_create(&thread[i], NULL, go_east_to_west, &cars[i]);
    if (error_check != 0){
      sprintf(message, "error: pthread_create, %i\n", error_check);
      write_log(message, false);
    }
    sleep(exponential_random(data->mean));
  }

  for (int i = 0; i < quantity; i++) {
    error_check = pthread_join(thread[i], NULL);
    if (error_check != 0) {
      sprintf(message, "error: pthread_join, %i\n", error_check);
      write_log(message, false);
    }
  }
  return NULL;
}

void* create_cars_going_west_to_east(void *arg){
  char message[512];
  thread_car_creator *data =  (thread_car_creator *)arg;
  screen_size_data screen_data;
  screen_data = get_screen_size_data();
  int qty = data->quantity;
  int errCheck;
  pthread_t thread[qty];
  thread_data_car cars[qty];
  for(int i=0;i<qty;i++){
    cars[i].thread_id = i;
    cars[i].direction = TO_EAST;
    cars[i].interface_car_id = get_car_id();
    cars[i].position = screen_data.max_x;
    cars[i].stop_point = screen_data.max_x - screen_data.after_bridge;
    cars[i].end_road_point = screen_data.before_bridge + 20;
    cars[i].end_bridge_point = screen_data.after_bridge - screen_data.before_bridge;
    if((errCheck = pthread_create(&thread[i], NULL, go_west_to_east, &cars[i]))){
      sprintf(message, "error: pthread_create, %i\n", errCheck);
      write_log(message, false);
    }
    sleep(exponential_random(data->mean));
  }

  for (int i = 0; i < qty; i++) {
    errCheck = pthread_join(thread[i], NULL);
    if (errCheck != 0) {
      sprintf(message, "error: pthread_join, %i\n", errCheck);
      write_log(message, false);
    }
  }
  return NULL;
}