#include <stdio.h>

char charType = 'a';
short shortType = 1;
int intType = 2;
long longType = 2;
float floatType = 2;
double doubleType = 3;

int main(){
    printf("Tamaño de tipo de dato char:%ld byte\n",sizeof(charType));
    printf("Tamaño de tipo de dato short:%ld byte\n",sizeof(shortType));
    printf("Tamaño de tipo de dato int:%ld byte\n",sizeof(intType));
    printf("Tamaño de tipo de dato long:%ld byte\n",sizeof(longType));
    printf("Tamaño de tipo de dato float:%ld byte\n",sizeof(floatType));
    printf("Tamaño de tipo de dato double: %ld byte\n",sizeof(doubleType));

return 0;    
}

