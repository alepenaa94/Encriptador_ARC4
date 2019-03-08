#ifndef ENCRIPTADOR_H
#define ENCRIPTADOR_H
#include <stdlib.h>
#define ARRAYLEN 256


// Definicion de variables o estructuras
typedef struct encriptador{
	unsigned char S[ARRAYLEN];
	int i;
	int j;
	char* keyStream;
	int keyStream_tam;
}encriptador_t;


// Definicion de firmas de funciones

/* Crea la estructura de encriptador en memoria en funcion
a la clave pasada como parametro. Devuelve el puntero de 
encriptador o NULL si no pudo crearse. */
void encriptador_crear(encriptador_t* enc,char* clave);

/* Recibe por parametro un encriptador_t* , el texto a traducir, su tamanio 
y el destino en el cual se guardara dicha traduccion. */
void encriptador_trad(encriptador_t* enc,char* txt,size_t txt_tam ,char* dest);

/* Recibe por parametro el encriptador_t* del cual quiero liberar memoria */
void encriptador_destruir(encriptador_t* encrip);

/* Devuelve un "char*" el cual contiene el keyStream con la cual se 
realiza la encripcion */
char* encriptador_obt_ks(encriptador_t* encrip);

/* Devuelve la longitud del keyStream*/
int encriptador_obt_ks_tam(encriptador_t* encrip);


#endif // ENCRIPTADOR_H
