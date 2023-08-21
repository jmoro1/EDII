#include <stdio.h>

/*3. Crear una función que realice la suma de dos enteros. Utilizarla para imprimir por pantalla la suma de dos enteros predefinidos.*/

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

