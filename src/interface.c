#include "interface.h"
#include <unistd.h>
#include <stdlib.h>

#define DELAY 125000

#define STREET_BORDER_PAIR 1
#define CAR_EAST_PAIR 2
#define CAR_WEAST_PAIR 3

struct point {
  int x;
  int y;
};

struct point street_i, street_f, bridge_i, bridge_f;
int street_height;
int street_len;
int bridge_len;

void animation(void)
{
  int max_y = 0, max_x = 0;
  int x, y;
  int street_line_len;

  initscr();
  noecho();
  curs_set(false);

  if (has_colors() == false) {
    endwin();
    printf("Su terminal no soporta colores\n");
    exit(1);
  }

  start_color();
  init_pair(1, COLOR_YELLOW, COLOR_BLACK);

  init_pair(2, COLOR_GREEN, COLOR_BLACK);

  init_pair(3, COLOR_BLUE, COLOR_BLACK);

  while(1) {
  // Global var `stdscr` is created by the call to `initscr()`
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

  erase();

  // Draw street border
  attron(COLOR_PAIR(STREET_BORDER_PAIR));
  move(street_i.y, 0);
  hline('=', street_len);

  move(street_i.y, street_f.x);
  hline('=', street_len);

  move(street_f.y, 0);
  hline('=', street_len);

  move(street_f.y, street_f.x);
  hline('=', street_len);

  move(bridge_i.y, bridge_i.x);
  hline('=', bridge_len);

  move(bridge_f.y, bridge_i.x);
  hline('=', bridge_len);

  x = street_i.x;
  y = street_i.y;
  while(x < bridge_i.x && y < bridge_i.y) {
    mvprintw(y, x, " # ");
    x++;
    y++;
  }

  x = bridge_f.x;
  y = bridge_i.y;
  while(x < street_f.x && y > street_i.y) {
    mvprintw(y, x, " # ");
    x++;
    y--;
  }

  x = bridge_f.x;
  y = bridge_f.y;
  while(x < street_f.x && y < street_f.y) {
    mvprintw(y, x, " # ");
    x++;
    y++;
  }

  x = street_i.x;
  y = street_f.y;
  while(x < bridge_i.x && y > bridge_f.y) {
    mvprintw(y, x, " # ");
    x++;
    y--;
  }
  attroff(COLOR_PAIR(STREET_BORDER_PAIR));

  // Draw street signs
  for (x = 0; x < street_len; x+=street_line_len) {
    if ((x % 2) == 0) continue;
    move(street_i.y + street_height, x);
    hline('-', street_line_len);
  }

  for (x = street_f.x;
       x < street_f.x + street_len;
       x+=street_line_len) {
    if ((x % 2) == 0) continue;
    move(street_i.y + street_height, x);
    hline('-', street_line_len);
  }

  // Dibujar carros aquí

  refresh();

  usleep(DELAY);

  }

  endwin();
}

void draw_car(int px, int direction) {
  int y;
  if (direction == 1) {
    y = bridge_i.y;
    if (px >= street_i.x && px <= street_f.x) y = bridge_i.y + street_height / 2;
    attron(COLOR_PAIR(CAR_EAST_PAIR));
    mvprintw(y-1,   px, " 0 o ");
    mvprintw(y+1-1, px, "= X|>");
    mvprintw(y+2-1, px, " 0 o ");
    attroff(COLOR_PAIR(CAR_EAST_PAIR));
  } else {
    y = bridge_f.y;
    if (px <= bridge_f.x && px >= bridge_i.x) y = bridge_i.y + street_height / 2;
    attron(COLOR_PAIR(CAR_WEAST_PAIR));
    mvprintw(y-1,   px, " o 0 ");
    mvprintw(y+1-1, px, "<|X =");
    mvprintw(y+2-1, px, " o 0 ");
    attroff(COLOR_PAIR(CAR_WEAST_PAIR));
  }
}
