#define _POSIX_C_SOURCE 200112L
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include "socket.h"
#include <ctype.h>
#include "encriptador.h"
#include <stdlib.h>
#define TAM_BUFF_SERVER 50
#define TAM_BUFF_CLIENT 64
#define RET_ERRO 1
#define RET_OK 0
#define PARM_CLT 6
#define PARM_SRV 4
#define CERO 0
#define COMPORTAMIENTO 1
#define PARM_INVAL 1
#define PUERTO_SRV 2
#define CLAVE_SRV 3
#define PUERTO_CLT 3
#define IP_CLT 2
#define CLAVE_CLT 4
#define CLT_FILE 5

//Chequea si el str pasado es todo digito.
bool es_digito(char* str){
	for (int i = CERO; i < strlen(str); i++){
		if(isdigit(str[i])==CERO){
			return false;
		}
	}
	return true;
}

void imprimir_hex(char* texto,size_t size, FILE* stream,bool mayus){
	for (int i = CERO; i < size; i++){
		if(mayus){
			fprintf(stream,"%02X",(unsigned char)texto[i]);
			continue;	
		}
		fprintf(stream,"%02x",(unsigned char)texto[i]);
	}
}

// el tp se comporta como servidor con esta funcion
int servidor(char* argv[]){
	if(es_digito(argv[PUERTO_SRV])==false){
		fprintf(stderr, "Parametros incorrectos \n");
		return RET_ERRO;
	}//chequeo que el puerto sea digito entero.
	char* puerto = argv[PUERTO_SRV]; 
	char* ip = "0"; // por ser servidor.
	char* clave = argv[CLAVE_SRV]; // clave para descifrar
	socket_t srv;
	socket_t clt_cnx;
	socket_crear(&srv,ip,puerto);
	socket_aceptar_clt(&clt_cnx,&srv);
	char buff[TAM_BUFF_SERVER];
	int a=1;
	encriptador_t encrip;
	encriptador_crear(&encrip,clave);
	FILE* file_out;
	file_out = fopen("out","wb");
	if(!file_out){
		return RET_ERRO;
	}
	char destino[TAM_BUFF_SERVER];
	for (int i = CERO; i < TAM_BUFF_SERVER; i++){
		buff[i]='\0';
	}
	while(a!=CERO){
		a = socket_recibir(&clt_cnx,buff,TAM_BUFF_SERVER);
		if(a!=CERO){
			encriptador_trad(&encrip,buff,a,destino);
			imprimir_hex(destino,a,stdout,false);
			fwrite(destino, sizeof(char) ,a, file_out);
		}
	}
	char* keyS = encriptador_obt_ks(&encrip);
	int largo_keyS = encriptador_obt_ks_tam(&encrip);
	imprimir_hex(keyS,largo_keyS,stderr,true);		
	fclose(file_out);
	encriptador_destruir(&encrip);
	return RET_OK;
}



// el tp se comporta como cliente con esta funcion
int cliente(char* argv[]){
	if(es_digito(argv[PUERTO_CLT])==false){
		fprintf(stderr, "Parametros incorrectos \n");
		return RET_ERRO;
	}//chequeo que el puerto sea todo digito lo pasado.
	char* puerto = argv[PUERTO_CLT];
	char* ip = argv[IP_CLT]; // a lo que quiero conectarme
	char* clave = argv[CLAVE_CLT]; //clave para cifrar
	char* file_nomb = argv[CLT_FILE];
	socket_t clt;
	socket_crear(&clt,ip,puerto);
	FILE* file_in;
	file_in = fopen(file_nomb,"r");
	if(!file_in){
		fprintf(stderr, "Parametros incorrectos \n");
		return RET_ERRO;
	}
	encriptador_t encrip;
	encriptador_crear(&encrip,clave);
	int n_read;
	char buff[TAM_BUFF_CLIENT];
	char destino[TAM_BUFF_CLIENT];
	while(feof(file_in)==CERO){
		n_read = fread(buff,sizeof(char),TAM_BUFF_CLIENT,file_in);
		encriptador_trad(&encrip,buff,n_read,destino);
		imprimir_hex(destino,n_read,stdout,false);
		int k = socket_enviar(&clt,destino,n_read); 
		if(k==1){
			fclose(file_in);
			return RET_ERRO;
		}
	}
	char* keyS = encriptador_obt_ks(&encrip);
	int largo_keyS = encriptador_obt_ks_tam(&encrip);
	imprimir_hex(keyS,largo_keyS,stderr,true);	
	fclose(file_in);
	encriptador_destruir(&encrip);
	return RET_OK;
}


int	main(int argc, char *argv[]){
	if(argc == PARM_INVAL){
		fprintf(stderr,"Parametros incorrectos \n");
		return RET_ERRO;
	}
	if(strcmp(argv[COMPORTAMIENTO],"client")==CERO && argc==PARM_CLT){
		return cliente(argv);
	} else if (strcmp(argv[COMPORTAMIENTO],"server")==CERO && argc==PARM_SRV){
		return servidor(argv);
	}else{
		fprintf(stderr,"Parametros incorrectos \n");
		return RET_ERRO;
	}
	return RET_OK;
}







