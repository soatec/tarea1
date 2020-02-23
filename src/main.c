#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include "../include/road.h"
#include "../include/interface.h"

int main(int argc, char *argv[])
{
  int error_check;
  int opt;
  int total_cars;
  int from_west_to_east = -1;
  int from_east_to_west = -1;
  int east_mean = -1;
  int west_mean = -1;

  while ((opt = getopt(argc, argv, "e:w:x:y:")) != -1) {
    switch (opt) {
      case 'e':
        from_east_to_west = atoi(optarg);
        break;
      case 'w':
        from_west_to_east = atoi(optarg);
        break;
      case 'x':
        east_mean = atoi(optarg);
        break;
      case 'y':
        west_mean = atoi(optarg);
        break;
      default:
        fprintf(stderr, "Usage: %s -e #CarsEastToWest -w #CarsWestToEast -x #EastMean -y #WestMean\n", argv[0]);
        exit(EXIT_FAILURE);
    }
  }

  if (east_mean == -1) {
    fprintf(stderr, "-x #EastMean is mandatory\n");
    exit(EXIT_FAILURE);
  }

  if (west_mean == -1) {
    fprintf(stderr, "-y #WestMean is mandatory\n");
    exit(EXIT_FAILURE);
  }

  if (from_east_to_west == -1) {
    fprintf(stderr, "-e #CarsEastToWest is mandatory\n");
    exit(EXIT_FAILURE);
  }

  if (from_west_to_east == -1) {
    fprintf(stderr, "-o #CarsWestToEast is mandatory\n");
    exit(EXIT_FAILURE);
}

  fprintf(stdout, "\n****************\n* SOA: TAREA 1 *\n****************\n\n");
  fprintf(stdout, "Cars from east to  west: %u\n", from_east_to_west);
  fprintf(stdout, "East mean: %u\n", east_mean);
  fprintf(stdout, "Cars from west to  east: %u\n", from_west_to_east);
  fprintf(stdout, "West mean: %u\n", west_mean);

  thread_car_creator car_creator[2];
  pthread_t creator[2];
  pthread_t animation_thread;

  error_check = init_road_with_bridge();
  if (error_check != 0) {
    fprintf(stderr, "error: uninit_road_with_bridge, %i\n", error_check);
    exit(EXIT_FAILURE);
  }

  car_creator[TO_WEST].quantity = from_east_to_west;
  car_creator[TO_WEST].mean = west_mean;
  car_creator[TO_EAST].quantity = from_west_to_east;
  car_creator[TO_EAST].mean = east_mean;
  total_cars = from_east_to_west + from_west_to_east;
  error_check = pthread_create(&animation_thread, NULL, animation, &total_cars);
  if (error_check != 0) {
    stop_animation();
    fprintf(stderr, "error: pthread_create, %i\n", error_check);
    exit(EXIT_FAILURE);
  }
  sleep(1);

  error_check = pthread_create(&creator[TO_WEST], NULL, create_cars_going_east_to_west, &car_creator[TO_WEST]);
  if (error_check != 0) {
    stop_animation();
    fprintf(stderr, "error: pthread_create, %i\n", error_check);
    exit(EXIT_FAILURE);
  }

  error_check = pthread_create(&creator[TO_EAST], NULL, create_cars_going_west_to_east, &car_creator[TO_EAST]);
  if (error_check != 0) {
    stop_animation();
    fprintf(stderr, "error: pthread_create, %i\n", error_check);
    exit(EXIT_FAILURE);
  }

  error_check = pthread_join(creator[TO_WEST], NULL);
  if (error_check != 0) {
    stop_animation();
    fprintf(stderr, "error: pthread_join, %i\n", error_check);
    exit(EXIT_FAILURE);
  }

  error_check = pthread_join(creator[TO_EAST], NULL);
  if (error_check != 0) {
    stop_animation();
    fprintf(stderr, "error: pthread_join, %i\n", error_check);
    exit(EXIT_FAILURE);
  }

  stop_animation();

  error_check = pthread_join(animation_thread, NULL);
  if (error_check != 0) {
    fprintf(stderr, "error: pthread_join, %i\n", error_check);
    exit(EXIT_FAILURE);
  }

  error_check = uninit_road_with_bridge();
  if (error_check != 0) {
    fprintf(stderr, "error: uninit_road_with_bridge, %i\n", error_check);
    exit(EXIT_FAILURE);
  }

  exit(EXIT_SUCCESS);
}