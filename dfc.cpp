#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <signal.h>
#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/time.h>
#include <stdlib.h>
#include <memory.h>
#include <errno.h>
#include <string.h>
#include<iostream>
#include<sstream>
#include<fstream>
#include <sys/types.h>

#include <vector>

#include <map>

#include <stdbool.h>
#define MAXBUFSIZE 5000// 100 make this 512
#define PORT "10001" // the port client will be connecting to 

#define MAXDATASIZE 100 // max number of bytes we can get at once 


/* You will have to modify the program below */
using namespace std;

class IpAndPorts
{
public:
string ip;
string port;
IpAndPorts(string ip_i,string port_i){
ip = ip_i;
port = port_i;
}
IpAndPorts(){}
};

map<string,IpAndPorts> servers;
map<string,IpAndPorts>::iterator it; //iterator to loop through the map

string userId;
string password;

// To split using a delimiting character (used to load conf)
void split(const string &s, char delim, vector<string> &elems) {vector<string> splitString(const string &s, char delim) 
{string openConfFile(string fileName)
{
bool LoadConf()
{
  string confContent = openConfFile("dfc.conf");
  cout<<confContent<<endl;
  //exit(0);
  if(confContent == "")
	return false;
  vector<string> lines;
  lines = splitString(confContent, '\n'); // get the lines.
  for(int i = 0 ; i < 4; i++)
  {
  		vector<string> serversAndIP; 
  		serversAndIP = splitString(lines[i], ' '); //lines Server DFS1 127.0.0.1:10001 becomes space separated.
  		vector<string> ipport;
		ipport = splitString(serversAndIP[2],':'); // we get 127.0.0.1 and 10001
  		IpAndPorts* obj = new IpAndPorts(ipport[0],(ipport[1]));
  		servers[serversAndIP[1]] = *obj; 	
  }
  
  vector<string> username = splitString(lines[4], ' ');
  userId = username[1];

  vector<string> passwordStrings = splitString(lines[5], ' ');
  password = passwordStrings[1];
    
  //to check if loaded userid and passwords correctly
  for ( it = servers.begin(); it != servers.end(); it++ )
    cout << it->first<< ':'<< it->second.ip<<" "<<it->second.port<< endl ;
  cout<<userId<<" "<<password<<endl;

  
 return true;
}
void askInputs(char* userInput)
{
bool startsWith(const char *pre, const char *str)
{// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
int main (int argc, char * argv[])
{
   if(!LoadConf())
	{
	cout<<"Did not find server configuration file, terminating..."<<endl;
	return 1;
	}
	int sockfd, numbytes;
	char buf[MAXDATASIZE];
	struct addrinfo hints, *servinfo, *p;
    int rv;
    char s[INET6_ADDRSTRLEN];

    /*if (argc != 2) {
        fprintf(stderr,"usage: client hostname\n");
        exit(1);
    }*/

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

cout<<servers["DFS1"].ip.c_str();
    if ((rv = getaddrinfo(/*argv[1]*/servers["DFS1"].ip.c_str(), PORT, &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }

    // loop through all the results and connect to the first we can
    for(p = servinfo; p != NULL; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype,
                p->ai_protocol)) == -1) {
            perror("client: socket");
            continue;
        }

        if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            close(sockfd);
            perror("client: connect");
            continue;
        }

        break;
    }

    if (p == NULL) {
        fprintf(stderr, "client: failed to connect\n");
        return 2;
    }

    inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr),
            s, sizeof s);
    printf("client: connecting to %s\n", s);

    freeaddrinfo(servinfo); // all done with this structure

    if ((numbytes = recv(sockfd, buf, MAXDATASIZE-1, 0)) == -1) {
        perror("recv");
        exit(1);
    }

    buf[numbytes] = '\0';

    printf("client: received '%s'\n",buf);
   char terminalCommand[80];
		
		askInputs(terminalCommand);
		
		if(strcmp(terminalCommand,"exit")==0)
			return 0;
			send(sockfd, terminalCommand,10,0);
    close(sockfd);

    return 0;
	int nbytes;                             // number of bytes send by sendto()
	int sock;                               //this will be our socket
	char buffer[MAXBUFSIZE];

	struct sockaddr_in remote;              //"Internet socket address structure"

	/*if (argc < 3)
	{
		printf("USAGE:  <server_ip> <server_port>\n");
		exit(1);
	}*/

	/******************	bzero(&remote,sizeof(remote));               //zero the struct
	remote.sin_family = AF_INET;                 //address family
	remote.sin_port = htons(atoi(argv[2]));      //sets port to network byte order
	remote.sin_addr.s_addr = inet_addr(argv[1]); //sets remote IP address

	//Causes the system to create a generic socket of type UDP (datagram)
	if ((sock = socket(AF_INET,SOCK_DGRAM,IPPROTO_UDP)) <0)//**** CALL SOCKET() HERE TO CREATE A UDP SOCKET ****) < 0)
	{
	/******************
	char command[80];
	while(true)
	{		close(sock);

}
