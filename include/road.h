#ifndef TAREA1_ROAD_H
#define TAREA1_ROAD_H

// Preprocessor macros
#define TO_EAST 0
#define TO_WEST 1

//Structures
typedef struct thread_car_creator
{
  int mean;
  int quantity;
} thread_car_creator;

int init_road_with_bridge();

int uninit_road_with_bridge();

void* create_cars_going_east_to_west(void *arg);

void* create_cars_going_west_to_east(void *arg);

#endif //TAREA1_ROAD_H