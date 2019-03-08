#define _POSIX_C_SOURCE 200112L
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <ctype.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <netinet/ip.h> 
#include "socket.h"
#define RET_ERRO 1
#define RET_OK 0
#define CERO 0
#define INVAL -1

void socket_crear(socket_t* skt_n,const char* ip, char* puerto){
	skt_n->es_server=false;
	skt_n->skt = CERO;
	skt_n->fd = CERO;
	struct addrinfo hints;
	struct addrinfo *temp, *ptr;	

	memset(&hints,CERO,sizeof(struct addrinfo));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	if(strcmp(ip,"0")==CERO){
		//es servidor
		skt_n->es_server = true;
		hints.ai_flags = AI_PASSIVE;
	}else{
		//es cliente
		hints.ai_flags = CERO;
	}
	int skt;
	skt_n->fd = getaddrinfo(ip,puerto,&hints,&ptr);
	if (skt_n->fd!= CERO){
		printf("Error en getaddrinfo: %s\n",gai_strerror(skt_n->fd));
		return;
	}
	if (skt_n->es_server){
		skt=socket(ptr->ai_family,ptr->ai_socktype,ptr->ai_protocol);
		if(skt==INVAL){
			printf("Error: %s\n", strerror(errno));
			freeaddrinfo(ptr);
			return;
		}
		skt_n->fd = bind(skt,ptr->ai_addr,ptr->ai_addrlen);
		if(skt_n->fd==INVAL){
			printf("Error: %s\n", strerror(errno));
			close(skt);
			freeaddrinfo(ptr);
			return;
		}
		freeaddrinfo(ptr);
		skt_n->fd = listen(skt,1);
		if(skt_n->fd==INVAL){
			printf("Error: %s\n", strerror(errno));
			close(skt);
			return;
		}
		skt_n->skt=skt;
	}else{//si es cliente
		bool skt_cnect = false;
		for(temp=ptr; (temp!=NULL)&&(skt_cnect==false); temp=temp->ai_next){
			skt=socket(temp->ai_family,temp->ai_socktype,temp->ai_protocol);
			if(skt == INVAL){
				printf("Error: %s\n", strerror(errno));
			}else{
				skt_n->fd =connect(skt,temp->ai_addr,temp->ai_addrlen);
				if(skt_n->fd == -1){
					printf("Error: %s\n", strerror(errno));
					close(skt);
				}
				skt_cnect = (skt_n->fd != -1);
			}
		}
		freeaddrinfo(ptr);
		if (skt_cnect==false){ 	//significa que no hubo direcciones validas..
			return;
		}
		skt_n->skt=skt;
	}
	return;
}

void socket_aceptar_clt(socket_t* nuevo_skt,socket_t* conexion){
	if(conexion->es_server==false){
		return;
	}
	int skt_nuevo = CERO;
	skt_nuevo = accept(conexion->skt,NULL,NULL);
	if(skt_nuevo==INVAL){
		printf("Error: %s\n", strerror(errno));
		return;
	}
	nuevo_skt->skt = skt_nuevo;
}

int socket_enviar(socket_t* skt_cx,char* texto,size_t size){
	int bytes_enviar = size;
	int b_env = CERO;
	int bytes_exito;
	bool socket_error = false;
	bool otro_socket_cerrado = false;
	while(b_env<bytes_enviar&&socket_error==false&&otro_socket_cerrado==false){
		bytes_exito=send(skt_cx->skt,texto,bytes_enviar-b_env,MSG_NOSIGNAL);
		if(bytes_exito==CERO){
			otro_socket_cerrado=true;
		}else if (bytes_exito<CERO){
			printf("Error: %s\n",strerror(errno) );
			socket_error=true;
		}else{
			b_env += bytes_exito;
			texto += bytes_exito;
		}
	}
	if(socket_error||otro_socket_cerrado){
		return RET_ERRO;
	}
	return RET_OK;
}

int socket_recibir(socket_t* skt_cx, char* buff,size_t size){
	int bytes_recb = size;
	int recibidos = CERO;
	int recib_ant = CERO;
	int bytes_exitosos;
	bool socket_error = false;
	bool otro_socket_cerr = false;
	while(recibidos<bytes_recb&&socket_error==false&&otro_socket_cerr==false){
		bytes_exitosos=recv(skt_cx->skt,buff,bytes_recb-recibidos,MSG_NOSIGNAL);
		if(bytes_exitosos==CERO){
			if(recib_ant!=CERO){
				return recib_ant;
			}
			otro_socket_cerr=true;
		}else if (bytes_exitosos<CERO){
			printf("Error: %s\n",strerror(errno) );
			socket_error=true;
		}else{
			recib_ant = bytes_exitosos;
			recibidos += bytes_exitosos;
			buff += bytes_exitosos;
		}
	}
	if(socket_error||otro_socket_cerr){
		return CERO; 
	}
	return recibidos;
}

void socket_destruir(socket_t* conexion){
	shutdown(conexion->skt,SHUT_RDWR);
	close(conexion->skt);
}
