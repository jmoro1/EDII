#include <stdio.h>

/*4. Indicar los valores de x e y (y sus direcciones de memoria) en cada sentencia del siguiente
fragmento de código (mencionar cómo se llega a la obtención de los mismos).

int x = 1, y = 2;
int *ptr;
ptr = &x;
y = *ptr;
*ptr = 0;

*/



int sum(int a, int b){
    int suma;
    suma = a + b;
    return suma;
}

int main(){

int sumi,sumj;
printf("Ingrese primer sumando:\n");
scanf("%i",&sumi);
printf("Ingrese segundo sumando:\n");
scanf("%i",&sumj);
printf("La suma es: %ld \n",sum(sumi,sumj));
return 0;    
}

