#include <iostream>
#include <string>
#include <map>
#include <cstring>
#include <netinet/in.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <errno.h>
#include <vector>
#include "utilities.h"

#define PORT 23010

using namespace std;
typedef map<string,string> servermap;

int main(){
	servermap smap = getMap("server3.txt");

	int sockfd, newsockfd;
	struct sockaddr_in server_addr, client_addr;
	struct hostent *server;
	socklen_t client_len, server_len;
	int sockoptval = 1, buffer_len;
	char buffer[512];
	string searchKey,value;

	/** create TCP socket*/
	sockfd = socket(AF_INET,SOCK_STREAM,0);
	if(sockfd < 0){
		cout<<"\n Error creating TCP socket"<<endl;
		exit(1);
	}

	/* allow immediate reuse of the port */ 
	setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &sockoptval, sizeof(int));

	/** bind the TCP socket*/
	memset((char *)&server_addr, 0, sizeof(server_addr));
	server = gethostbyname("nunki.usc.edu");
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(PORT);
	memcpy((void *)&server_addr.sin_addr, server->h_addr_list[0], server->h_length);
	if(bind(sockfd,(struct sockaddr *)&server_addr, sizeof(server_addr)) < 0){
		cout<<"\n Error binding the TCP socket"<<endl;
		exit(1);
	}

	/** get more socket details*/
	server_len = sizeof(server_addr);
	if (getsockname(sockfd, (struct sockaddr *)&server_addr, &server_len) == -1){
		cout<<"\n Error to get socket name"<<endl;
		exit(1);
	}
	/**Boot up message which shows port no. and IP address of server 3*/
	cout<<"\n The TCP port number is  "<< ntohs(server_addr.sin_port);
	cout<<" and the IP address is "<< inet_ntoa(server_addr.sin_addr)<<"\n"<<endl;

	if(listen(sockfd,5) < 0){
		cout<<"\n Error is listening"<<endl;
		exit(1);
	}

	client_len = sizeof(client_addr);
	for(;;){
		/** receive the request from server 2*/
		while ((newsockfd = accept(sockfd, (struct sockaddr *)&client_addr, &client_len)) < 0) {
			/* we may break out of accept if the system call */
                        /* was interrupted. In this case, loop back and */
                        /* try again */
                        if ((errno != ECHILD) && (errno != ERESTART) && (errno != EINTR)) {
                                cout<<"\n accept failed"<<endl;
                                exit(1);
                        } 
                }
		
		buffer_len = read(newsockfd,buffer,512);
		if(buffer_len < 0){
			cout<<"\n Error receiving message from server2"<<endl;
			exit(1);			
		}
		buffer[buffer_len]='\0';
		searchKey = getWords(buffer).at(1);
		
		/** message after receiving the request from server 2*/
		cout<<"\n The Server 3 has received a request with key "<<searchKey<<" from the Server 2 with port number ";
		cout<<ntohs(client_addr.sin_port)<< " and the IP address is "<<inet_ntoa(client_addr.sin_addr)<<endl;

		/** retrieve value from the map and construct the reply message*/
		value = smap.find(searchKey)->second;
		strcpy(buffer,"POST ");
		strcat(buffer,value.c_str());
	
		/** send the reply to server 2*/
		buffer_len = write(newsockfd,buffer,strlen(buffer));
		if (buffer_len < 0){
			cout<<"\n ERROR writing to socket"<<endl;
			exit(1);
		}
		/** message after sending reply to server 2*/
		cout<<"\n The Server 3 sends the reply "<< buffer << " to Server 2 with port number "<<ntohs(client_addr.sin_port);
		cout<<" and IP address "<< inet_ntoa(client_addr.sin_addr)<<endl;
	}
	return 0;
}
