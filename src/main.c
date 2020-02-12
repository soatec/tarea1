#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
    int opt;
    int oeste = -1;
    int este  = -1;

    while ((opt = getopt(argc, argv, "e:o:")) != -1) {
        switch (opt) {
        case 'e':
          este = atoi(optarg);
          break;
        case 'o':
          oeste = atoi(optarg);
          break;
        default:
            fprintf(stderr, "Uso: %s -e #CarrosDelEste -o #CarrosDelOeste\n", argv[0]);
            exit(EXIT_FAILURE);
        }
    }

    if (este == -1) {
      fprintf(stderr, "-e #CarrosDelEste es un parámetro obligatorio\n");
      exit(EXIT_FAILURE);
    }

    if (oeste == -1) {
      fprintf(stderr, "-o #CarrosDelOeste es un parámetro obligatorio\n");
      exit(EXIT_FAILURE);
    }

    fprintf(stdout, "\n****************\n* SOA: TAREA 1 *\n****************\n\n");
    fprintf(stdout, "Cantidad de carros del este: %u\n", este);
    fprintf(stdout, "Cantidad de carros del oeste: %u\n", oeste);
}
