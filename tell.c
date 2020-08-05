#include <stdio.h>
#include <winsock.h>
#include <string.h>
#include <stdlib.h>

SOCKET zwischenserver_socket;

char response[100];
char name[50];
char name_out[60];
char output[255];
int code = 0;

void connectWithServer(void) {

	//----------------------------------------------------------------------------------
	WSADATA wsaData;
	
	if (WSAStartup(MAKEWORD(1,1), &wsaData) != 0) {
            fprintf(stderr, "WSAStartup failed.\n");
            exit(1);
    }
	//---------------------------------------------------------------------------------------
	
	zwischenserver_socket = socket(AF_INET, SOCK_STREAM, 0);
	
	char buffer[25];
	strcpy(buffer, getenv("USERNAME"));
	
	for(int i = 0; i < strlen(buffer) && i < 15; i++) {
		code += (int)buffer[i] * (10^(i + 1));
	}
	
	printf("Code : %i\n", code);
	
	struct sockaddr_in zwischenserver_address;
	zwischenserver_address.sin_family = AF_INET;
    zwischenserver_address.sin_port = htons(code);
	zwischenserver_address.sin_addr.s_addr = inet_addr("127.0.0.1");
	
	int connection_status = connect(zwischenserver_socket, (SOCKADDR *) &zwischenserver_address, sizeof(zwischenserver_address));
	
	if(connection_status < 0) {
		printf("Connection mit server_socket fehlgeschlagen!\n\n");
		wprintf(L"%ld", WSAGetLastError());
	}
	
	send(zwischenserver_socket, "Tell him!", sizeof("Tell him!"), 0);
	
	recv(zwischenserver_socket, response, sizeof response, 0);
	
	printf("%s\n", response);
}

int main(void) {
	
	connectWithServer();
	
	/*printf("Enter your name: ");
	fgets(name, 20, stdin);
	
	strcat(name_out, "%");
	

	for(int i = 0; i < 20; i++) {
		if(name[i] == '\n') {
			name[i] = '\0';
		}
	}
	strcat(name_out, name);*/
	strcat(name_out, "%");
	strcat(name_out, getenv("USERNAME"));
	send(zwischenserver_socket, name_out, sizeof name_out, 0);
	
	int quit = 0;
	
	char last_message[255];
	char textmessage[200];
	while(quit == 0) {
		fgets(textmessage, sizeof textmessage, stdin);
		
		if(textmessage[0] == '%') {
			for(int i = 0; textmessage[i] != '\0'; i++) {
				if(textmessage[i] == '\n') {
					textmessage[i] = '\0';
				}
			}
		}
		strcat(output, textmessage);
		send(zwischenserver_socket, output, sizeof output, 0);
		
		strcpy(output, "");
		
		
	}
	
	return 0;
}