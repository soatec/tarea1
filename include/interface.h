#include <ncurses.h>

typedef struct screen_size_data {
  int max_x;
  int before_bridge;
  int after_bridge;
} screen_size_data;

void* animation(void* arg);

void draw_car(int px, int direction);

int get_car_id();

void draw_moving_car(int id, int px, int direction);

void write_log(char *message, bool emphasis);

void stop_animation();

screen_size_data get_screen_size_data();