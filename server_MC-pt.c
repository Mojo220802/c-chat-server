//compile in gcc : 		gcc client.c -o client.exe -lws2_32
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <winsock.h>
#include <pthread.h>

SOCKET server_socket;
SOCKET client_socket[10];
char client_name[10][60];
char names[50];
char textbuffer[255];
char last_message[255];

char kicked[] = "!Du wurdest vom Server gekicked wegen SPAM!\n";
int clients = 0;
int max_clients = 10;
int name = 0;
int emptySlots[10] = {0,0,0,0,0,0,0,0,0,0};
int slot = 0;

char abschnitt[] = "---------------------------------------------------------------------";
char take[] = "---------------------------------Users online: ------------------------------------\n";
char commands[] = "--------------------------------- Befehle ------------------------------------\n -?		|| Befehle anzeigen\n -users		|| Liste der Namen aller User online\n @*Username*	|| Nachricht nur an *Username*\n ------------------------------------------------------------";

FD_SET set;
pthread_t newThreadIn[10], newThreadOut[10];

void printNames(int client) {
	char buffer[60];
	char out[255] = "";

	strcat(out, take);
	for(int i = 0; i < 10; i++) {
		if(emptySlots[i] == 1) {
			strcpy(buffer, "");
			strcpy(buffer, client_name[i]);
			strcat(out, buffer);
			strcat(out, "\n");
			//strcat(buffer, "\n");
			//printf("%s\n", buffer);
			//send(client_socket[client], buffer, sizeof buffer, 0);
			
		}
	}
	strcat(out, abschnitt);
	send(client_socket[client], out, sizeof out, 0);
	
}

void printCommands(int client) {
	send(client_socket[client], commands, sizeof commands, 0);
}

int getClientNumber() {
	return clients;
}


static void *listenToClient(void* val) {
	int client_number = slot;
	char textbuffer[255];
	char last_message[255];
	char output[315];
	
	if(name == 2) {
	recv(client_socket[client_number], client_name[client_number], sizeof (client_name[client_number]), 0);
	recv(client_socket[client_number], client_name[client_number], sizeof (client_name[client_number]), 0);
	printf("%s joined the Server\n", client_name[client_number]);
	}
	
	for(int quit = 0; quit == 0;strcpy(last_message, textbuffer)) {
		
		recv(client_socket[client_number], textbuffer, sizeof textbuffer, 0);
		
		if(strcmp(textbuffer, last_message) == 0) {
			//send(client_socket[client_number], kicked, sizeof kicked, 0);
			send(client_socket[client_number], "!", sizeof "!", 0);
			closesocket(client_socket[client_number]);
			emptySlots[client_number] = 0;
			quit = 1;
			clients--;
		} else {
			
			
			if(textbuffer[0] == '%') {
				//strcpy(client_name[client_number], "");
				char bufferName[60];
				for(int i = 0; textbuffer[i] != '\0' && i < 60; i++) {
					bufferName[i] = textbuffer[i+ 1];
					bufferName[i + 1] = '\0';
				}
				int t = 0;
				for(int i = 0; i < max_clients; i++) {
					if(emptySlots[i] == 1 && strcmp(bufferName, client_name[i]) == 0) {
						send(client_socket[client_number], "----------------------------------------------------------------\n Name already taken\n ----------------------------------------------------------------\n", sizeof "----------------------------------------------------------------\n Name already taken\n ----------------------------------------------------------------\n", 0);
						t = 1;
					}
				}
				
				if(t == 0) {
					strcpy(client_name[client_number], "");
					strcpy(client_name[client_number], bufferName);
				}
				
			} else if(strncmp(textbuffer, "-users", 6) == 0) {
				//send(client_socket[client_number], take, sizeof take, 0);
				printNames(client_number);
				//send(client_socket[client_number], abschnitt, sizeof abschnitt, 0);
				
			} else if(textbuffer[0] == '@') {
				char buffer[70];
				
				int d;
				for(d = 1; d < 70 || textbuffer[d] == ' ' || textbuffer[d] == '\0' || textbuffer[d] == '\n'; d++) {
					if(textbuffer[d] == '\n' || textbuffer[d] == ' ') {
						buffer[d] = '\0';
						break;
					}
					buffer[d - 1] = textbuffer[d];
					buffer[d] = '\0';
				}
				
				for(int i = 0; i < max_clients; i++) {
					if(strcmp(buffer, client_name[i]) == 0) {
						strcpy(output, "");
						strcat(output, client_name[client_number]);
						strcat(output, ": ");
						strcat(output, textbuffer);
						send(client_socket[i], output, sizeof output, 0);
						break;
					} else if(i == max_clients - 1) {
						send(client_socket[client_number], "---------------------------------------------------------\n User existiert nicht\n---------------------------------------------------------\n", sizeof "---------------------------------------------------------\n User existiert nicht\n---------------------------------------------------------\n", 0);
					}
				}
				//printf("%s\n", output);
			} else  if(strncmp(textbuffer, "-?", 2) == 0) {
				printCommands(client_number);
			} else if(strncmp(textbuffer, "-", 1) == 0) {
				
			} else { 
			
			
			
			
			for(int i = 0; i < getClientNumber(); i++) {
				//printf("%i", getClientNumber());
				//if(i != client_number) {
					strcpy(output, "");
					strcat(output, client_name[client_number]);
					strcat(output, ":  ");
					strcat(output, textbuffer);
					send(client_socket[i], output, sizeof(output), 0);
					
				//}
			}
			}
			printf("%s: %s", client_name[client_number], textbuffer);
		}
	}
}

