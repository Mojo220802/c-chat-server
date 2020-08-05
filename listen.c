#include <stdio.h>
#include <winsock.h>
#include <string.h>

SOCKET zwischenserver_socket;

char response[100];
char last_message[255] = "kick=that$&shit?)==";
int last = 0;

void connectWithServer(void) {
	
	//----------------------------------------------------------------------------------
	WSADATA wsaData;
	
	if (WSAStartup(MAKEWORD(1,1), &wsaData) != 0) {
            fprintf(stderr, "WSAStartup failed.\n");
            exit(1);
    }
	//---------------------------------------------------------------------------------------
	
	zwischenserver_socket = socket(AF_INET, SOCK_STREAM, 0);
	
	int code = 0;
	char buffer[25];
	strcpy(buffer, getenv("USERNAME"));
	
	for(int i = 0; i < strlen(buffer) && i < 15; i++) {
		code += (int)buffer[i] * (10^(i + 1));
	}
	
	struct sockaddr_in zwischenserver_address;
	zwischenserver_address.sin_family = AF_INET;
    zwischenserver_address.sin_port = htons(code);
	zwischenserver_address.sin_addr.s_addr = inet_addr("127.0.0.1");
	
	int connection_status = connect(zwischenserver_socket, (SOCKADDR *) &zwischenserver_address, sizeof(zwischenserver_address));
	
	if(connection_status < 0) {
		printf("Connection mit server_socket fehlgeschlagen!\n\n");
		wprintf(L"%ld", WSAGetLastError());
	}
	
	send(zwischenserver_socket, "Hey, listen!", sizeof("Hey, listen!"), 0);
	
	recv(zwischenserver_socket, response, sizeof response, 0);
	
	printf("%s\n", response);
}

int main(void) {
	connectWithServer();
	
	int quit = 0;
	
	
	while(quit == 0) {
		recv(zwischenserver_socket, response, sizeof response, 0);
		
		if(response[0] == '!') {
			
			printf("-----------------------------------------------------\n");
			printf("Server wurde geschlossen\n");
			printf("-----------------------------------------------------");
			quit = 1;
			
		} else {
		
		printf("%s", response);
		strcpy(last_message, response);
		strcpy(response, "\0");
	
		}
	}
	
	return 0;
}