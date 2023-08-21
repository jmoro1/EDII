#include <stdio.h>

/*
5. Codificar un procedimiento que intercambie dos enteros, por medio de la utilización de
punteros. Verificarlo mediante el llamado del mismo desde un código externo con impresión del
resultado (valores antes y después del intercambio).

*/


void swap(int *a, int *b){
int aux;
aux = *a;
*a = *b;
*b = aux;

}


int main(){
int x = 1, y = 2;
printf("Valores originales:\tx = %d, y = %d\n", x, y);
swap(&x, &y);
printf("Valores nuevos:\t\tx = %d, y = %d\n", x, y);

return 0;
}

