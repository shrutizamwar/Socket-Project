#include <iostream>
#include <string>
#include <map>
#include <cstring>
#include <unistd.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <vector>
#include "utilities.h"

#define PORT1 21010
#define PORT2 22010

using namespace std;
typedef map<string,string> servermap;

string getValueFromServer2(char buffer[]);

int main(){

	string searchKey, value;
	servermap smap = getMap("server1.txt");

	int sock_udp,msg_len,client_count=0;
	string client_name;
	char buffer[512], message[50];
	struct hostent *server;
	struct sockaddr_in server_addr, client_addr;
	socklen_t client_len = sizeof(client_addr);
	socklen_t server_len ;

	/** create UDP socket */
	sock_udp = socket(AF_INET,SOCK_DGRAM,0);
	if(sock_udp<0){
		cout<<"\n Error creating UDP socket"<<endl;
		exit(1);
	}

	/**bind the UDP socket*/
	server = gethostbyname("nunki.usc.edu");
	memset((char *)&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(PORT1);
	memcpy((void *)&server_addr.sin_addr, server->h_addr_list[0], server->h_length);
	if (bind(sock_udp, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
		cout<<"\n Bind UDP socket failed"<<endl;
		exit(1);
	}

	/** get UDP socket details of server 1 */
	server_len = sizeof(server_addr);
	if (getsockname(sock_udp, (struct sockaddr *)&server_addr, &server_len) == -1){
		cout<<"\n Error to get socket name"<<endl;
		exit(1);
	}
	/** Boot up message which shows port no. and IP address of server 1*/
	cout<<"\n The server 1 has UDP port number "<<ntohs(server_addr.sin_port);
	cout<<" and IP address "<< inet_ntoa(server_addr.sin_addr)<<endl;

	/**receive the request from the client*/
	for(;;) {
        msg_len = recvfrom(sock_udp, buffer, 512, 0, (struct sockaddr *)&client_addr, &client_len);
		if (msg_len < 0) {
            cout<<"\n Error receiving message from client"<<endl;
			exit(1);
        }
		buffer[msg_len]='\0';
		searchKey = getWords(buffer).at(1);
		
		if(client_count % 2 == 0){
			client_name = "Client 1";
		}
		else{
			client_name = "Client 2";
		}
		
		/** message upon receiving the first request from client 1*/
		cout<<"\n The Server 1 has received a request with key "<<searchKey<<" from "<< client_name <<" with port number ";
		cout<< ntohs(client_addr.sin_port)<<" and IP address "<< inet_ntoa(client_addr.sin_addr)<<endl;

		/** If the server 1 has entry in its table then reply to the client 1 immediately*/
		if(smap.find(searchKey)!=smap.end()){
			value = smap.find(searchKey)->second;
			strcpy(message, "POST ");
			strcat(message,value.c_str());
		}
		/** If server 1 doesn't have entry in its table request server 2 to get the value*/		
		else{
			strcpy(message,getValueFromServer2(buffer).c_str());
			smap[searchKey] = getWords(buffer).at(1);
		}
		
		/** send the value to client 1*/
		msg_len = sendto(sock_udp, message ,strlen(message), 0,(struct sockaddr *) &client_addr,sizeof(client_addr));
		if(msg_len < 0){
			cout<<"\n Error sending message to client"<<endl;
			exit(1);
		}
		/** message after sending the reply to client 1*/
		cout<<"\n The Server 1 sends the reply "<< message << " to "<< client_name <<" with port number "<<ntohs(client_addr.sin_port);
		cout<<" and IP address "<< inet_ntoa(client_addr.sin_addr)<<endl;
		
		client_count++;
    }
	return 0;
}

string getValueFromServer2(char buffer[]){
	int sockfd, buffer_len;
	struct hostent *server;
	struct sockaddr_in server_addr, my_addr;
	socklen_t my_len;
	string message;

	/**create TCP socket*/
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

	/* connect to server */
	if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
		cout<<"\n Connect failed"<<endl;
		exit(1);
	}
	
	/** send request to server 2*/
	buffer_len = write(sockfd,buffer,strlen(buffer));
	if (buffer_len < 0){
        cout<<"\n ERROR sending message to server2"<<endl;
		exit(1);
	}

	/** get socket details of server 1*/
	if (getsockname(sockfd, (struct sockaddr *)&my_addr, &my_len) == -1){
		cout<<"\n Error to get socket name"<<endl;
		exit(1);
	}

	/** message after sending request to server 2*/
	cout<<"\n The Server 1 sends request "<< buffer <<" to the Server 2"<<endl;
	cout<<"\n The TCP port number is  "<< ntohs(my_addr.sin_port);
	cout<<" and the IP address is "<< inet_ntoa(my_addr.sin_addr)<<endl;
	
	/** receive the value from server 2*/
	buffer_len = read(sockfd,buffer,512);
	if (buffer_len < 0){
        cout<<"\n ERROR receiving message from server 2"<<endl;
		exit(1);
	}
	buffer[buffer_len]='\0';
	
	/** message after receiving the reply from server 2*/
	cout<<"\n The Server 1 received the value "<<buffer<<" from Server 2 with port number ";
	cout<<ntohs(server_addr.sin_port)<<" and IP address "<<inet_ntoa(server_addr.sin_addr)<<endl;
	message = buffer;

	/** Close the TCP connection with server 2*/
	close(sockfd);
	cout<<"\n The Server 1 closed TCP connection with Server 2"<<endl;
	return message;
}
