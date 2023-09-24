#include <stdio.h>

/*
14. Dado el siguiente código, ¿qué se mostraría por pantalla al correrlo?
#include <stdio.h>
union aux {
int a;
char b;
};
int main(void){
union aux var;
var.a = 77;
printf("a: %d\n", var.a);
printf("b: %c\n", var.b);
}

//////////////////RESPUESTA////////////////

EL VALOR DE a SERA DE 77 COMO FUE FIJADO, EN EL CASO DE b MOSTRARÁ EL 77 PERO DE LA TABLA ASCII QUE ES LA "M"
*/


union aux {
int a;
char b;
};

int main(){

union aux var;

var.a = 77;

printf("a: %d\n", var.a);
printf("b: %c\n", var.b);


return 0;
}

