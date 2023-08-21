#include <stdio.h>

/*
6.Se tiene la siguiente declaración:
int x[5];
int *ptr;
a) ¿Cómo haría para que el puntero ptr apunte a la primera posición del vector x?
b) ¿Recorrer el vector completo utilizando incrementos en el puntero ptr?
c) Realizar un printf del puntero ptr para cada incremento del punto b. Indicar por qué entre
valor y valor existe un salto en la secuencia.
d) Si en lugar de tener un vector de enteros (int) en el ejemplo utilizáramos un vector de chars, el
salto entre valores consecutivos del punto c cambiaría? Corroborarlo.

*/


// RESPUESTA
/*  a) haría ptr = &x[0]
    c) Hay un salto de 4 bytes que es el tamaño de un int
    d) Si, el salto debería ser de 1 byte que es el tamaño de un char */


int main(){

int i;
char x[5] = {0,1,2,3,4};
char *ptr;

ptr = &x[0];

for(i=0;i<5;i++){
    printf("Dirección y valor del array: %p , %c\n",ptr+i,*(ptr+i));    
}


return 0;
}

