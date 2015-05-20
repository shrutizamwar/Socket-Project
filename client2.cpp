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
#include "utilities.h"

#define SERVER_PORT 21010
using namespace std;
typedef map<string,string> clientmap;

int main(){

	/** read the input files having key value pairs and store them in a map*/
	string search, key,value;
	clientmap cmap = getMap("client2.txt");	

	/** Boot up message and retrieve the key from the search term entered by the user*/
	cout<<"\n Please Enter Your Search (USC, UCLA etc) : "<<endl;
	cin>>search;
	key = cmap.find(search)->second;

	/** message upon receiving the valid keyword*/
	cout<<"\n The Client 2 has received a request with search word " << search <<", which maps to key "<<key<<endl;

	/** create message to be sent to the server1*/
	char message[50] = "GET ";
	strcat(message,key.c_str());


	int sockfd,msg_len;
	struct hostent *host; 
	struct sockaddr_in server_addr, my_addr;
	socklen_t my_len,server_len;

	/**create client UDP socket*/
	sockfd = socket(AF_INET,SOCK_DGRAM,0);
	if(sockfd<0){
		cout<<"\n Error creating UDP socket"<<endl;
		exit(1);
	}

	/** get more details of host from the host name*/
	host = gethostbyname("nunki.usc.edu");
        if (!host) {
		cout<<"\n Could not obtain host address from name"<<endl;
		exit(1);
	}
	
	memset((char*)&my_addr, 0, sizeof(my_addr));
	my_addr.sin_family = AF_INET;
	my_addr.sin_port = htons(0);
	memcpy((void *)&my_addr.sin_addr, host->h_addr_list[0], host->h_length);
	
	/**Bind client socket*/
	if (bind(sockfd, (struct sockaddr *)&my_addr, sizeof(my_addr)) < 0) {
		cout<<"\n Bind UDP socket failed"<<endl;
		exit(1);
	}
	
	memset((char*)&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(SERVER_PORT);
	memcpy((void *)&server_addr.sin_addr, host->h_addr_list[0], host->h_length);

	/** send request to server 1*/
	msg_len = sendto(sockfd, message, strlen(message), 0, (struct sockaddr *)&server_addr, sizeof(server_addr));
	if( msg_len < 0){
		cout<<"\n Sending message to server 1 failed"<<endl;
		exit(1);
	}

	/** message after sending the request to server 1*/
	cout<<"\n The Client2 sends the request "<< message<< " to the server1 with port number "<<ntohs(server_addr.sin_port);
	cout<<" and IP address "<< inet_ntoa(server_addr.sin_addr)<<endl;

	/** get socket name of client to show its details in the message*/	
	my_len = sizeof(my_addr);
	if (getsockname(sockfd, (struct sockaddr *)&my_addr, &my_len) == -1){
		cout<<"\n Error to get socket name"<<endl;
		exit(1);
	}

	/** message to show client is on which port and address*/
	cout<<"\n The Client2's port number is "<<ntohs(my_addr.sin_port);
	cout<<" and IP address is "<< inet_ntoa(my_addr.sin_addr)<<"\n"<<endl;

	/** receive message from server*/
	server_len = sizeof(my_addr);
	msg_len = recvfrom(sockfd, message,512,0 ,(struct sockaddr *)&my_addr, &server_len);
	if(msg_len < 0){
		cout<<"\n Error receiving message from server 1"<<endl;
		exit(1);
	}
	message[msg_len]='\0';
	
	/** message after receiving the reply from server 1*/
	cout<<"\n The Client 2 received the value "<<message<<" from Server 1 with port number " <<ntohs(server_addr.sin_port);
	cout<<" and IP address "<< inet_ntoa(server_addr.sin_addr)<<"\n"<<endl;

	if (getsockname(sockfd, (struct sockaddr *)&my_addr, &my_len) == -1){
		cout<<"\n Error to get socket name"<<endl;
		exit(1);
	}
	cout<<"\n The Client2's port number is "<<ntohs(my_addr.sin_port);
	cout<<" and IP address is "<< inet_ntoa(my_addr.sin_addr)<<"\n"<<endl;

	/** Close the UDP connection with server 1*/
	close(sockfd);
	return 0;
}

