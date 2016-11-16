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

map<string,string> servers;
map<string,string>::iterator it; //iterator to loop through the map

string userId;
string password;

// To split using a delimiting character (used to load conf)
void split(const string &s, char delim, vector<string> &elems) {
    stringstream ss;
    ss.str(s);
    string item;
    while (getline(ss, item, delim)) {
        elems.push_back(item);
    }
}
vector<string> splitString(const string &s, char delim) 
{
    vector<string> elems;
    split(s, delim, elems);
    return elems;
}
string openConfFile(string fileName)
{
	int indexConf = open("dfc.conf",O_RDONLY);
	if(indexConf == -1)
		return "";
		
	else
	{
	close(indexConf);
	ifstream file(fileName.c_str());
	string str;
	string file_contents;
	while (getline(file, str))
	{
	  file_contents += str;
	  file_contents.push_back('\n');
	} 
	return file_contents;
	}	
}

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
  		serversAndIP = splitString(lines[i], ' ');
  		servers[serversAndIP[1]] = serversAndIP[2];	
  }
  
  vector<string> username = splitString(lines[4], ' ');
  userId = username[1];

  vector<string> passwordStrings = splitString(lines[5], ' ');
  password = passwordStrings[1];
    
  //to check if loaded userid and passwords correctly
  for ( it = servers.begin(); it != servers.end(); it++ )
    cout << it->first<< ':'<< it->second<< endl ;
  cout<<userId<<" "<<password<<endl;

  
 return true;
}
void askInputs(char* userInput)
{
	char str[80];
	int i;
	printf("Enter a command (get [Filename] put [Filename] ls exit): ");
	fgets(str, 50, stdin);

	/* remove newline, if present */
	i = strlen(str)-1;
	if( str[ i ] == '\n') 
		str[i] = '\0';


	strcpy(userInput,"");
	strcat(userInput,str);
}

