# Tarea Corta 1
Sincronización de threads

## Requisitos

Es necesario tener instalada la biblioteca libncurses5:

***sudo apt-get install libncurses5-dev***

## Uso
1. Ejecute la regla ***make***
2. Ejecute el programa ***build/tarea1 -e 10 -w 10 -x 5 -y 8***

En donde,
 
* ***e*** es la cantidad de carros que viajan de oeste a este.
 
* ***w*** es la cantidad de carros que viajan de este a oeste.
 
* ***x*** es la media para la distribución exponencial del tiempo de creación de carros que van de oeste a este.
 
* ***y*** es la media para la distribución exponencial del tiempo de creación de carros que van de este a oeste.

Esta tarea incluye una animación sencilla en la terminal que muestra la posición de los carros en la carretera. A continuación se muestra una imagen de esta interfaz:

![Alt text](docs/interfaz.png?raw=true "Title")

Esta tarea corta se puede encontrar en https://github.com/soatec/tarea1.