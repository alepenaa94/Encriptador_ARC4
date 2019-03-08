#ifndef _SOCKET_H
#define _SOCKET_H

// Definicion de variables o estructuras
typedef struct socket{
	bool es_server;
	int skt;
	int fd;
}socket_t;

/* Se pasa por parametro el numero de puerto y también en forma de string
se pasa la direccion de lo que quiero conectarme luego, o si es server
se pasa directamente "0".
Devuelve un socket_t.*/
void socket_crear(socket_t* srv,const char* ip,char* puerto);

/* Se pasa un socket cliente para realizar la conexion con el servidor seteado
al crear el socket.
Devuelve 1 si hubo algun error sino 0 si esta todo bien.*/
int socket_conectar_a_srv(socket_t* socket);

/*Recibe un socket servidor que intenta realizar una escucha de un cliente que
quiere conectarse al servidor y ademas se setea la escucha luego de dicha 
conexion a 1, de forma que espera solo un cliente.
Devuelve 1 si hubo algun error sino 0 si esta todo bien.*/
int socket_enlazar_y_escuchar(socket_t* socket);

//Devuelvo un nuevo socket donde esta conectado con el cliente.
void socket_aceptar_clt(socket_t* cliente ,socket_t* socket);

/*Envia la cantidad size apuntada por texto, asegurando que se envie la 
cantidad que se le especifica y devuelve cero. En caso de error
se devuelve 1.*/
int socket_enviar(socket_t* socket,char* texto,size_t size);

/*Recibe y va guardando en el buff la cantidad especificada y devuelve
 cero. En caso de error se devuelve 1.*/
int socket_recibir(socket_t* socket,char* buff,size_t size);

//Destruye el socket.
void socket_destruir(socket_t* socket);


#endif