bool startsWith(const char *pre, const char *str)
{
    size_t lenpre = strlen(pre),
           lenstr = strlen(str);
    return lenstr < lenpre ? false : strncmp(pre, str, lenpre) == 0;
}
// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

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

    if (argc != 2) {
        fprintf(stderr,"usage: client hostname\n");
        exit(1);
    }

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    if ((rv = getaddrinfo(argv[1], PORT, &hints, &servinfo)) != 0) {
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

	if (argc < 3)
	{
		printf("USAGE:  <server_ip> <server_port>\n");
		exit(1);
	}

	/******************
	  Here we populate a sockaddr_in struct with
	  information regarding where we'd like to send our packet 
	  i.e the Server.
	 ******************/
	bzero(&remote,sizeof(remote));               //zero the struct
	remote.sin_family = AF_INET;                 //address family
	remote.sin_port = htons(atoi(argv[2]));      //sets port to network byte order
	remote.sin_addr.s_addr = inet_addr(argv[1]); //sets remote IP address

	//Causes the system to create a generic socket of type UDP (datagram)
	if ((sock = socket(AF_INET,SOCK_DGRAM,IPPROTO_UDP)) <0)//**** CALL SOCKET() HERE TO CREATE A UDP SOCKET ****) < 0)
	{
		printf("unable to create socket");
	}

	/******************
	  sendto() sends immediately.  
	  it will report an error if the message fails to leave the computer
	  however, with UDP, there is no error if the message is lost in the network once it leaves the computer.
	 ******************/

	char command[80];
	while(true)
	{	
		askInputs(command);
		if(strcmp(command,"exit")==0)
			break;
		
		nbytes = sendto(sock,command,sizeof(command),0,(struct sockaddr *)&remote,sizeof(remote));

		// Blocks till bytes are received
		struct sockaddr_in from_addr;
		int addr_length = sizeof(struct sockaddr);
		bzero(buffer,sizeof(buffer));

		unsigned int remote_length = sizeof(remote);         //length of the sockaddr_in structure

		
				

		nbytes = recvfrom(sock,buffer,MAXBUFSIZE, 0, (struct sockaddr *)&remote, &remote_length); //this returns number of packets for get and the file names for ls and ack for put
		
		if(strcmp(command,"ls")==0)
			{
			printf("%s\n",buffer);		
			continue;
			}
		else if(!( (startsWith("get ",command)) || (startsWith("put ",command)) ))
		{
			
			printf("%s\n",buffer);		
			continue;
		}
////////////////////////////// understand the command
		char command1[80];
		char fileName[80];
		char word[80];
		char* split;
		bool isValid = true;
		split=strtok(command," ");

		    strcpy(word,split);
		    split=strtok(NULL," ");


		    if((strcmp(word,"get") == 0) || (strcmp(word,"put") == 0))
			strcpy(command1,word);
		    else
			isValid = false;
			
		    strcpy(word,split);
    		    strcpy(fileName,word);

////////////////////////////////
		if(strcmp(command1,"get")==0)
		{
			char* ack = "ack";
			nbytes = sendto(sock,ack,sizeof(ack),0,(struct sockaddr *)&remote,sizeof(remote));


			unsigned long numberOfPackets;

			memcpy( &numberOfPackets, buffer, sizeof( unsigned long ) );

			FILE* file = fopen( "clientFile", "wb" );

			while(numberOfPackets > 0)
			{		
				nbytes = recvfrom(sock,buffer,MAXBUFSIZE, 0, (struct sockaddr *)&remote, &remote_length);
				fwrite( buffer, 1, nbytes, file );

				numberOfPackets = numberOfPackets - 1;

				strcpy(buffer,ack);		
				nbytes = sendto(sock,buffer,sizeof(ack),0,(struct sockaddr *)&remote,sizeof(remote));			
			}

			fclose(file);
		}
		 if(strcmp(command1,"put")==0)
		{
				char file_buffer[MAXBUFSIZE];
				FILE *fp;
				fp=fopen(fileName,"r");
				if(fp==NULL)
				{
				printf("file does not exist\n");
				continue;
				}

				fseek(fp,0,SEEK_END);
				size_t file_size=ftell(fp);
				fseek(fp,0,SEEK_SET);
				
				unsigned long numberOfPackets = (unsigned long)file_size/((unsigned long)sizeof(file_buffer))+1;
				
				unsigned char numberOfPacketsPacket[sizeof(unsigned long )];
				memcpy( numberOfPacketsPacket, & numberOfPackets, sizeof( unsigned long ) );

				nbytes = sendto(sock,numberOfPacketsPacket,sizeof(numberOfPacketsPacket),0,(struct sockaddr *)&remote, sizeof(remote));     // send no. of bytes to send
				nbytes = recvfrom(sock,buffer,MAXBUFSIZE, 0, (struct sockaddr *)&remote, &remote_length);			         //get some ack

				char ack[10];
				size_t bytesRead = 0;
				long sizeRead = 0;

				if (fp != NULL)    
				{

				  while ((bytesRead = fread(file_buffer,1, sizeof(file_buffer) , fp)) > 0)  // read up to sizeof(buffer) bytes
				  {					
					sizeRead++;					
					nbytes = sendto(sock,file_buffer,bytesRead,0,(struct sockaddr *)&remote, sizeof(remote));     // process bytesRead worth of data in buffer
					
					struct timeval timeout;//={2,0}; //set timeout for 2 seconds
					timeout.tv_sec = 100;
					setsockopt(sock,SOL_SOCKET,SO_RCVTIMEO,(struct timeval*)&timeout,sizeof(struct timeval));


					nbytes = recvfrom(sock,ack,MAXBUFSIZE, 0, (struct sockaddr *)&remote, &remote_length);
					while(nbytes<0)
					//while(strcmp(ack,"ack")!=0)
					{
						nbytes = sendto(sock,file_buffer,bytesRead,0,(struct sockaddr *)&remote, sizeof(remote));     // process bytesRead worth of data in buffer
						nbytes = recvfrom(sock,ack,sizeof(ack), 0, (struct sockaddr *)&remote, &remote_length);
					}
				  }
				}
		}
	}
	close(sock);

}

