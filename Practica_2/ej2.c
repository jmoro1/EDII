#include <stdio.h>

/*2-Crear un vector de enteros de 10 posiciones, recorrerlo e imprimir por pantalla cada uno de sus valores. ¿Qué tamaño ocupa en memoria?*/

int main(){
int i;
int v[10] = {0,1,2,3,4,5,6,7,8,9};

for(i= 0;i<10;i++){
    printf("El nro es: %ld\n",v[i]);
}

printf("El vector ocupa: %ld bytes\n",sizeof(v));
return 0;    
}

