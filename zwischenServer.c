#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock.h>
#include <pthread.h>

SOCKET server_socket;
SOCKET tell_socket;
SOCKET listen_socket;
SOCKET zwischenserver_socket;
SOCKET buffer_socket;
int connection_status_listen = 0;
int connection_status_tell = 0;
pthread_t server_tell, server_listen;


char blockedNames[10][70];
int emptyBlock[10] = {0,0,0,0,0,0,0,0,0,0};
int blocked = 0;
int b = 0;

char ip[] = "192.168.2.107";
char response[255];
char name[60] = "John";
int quit = 0;
FD_SET set;

static void *listenServer(void* val) {
	char response_listen[255];
	char last_message[255];
	
	while(quit == 0) {
		recv(server_socket, response_listen, sizeof response_listen, 0);
		
		//send(listen_socket, response_listen, sizeof(response_listen), 0);
		//strcpy(response_listen, "\0");
		
		if(response_listen[0] == '!') {
			
			printf("-----------------------------------------------------\n");
			printf("Server wurde geschlossen\n");
			printf("-----------------------------------------------------");
			quit = 1;
			
		} else if(strncmp(response_listen, "\n", 1) == 0 || strncmp(response_listen, "\0", 1) == 0) {
			
		} else {
			b = 0;
			for(int i = 0; i < 10; i++) {
				if(emptyBlock[i] == 1) {
					if(strncmp(blockedNames[i], response_listen, strlen(blockedNames[i]) - 1) == 0) {
						b = 1;
					}/* else {
						char mo[70];
						strncpy(mo, response_listen, strlen(blockedNames[i]));
						printf("|%s|: |%s|\n", blockedNames[i], mo);
					}*/
				}
			}
		
			if(b == 0) {
				printf("%s\n", response_listen);
				strcpy(last_message, response_listen);
				strcpy(response_listen, "\0");
			} else {
				b = 0;
			}
		}
	}
}

static void *tellServer(void* val) { 
	char response_tell[255];
	for(;;) {
	recv(tell_socket, response_tell, sizeof response_tell, 0);
	
	send(server_socket, response_tell, sizeof(response_tell), 0);
	
	if(response_tell[0] == '%') {
		strcpy(name, "");
		for(int i = 0; response_tell[i] != '\0' && i < 60; i++) {
			name[i - 1] = response_tell[i];
		}
	} else {
		if(strncmp(response_tell, "-block", 6) == 0 && blocked < 10) {
			for(int i = 0; i < 10; i++) {
				if(emptyBlock[i] == 0) {
					for(int v = 0; v < 70; v++) {
						if(response_tell[v + 7] == '\0' || response_tell[v + 7] == '\n' /*|| response_tell[v + 6] == " "*/) {
							blockedNames[i][v] = '\0';
							break;
						}
						blockedNames[i][v] = response_tell[v + 7];
					}
					printf(" Blockname:%s\n", blockedNames[i]);
					emptyBlock[i] = 1;
					blocked++;
					break;
				}
			}
		} else if(strncmp(response_tell, "-unblock", 8) == 0) {
			char buffer_block[70];
			
			for(int v = 0; v < 70; v++) {
				if(response_tell[v + 9] == '\0' || response_tell[v + 9] == '\n' /*|| response_tell[v + 6] == " "*/) {
					buffer_block[v] = '\0';
					break;
				}
				buffer_block[v] = response_tell[v + 9];
			}
			printf("Unblock: |%s|\n", buffer_block);
			
			for(int i = 0; i < 10; i++) {
				if(emptyBlock[i] == 1 && strncmp(blockedNames[i], buffer_block, strlen(blockedNames[i]) - 1) == 0) {
					strcpy(blockedNames[i], "\0");
					//printf("|%s| : |%s|\n", buffer_block, blockedNames);
					emptyBlock[i] = 0;
					blocked--;
					break;
				}
			}
		}
		send(listen_socket, response_tell, sizeof(response_tell), 0);
	}
	}
}

void connectToServer(void) {
	
}

void socketSetup(void) {
	//----------------------------------------------------------------------------------
	WSADATA wsaData;
	
	if (WSAStartup(MAKEWORD(1,1), &wsaData) != 0) {
            fprintf(stderr, "WSAStartup failed.\n");
            exit(1);
    }
	//---------------------------------------------------------------------------------------
	
	int code = 0;
	char buffer[25];
	strcpy(buffer, getenv("USERNAME"));
	
	for(int i = 0; i < strlen(buffer) && i < 15; i++) {
		code += (int)buffer[i] * (10^(i + 1));
	}
	
	server_socket = socket(AF_INET, SOCK_STREAM, 0);
	tell_socket = socket(AF_INET, SOCK_STREAM, 0);
	listen_socket = socket(AF_INET, SOCK_STREAM, 0);
	zwischenserver_socket = socket(AF_INET, SOCK_STREAM, 0);
	
	struct sockaddr_in server_address;
	server_address.sin_family = AF_INET;
    server_address.sin_port = htons(1852);
	server_address.sin_addr.s_addr = inet_addr(ip);
	
	struct sockaddr_in zwischenserver_address;
	zwischenserver_address.sin_family = AF_INET;
    zwischenserver_address.sin_port = htons(code);
	zwischenserver_address.sin_addr.s_addr = inet_addr("127.0.0.1");
	
	
	int test = bind(zwischenserver_socket, (SOCKADDR *) &zwischenserver_address, sizeof(zwischenserver_address));
	
	if(test < 0) {
		printf("Zwischenserver socket bind failed!\n");
		return;
	}
	
	
	int connection_status = connect(server_socket, (SOCKADDR *) &server_address, sizeof(server_address));
	
	if(connection_status < 0) {
		printf("Zwischenserver konnte nicht mit Server verbinden!\n");
		return;
	}
	
	recv(server_socket, response, sizeof response, 0);
	printf("%s\n", response);
	
	//send(server_socket, name, sizeof name, 0);
}



int main(void) {
	
	socketSetup();
	
	int test2 = listen(zwischenserver_socket, 2);
	
	if(test2 < 0) {
		printf("Problem beim server listen!\n");
		return 0;
	}
	
	for(;;) {
		FD_ZERO(&set);
		
		FD_SET(zwischenserver_socket, &set);
		
		
		select(0, &set, NULL, NULL, NULL);
		
		if(FD_ISSET(zwischenserver_socket, &set)) {
			
			buffer_socket = accept(zwischenserver_socket, NULL, NULL);
			char category[100];
			recv(buffer_socket, category, sizeof category, 0);
			char server_accept[] = "Mit Zwischenserver verbunden!";
			
			int fm;
				fm = pthread_create( &server_listen, NULL, &listenServer, NULL);
			
				if(fm != 0) {
					printf("Fehler beim threaden!\n");
				}
			
			/*if(strcmp(category, "Hey, listen!") == 0 && connection_status_listen == 0) {
				listen_socket = buffer_socket;
				connection_status_listen = 1;
				send(listen_socket, server_accept, sizeof(server_accept), 0);
				
				
		
			}*/
			
			if(strcmp(category, "Tell him!") == 0 && connection_status_tell == 0) {
				tell_socket = buffer_socket;
				connection_status_tell = 1;
				send(tell_socket, server_accept, sizeof(server_accept), 0);
				
				int fm;
				fm = pthread_create( &server_tell, NULL, &tellServer, NULL);
			
				if(fm != 0) {
					printf("Fehler beim threaden!\n");
				}
			}
			
			
			if(quit == 1) {
				return 0;
			}
		}
	}
	
	return 0;
}