int main(void) {
	
	int f = 1;
	if(f == 1) {
		strcpy(client_name[3], "John");
		emptySlots[3] = 1;
	}
	
	//----------------------------------------------------------------------------------
	WSADATA wsaData;
	
	if (WSAStartup(MAKEWORD(1,1), &wsaData) != 0) {
            fprintf(stderr, "WSAStartup failed.\n");
            exit(1);
    }
	//---------------------------------------------------------------------------------------
	
	
	//--neue Server socket mit vorbereitung---------------------------------------------------------------------------------
	server_socket = socket(AF_INET, SOCK_STREAM, 0);
	
	
	struct sockaddr_in server_address;
	server_address.sin_family = AF_INET;
    server_address.sin_port = htons(1852);
	server_address.sin_addr.s_addr = inet_addr("192.168.2.107");
	//------------------------------------------------------------------------------------------
	
	
	//---verbindung zur Server socket-----------------------------------------------------------------------------------
	int test = bind(server_socket, (SOCKADDR *) &server_address, sizeof(server_address));
	
	if(test < 0) {
		printf("Server socket bind failed!\n");
		return 0;
	}
	
	//---------------------------------------------------------------------------------------
	
	
	char server_accept[] = "Mit Server verbunden!\n";
	
	int test2 = listen(server_socket, 10);
	
	if(test2 < 0) {
		printf("Problem beim server listen!\n");
		return 0;
	}
	
	
	printf("Warten auf clients...\n");
	
	//------------------------------------------------------------------------
	
	
	//strcpy(textbuffer, "test");
	
	for(;;) {
		FD_ZERO(&set);
		FD_SET(server_socket, &set);
		

		for(int i = 0; i < clients; i++) {
			FD_SET(client_socket[i],&set);
		}
		
		int slct = select(0, &set, NULL, NULL, NULL);
		
		if(FD_ISSET(server_socket, &set)) {
			
			for(slot = 0; slot < 10; slot++) {
				if(emptySlots[slot] == 0) {
					client_socket[slot] = accept(server_socket, NULL, NULL);
					emptySlots[slot] = 1;
					
					break;
				}
			}
			
			
			send(client_socket[slot], server_accept, sizeof(server_accept), 0);
			
			if(name == 1) {
				recv(client_socket[slot], client_name[slot], sizeof (client_name[slot]), 0);
				printf("%s joined the server\n", client_name[slot]);
			}
			//recv(client_socket[slot], client_name[slot], sizeof client_name[slot], 0);
			//printf("%s joined the server\n", client_name[slot]);
			clients++;
			printf("Client verbunden!\n");
			//----------------------------------------------------------------------------------------------------------------------
			
			
			int fm;
			fm = pthread_create( &newThreadIn[slot], NULL, &listenToClient, NULL);
			
			if(fm != 0) {
				printf("Fehler beim threaden!\n");
			}
			//FD_CLR(server_socket, &set);
		
			if(client_name[clients] == NULL && name == 1) {
				strcpy(client_name[clients], "unknownuser");
				printf("scheise");
			}
			
		}
	}
	
	printf("closing");
	
	closesocket(server_socket);

	return 0;
}