#include <stdio.h>

/*
struct pru_struct {
char id1;
char id2;
char id3[10];
char *nombre;
char *domicilio;
int edad;
int varios;
};
main(){
int i;
int tmp;
struct pru_struct empleados = {
'B',
'C',
"Sensible",
"Pedro",
"Av. Carlos Calvo 1234",
23,
68,
};
showinfo(&empleados);
};
*/


///////////SALIDA///////////////
/*
Valores iniciales de la estructura
id1: B
id2: C
id3: Sensible
Nombre: Pedro
Direccion: Av. Carlos Calvo 1234
Edad: 23
Varios: 68
Direccion de la estructura: 0x0022FEF4
Direccion del miembro id1: 0x0022FEF4 (offset: 0 bytes)  //DIRECCION BASE DE LA ESTRUCTURA, O DIRECCION DE LA PRIMER POSICION DE LA STRUCT
Direccion del miembro id2: 0x0022FEF5 (offset: 1 bytes) // DIRECCION DEL ID2, COMO ES UN CHAR, TIENE UN OFFSET DE 1 BYTE (OSEA,UN CHAR)
Direccion del miembro id3: 0x0022FEF6 (offset: 2 bytes) //DIRECCION DEL ID3, TAMBIEN TIENE UN OFFSET DE 1 BYTE RESPECTO DE LA DIRECCION ANTERIOR, TAMAÑO DEL ID2
Direccion del miembro nombre: 0x0022FF00 (offset: 12 bytes) //DIRECCION DEL NOMBRE, TIENE UN OFFSET DE 10 BYTES RESPECTO DEL ANTERIOR QUE VIENE DEL VECTOR DE CHARS DE 10 POSICIONES DE ID3
Direccion del miembro domicilio: 0x0022FF04 (offset: 16 bytes) // EL NOMBRE ES UN PUNTERO A CHAR, ASIQUE TIENE EL TAMAÑO DE UNA PALABRA, OSEA 4 BYTES
Direccion del miembro edad: 0x0022FF08 (offset: 20 bytes) // IGUAL QUE EL ANTERIOR, PUNTERO A ALGO OCUPA 4 BYTES DE MEMORIA
Direccion del miembro varios: 0x0022FF0C (offset: 24 bytes) // OFFSET DE 4 BYTES, TAMAÑO DE UN INT
Dirección de la primera posición de memoria después de la estructura: 0x0022FF10 //LA DIFERENCIA ENTRE ESTA DIRECCION Y LA ANTERIOR SON 4 BYTES, TAMAÑO DE UN INT QUE CORRESPONDE A LA VARIABLE varios
*/


struct pru_struct {
char id1;
char id2;
char id3[10];
char *nombre;
char *domicilio;
int edad;
int varios;
};


void showinfo(pru_struct *empleado){

size_t offset=0; //VARIABLE PARA ALMACENAR LOS TAMAÑOS DE LOS TIPOS DE MEMORIA EN BYTES

printf("Valores iniciales de la estructura",*empleado);  //IMPRIMO LOS VALORES DE LOS ELEMENTOS DE LA ESTRUCTURA
printf("\nid1:%c",empleado->id1);
printf("\nid2:%c",empleado->id2);
printf("\nid3:%c",empleado->id3);
printf("\nnombre:%c",empleado->nombre);
printf("\ndomicilio:%c",empleado->domicilio);
printf("\nedad:%d",empleado->edad);
printf("\nvarios:%d",empleado->varios);

printf("\nDireccion de la estructura:%p",(void *)&empleado); 
printf("\nDireccion de id1:%p",(void *)&empleado->id1);//IMPRIMO LA DIRECCION DE LOS ELEMENTOS DE LA ESTRUCTURA
printf("(offset:%zu bytes)",offset); 
printf("\nDireccion de id2:%p",(void *)&empleado->id2);
printf("(offset:%zu bytes)",offset += sizeof(char)); // GUARDO EL TAMAÑO DEL ELEMENTO EN BYTES EN LA VARIABLE OFFSET Y LA MUESTRO EN PANTALLA
printf("\nDireccion de id3:%p",(void *)&empleado->id3);
printf("(offset:%zu bytes)",offset += sizeof(char));
printf("\nDireccion de nombre:%p",(void *)&empleado->nombre);
printf("(offset:%zu bytes)",offset += sizeof(char)*10);
printf("\nDireccion de domicilio:%p",(void *)&empleado->domicilio);
printf("(offset:%zu bytes)",offset += sizeof(char));    //ACA LA CUENTA FALLA PORQUE NO ESTA CALCULANDO CORRECTAMENTE EL TAMAÑO DEL CHAR (VER MAS ABAJO EN MAIN)
printf("\nDireccion de edad:%p",(void *)&empleado->edad);
printf("(offset:%zu bytes)",offset += sizeof(int));
printf("\nDireccion de varios:%p",(void *)&empleado->varios);
printf("(offset:%zu bytes)",offset += sizeof(int));

}


int main(){

int i;
int tmp;
struct pru_struct empleados = {
'B',
'C',
"Sensible",
"Pedro",  //EL COMPILADOR ME DA UN ERROR COMO SI NO PUDIERA ASIGNAR UNA STR A UN CHAR
"Av. Carlos Calvo 1234", //EL COMPILADOR ME DA UN ERROR COMO SI NO PUDIERA ASIGNAR UNA STR A UN CHAR
23,
68,
};
showinfo(&empleados);

return 0;
}

