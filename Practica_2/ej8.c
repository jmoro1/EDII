#include <stdio.h>

/*
8. La siguiente función calcula el largo de una cadena de caracteres. Reformularla para que la
funcionalidad sea resuelta por medio del uso de un puntero auxiliar en lugar de la variable n.
int strlen(char *s){
int n;
for (n=0; *s != '\0'; s++)
n++;
return n;
}
*/



int strlen(char *s){
int n;
for (n=0; *s != '\0'; s++)
n++;
return n;
}

int strlen_p(char* s){
    char* p_aux = s;

    while(*p_aux != '\0'){  //MIENTRAS EL CHAR DEL STR SEA DISTINTO DEL CARACTER NULO, INCREMENTO EL PUNTERO AUXILIAR
        p_aux ++;
    }

    return p_aux - s; //RESTO LA DIRECCION DEL PUNTERO AUXILIAR MENOS EL INICIO DE LA STRING, ENTONCES TENGO LA CANTIDAD DE ELEMENTOS DE 1 BYTE CONTADOS, OSEA CADA CHAR DEL STR
}

int main(){



return 0;
}

