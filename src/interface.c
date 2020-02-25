#include "../include/interface.h"
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>

//Preprocessor macros
#define DELAY 125000
#define STREET_BORDER_PAIR 1
#define CAR_EAST_PAIR 2
#define CAR_WEAST_PAIR 3
#define LOGW_PAIR 4
#define LOGW_EMPH_PAIR 5

//Structures
struct point {
  int x;
  int y;
};

typedef struct car {
  int px;
  int direction;
} car;

struct point street_i, street_f, bridge_i, bridge_f;

//Global variables
car *cars;
pthread_mutex_t mutex;
int finish = 0;
int current_cart = 0;
int max_x = 0;
int street_height;
int street_len;
int bridge_len;
WINDOW *logw;
WINDOW *picw;

void* animation(void* arg)
{
  int quantity = *(int*)arg;
  cars = malloc(sizeof(car)*quantity);
  int max_y = 0;
  int x, y;
  int street_line_len;

  pthread_mutex_init(&mutex, NULL);

  initscr();
  noecho();
  curs_set(false);

  if (has_colors() == false) {
    endwin();
    printf("Your terminal doesn't support color\n");
    exit(EXIT_FAILURE);
  }

  start_color();

  init_pair(1, COLOR_YELLOW, COLOR_BLACK);

  init_pair(2, COLOR_GREEN, COLOR_BLACK);

  init_pair(3, COLOR_BLUE, COLOR_BLACK);

  init_pair(4, COLOR_BLACK, COLOR_WHITE);

  init_pair(5, COLOR_RED, COLOR_WHITE);

  for (int i = 0; i < quantity; i++){
    cars[i].px = 400;
  }

  getmaxyx(stdscr, max_y, max_x);

  street_height = max_y * 0.2;
  street_len = max_x * 0.35;
  bridge_len = max_x * 0.2;
  street_i.x = max_x * 0.35;
  street_i.y = max_y * 0.1;
  street_f.x = max_x * 0.65;
  street_f.y = max_y * 0.5;
  bridge_i.x = max_x * 0.4;
  bridge_i.y = max_y * 0.2;
  bridge_f.x = max_x * 0.6;
  bridge_f.y = max_y * 0.4;
  street_line_len = 3;

  picw = newwin(max_y*0.6, max_x, 0, 0);
  logw = newwin(max_y*0.4, max_x, street_f.y + street_i.y, 0);
  refresh();

  scrollok(logw, true);
  wbkgd(logw, COLOR_PAIR(LOGW_PAIR));
  wrefresh(logw);

  while(1) {
    // Global var `stdscr` is created by the call to `initscr()`
    werase(picw);

    // Draw street border
    wattron(picw, COLOR_PAIR(STREET_BORDER_PAIR));
    wmove(picw, street_i.y, 0);
    whline(picw, '=', street_len);

    wmove(picw, street_i.y, street_f.x);
    whline(picw, '=', street_len);

    wmove(picw, street_f.y, 0);
    whline(picw, '=', street_len);

    wmove(picw, street_f.y, street_f.x);
    whline(picw, '=', street_len);

    wmove(picw, bridge_i.y, bridge_i.x);
    whline(picw, '=', bridge_len);

    wmove(picw, bridge_f.y, bridge_i.x);
    whline(picw, '=', bridge_len);

    x = street_i.x;
    y = street_i.y;
    while(x < bridge_i.x && y < bridge_i.y) {
      mvwprintw(picw, y, x, " # ");
      x++;
      y++;
    }

    x = bridge_f.x;
    y = bridge_i.y;
    while(x < street_f.x && y > street_i.y) {
      mvwprintw(picw, y, x, " # ");
      x++;
      y--;
    }

    x = bridge_f.x;
    y = bridge_f.y;
    while(x < street_f.x && y < street_f.y) {
      mvwprintw(picw, y, x, " # ");
      x++;
      y++;
    }

    x = street_i.x;
    y = street_f.y;
    while(x < bridge_i.x && y > bridge_f.y) {
      mvwprintw(picw, y, x, " # ");
      x++;
      y--;
    }
    wattroff(picw,COLOR_PAIR(STREET_BORDER_PAIR));

    // Draw street signs
    for (x = 0; x < street_len; x+=street_line_len) {
      if ((x % 2) == 0) continue;
      wmove(picw, street_i.y + street_height, x);
      whline(picw, '-', street_line_len);
    }

    for (x = street_f.x;
         x < street_f.x + street_len;
         x+=street_line_len) {
      if ((x % 2) == 0) continue;
      wmove(picw, street_i.y + street_height, x);
      whline(picw, '-', street_line_len);
    }

    // Dibujar carros aquí
    for (int i = 0; i < quantity; i++){
      draw_car(cars[i].px, cars[i].direction);
    }

    wrefresh(picw);
    usleep(DELAY);

    if (finish == 1){
      break;
    }
  }
  pthread_mutex_destroy(&mutex);
  delwin(picw);
  delwin(logw);
  endwin();
  free(cars);
  printf("\033[1;31m---> Execution has finished. Thank you! <---\n\033[0m");
  pthread_exit(NULL);
}

screen_size_data get_screen_size_data(){
  screen_size_data data;
  data.max_x = max_x;
  data.before_bridge = street_i.x;
  data.after_bridge = street_f.x;
  return data;
}

int get_car_id() {
  int id;
  pthread_mutex_lock(&mutex);
  id = current_cart;
  current_cart++;
  pthread_mutex_unlock(&mutex);
  return id;
}

void stop_animation() {
  finish = 1;
}

void draw_moving_car(int id, int px, int direction){
  pthread_mutex_lock(&mutex);
  cars[id].px = px;
  cars[id].direction = direction;
  pthread_mutex_unlock(&mutex);
}

void draw_car(int px, int direction) {
  int y;
  if (direction == 1) {
    y = bridge_i.y;
    if (px >= street_i.x && px <= street_f.x) y = bridge_i.y + street_height / 2;
    wattron(picw, COLOR_PAIR(CAR_EAST_PAIR));
    mvwprintw(picw, y-1,   px, " 0 o ");
    mvwprintw(picw, y+1-1, px, "= X|>");
    mvwprintw(picw, y+2-1, px, " 0 o ");
    wattroff(picw, COLOR_PAIR(CAR_EAST_PAIR));
  } else {
    y = bridge_f.y;
    if (px <= street_f.x && px >= street_i.x) y = bridge_i.y + street_height / 2;
    wattron(picw, COLOR_PAIR(CAR_WEAST_PAIR));
    mvwprintw(picw, y-1,   px, " o 0 ");
    mvwprintw(picw, y+1-1, px, "<|X =");
    mvwprintw(picw, y+2-1, px, " o 0 ");
    wattroff(picw, COLOR_PAIR(CAR_WEAST_PAIR));
  }
}

void write_log(char *message, bool emphasis) {
  if (emphasis) {
    wattron(logw, COLOR_PAIR(LOGW_EMPH_PAIR));
  }
  wprintw(logw, message);
  if (emphasis) {
    wattroff(logw, COLOR_PAIR(LOGW_EMPH_PAIR));
  }
  wrefresh(logw);
}
