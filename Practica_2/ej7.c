#include <stdio.h>

/*
7. Compilar el siguiente código y explicar lo obtenido en la corrida.
int main(void){
int a = 0x12345678;
short int b = 0xABCD;
char c = 'a';
int * ptr_a = &a;
short int * ptr_b = &b;
char * ptr_c = &c;
printf("\nValor de ptr_a:\t\t %p\n", ptr_a);
printf("Valor de ptr_a + 1:\t %p\n", ++ptr_a);
printf("\nValor de ptr_b:\t\t %p\n", ptr_b);
printf("Valor de ptr_b + 1:\t %p\n", ++ptr_b);
printf("\nValor de ptr_c:\t\t %p\n", ptr_c);
printf("Valor de ptr_c + 1:\t %p\n", ++ptr_c);
}


SALIDA:

Valor de ptr_a:          00000000005ffe84 (DIRECCION DEL PUNTERO A ENTERO)
Valor de ptr_a + 1:      00000000005ffe88 (DIRECCION DEL PUNTERO AL SIGUIENTE ENTERO OSEA +4 BYTES)

Valor de ptr_b:          00000000005ffe82 (DIRECCION DEL PUNTERO A short int)
Valor de ptr_b + 1:      00000000005ffe84 (DIRECCION DEL PUNTERO AL SIGUIENTE short int OSEA +2 BYTES)

Valor de ptr_c:          00000000005ffe81 (DIRECCION DEL PUNTERO A char)
Valor de ptr_c + 1:      00000000005ffe82 (DIRECCION DEL PUNTERO AL SIGUIENTE char OSEA +1 BYTE)
*/




int main(){

int a = 0x12345678;
short int b = 0xABCD;
char c = 'a';
int * ptr_a = &a;
short int * ptr_b = &b;
char * ptr_c = &c;
printf("\nValor de ptr_a:\t\t %p\n", ptr_a);
printf("Valor de ptr_a + 1:\t %p\n", ++ptr_a);
printf("\nValor de ptr_b:\t\t %p\n", ptr_b);
printf("Valor de ptr_b + 1:\t %p\n", ++ptr_b);
printf("\nValor de ptr_c:\t\t %p\n", ptr_c);
printf("Valor de ptr_c + 1:\t %p\n", ++ptr_c);

return 0;
}

