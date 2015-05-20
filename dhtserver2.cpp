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

#define PORT1 22010
#define PORT2 23010

using namespace std;
typedef map<string,string> servermap;

string getValueFromServer3(char buffer[]);

int main(){

	servermap smap = getMap("server2.txt");

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

	/** allow immediate reuse of the port 
	Used the code from https://www.cs.rutgers.edu/~pxk/417/notes/sockets/index.html */
	setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &sockoptval, sizeof(int));

	/** bind the TCP socket*/
	memset((char *)&server_addr, 0, sizeof(server_addr));
	server = gethostbyname("nunki.usc.edu");
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(PORT1);
	memcpy((void *)&server_addr.sin_addr, server->h_addr_list[0], server->h_length);
	if(bind(sockfd,(struct sockaddr *)&server_addr, sizeof(server_addr)) < 0){
		cout<<"\n Error binding the TCP socket"<<endl;
		exit(1);
	}

	/** get TCP socket details of server 2 */
	server_len = sizeof(server_addr);
	if (getsockname(sockfd, (struct sockaddr *)&server_addr, &server_len) == -1){
		cout<<"\n Error to get socket name"<<endl;
		exit(1);
	}
	/** Boot up message which shows port no. and IP address of server 2*/
	cout<<"\n The TCP port number is  "<< ntohs(server_addr.sin_port);
	cout<<" and the IP address is "<< inet_ntoa(server_addr.sin_addr)<<"\n"<<endl;

	if(listen(sockfd,5) < 0){
		cout<<"\n Error in listening"<<endl;
		exit(1);
	}

	client_len = sizeof(client_addr);
	for(;;){
		/** receive the request from server 1*/
		while ((newsockfd = accept(sockfd, (struct sockaddr *)&client_addr, &client_len)) < 0) {
			/* we may break out of accept if the system call */
                        /* was interrupted. In this case, loop back and */
                        /* try again */
						/** Used the code from https://www.cs.rutgers.edu/~pxk/417/notes/sockets/index.html */
                        if ((errno != ECHILD) && (errno != ERESTART) && (errno != EINTR)) {
                                cout<<"\n Accept failed"<<endl;
                                exit(1);
                        } 
                }
		
		buffer_len = read(newsockfd,buffer,512);
		if(buffer_len < 0){
			cout<<"\n Error receiving message from server1"<<endl;
			exit(1);			
		}
		buffer[buffer_len]='\0';
		searchKey = getWords(buffer).at(1);
		/** message after receiving the request from server 1*/
		cout<<"\n The Server 2 has received a request with key "<<searchKey<<" from the Server1 with port number ";
		cout<<ntohs(client_addr.sin_port)<< " and the IP address is "<<inet_ntoa(client_addr.sin_addr)<<"\n"<<endl;

		/**If the server 2 has entry in its table then reply to the server 1 immediately*/
		if(smap.find(searchKey)!=smap.end()){
			value = smap.find(searchKey)->second;
			strcpy(buffer,"POST ");
			strcat(buffer,value.c_str());
		}
		/** If server 2 doesn't have entry in its table request server 3 to get the value*/
		else{
			strcpy(buffer,getValueFromServer3(buffer).c_str());
			smap[searchKey] = getWords(buffer).at(1);
		}
		
		/** send reply to server 1*/
		buffer_len = write(newsockfd,buffer,strlen(buffer));
		if (buffer_len < 0){
			cout<<"\n ERROR writing to socket"<<endl;
			exit(1);
		}
		/** message after sending the reply to server 1*/
		cout<<"\n The Server 2 sends the reply "<< buffer << " to Server 1 with port number "<<ntohs(client_addr.sin_port);
		cout<<" and IP address "<< inet_ntoa(client_addr.sin_addr)<<endl;
	}
	return 0;
}

string getValueFromServer3(char buffer[]){
	int sockfd, buffer_len;
	struct hostent *server;
	struct sockaddr_in server_addr, my_addr;
	socklen_t my_len;
	string message;

	/**create client socket*/
	sockfd = socket(AF_INET,SOCK_STREAM,0);
	if(sockfd<0){
		cout<<"\n Error creating TCP socket"<<endl;
		exit(1);
	}

	/** get more details of host from the host name*/
	server = gethostbyname("nunki.usc.edu");
        if (!server) {
		cout<<"\n Could not obtain host address from name"<<endl;
		exit(1);
	}
	memset((char*)&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(PORT2);
	memcpy((void *)&server_addr.sin_addr, server->h_addr_list[0], server->h_length);

	/** connect to server */
	if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
		cout<<"\n Connect failed"<<endl;
		exit(1);
	}
	
	/** sending message to server 2*/
	buffer_len = write(sockfd,buffer,strlen(buffer));
	if (buffer_len < 0){
        	cout<<"\n ERROR sending message to server3"<<endl;
		exit(1);
	}

	/** get more socket details*/
	if (getsockname(sockfd, (struct sockaddr *)&my_addr, &my_len) == -1){
		cout<<"\n Error to get socket name"<<endl;
		exit(1);
	}
	/** message after sending request to server 3*/
	cout<<"\n The Server 2 sends request "<< buffer <<" to the Server 3"<<endl;
	cout<<"\n The TCP port number is  "<< ntohs(my_addr.sin_port);
	cout<<" and the IP address is "<< inet_ntoa(my_addr.sin_addr)<<endl;
	
	/** receive the reply from server 3*/
	buffer_len = read(sockfd,buffer,512);
	if (buffer_len < 0){
        	cout<<"\n ERROR receiving message from server 3"<<endl;
		exit(1);
	}
	buffer[buffer_len]='\0';
	/** message after receiving the reply from server 3*/
	cout<<"\n The Server 2 received the value "<<buffer<<" from Server 3 with port number ";
	cout<<ntohs(server_addr.sin_port)<<" and IP address "<<inet_ntoa(server_addr.sin_addr)<<endl;
	message = buffer;

	/** Close the TCP connection with server 3*/
	close(sockfd);
	cout<<"\n The Server 2 closed TCP connection with Server 3"<<endl;
	return message;
}
