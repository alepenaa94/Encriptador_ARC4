#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "encriptador.h"
#define ARRAYLEN 256
#define CERO 0

// Definicion de funciones auxiliares
void swapp(unsigned char* arreglo, int pos_a, int pos_b){
	 //Realiza el swap (intercambio) entre la pos_a y pos_b 
	//del puntero del arreglo pasado. 
	unsigned char temp = arreglo[pos_a];
	arreglo[pos_a] = arreglo[pos_b];
	arreglo[pos_b] = temp;
}
void ksa(encriptador_t* encrip, char* clave, size_t clave_tam){
	//Genera el vector S a partir de la clave, con el cual 
	//se realizara el encriptamiento
	int i,j;
	for (i = CERO; i < ARRAYLEN; i++){
		encrip->S[i] = i;
	}
	j=CERO;
	for (int i = CERO; i < ARRAYLEN; ++i){
		j=((j+encrip->S[i]+clave[(i%clave_tam)])%ARRAYLEN);
		swapp(encrip->S,i,j);
	}
}
char prga(encriptador_t* encrip){
	(encrip->i) = ((encrip->i)+1)%ARRAYLEN;
	(encrip->j) = ((encrip->j)+encrip->S[(encrip->i)])%ARRAYLEN;
	swapp(encrip->S,(encrip->i),(encrip->j));
	return (encrip->S[(encrip->S[(encrip->j)]+encrip->S[(encrip->i)])%ARRAYLEN]);
}

//Definicion de funciones
void encriptador_crear(encriptador_t* encrip,char* clave){
	ksa(encrip,clave,strlen(clave));
	encrip->i=CERO;
	encrip->j=CERO;
	encrip->keyStream = NULL;
	encrip->keyStream_tam = CERO;
}
void encriptador_trad(encriptador_t* encp,char* txt,size_t txt_tam,char* dest){
	char aux;
	int tam_ant = encp->keyStream_tam;
	if(encp->keyStream==NULL){
		encp->keyStream = malloc(sizeof(char)*txt_tam);
		encp->keyStream_tam = txt_tam;
	}else{
		int nuevo_tam = tam_ant+txt_tam;
		encp->keyStream = realloc((encp->keyStream),nuevo_tam*sizeof(char));
		encp->keyStream_tam = nuevo_tam;
	}
	int a=tam_ant;
	for (int m = CERO; m < txt_tam; m++){
		aux = prga(encp);
		dest[m] = (txt[m]^aux);
		encp->keyStream[a]=aux;
		a++;
	}
}
void encriptador_destruir(encriptador_t* encrip){
	free(encrip->keyStream);
}
char* encriptador_obt_ks(encriptador_t* encrip){
	return encrip->keyStream;
}
int encriptador_obt_ks_tam(encriptador_t* encrip){
	return encrip->keyStream_tam;
}
