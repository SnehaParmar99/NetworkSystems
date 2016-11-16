#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include<iostream>
#include<sstream>
#include<fstream>

#include <vector>

#include <map>

#define MAXLINE 30000 /*max text line length*/
#define LISTENQ 8000 /*maximum number of client connections*/

using namespace std;


class ContentTypes
{
	public:
	string fileType ;
	string contentType;
	static int size;
	public:
	ContentTypes(){fileType = ""; contentType = "";}
	ContentTypes(string file,string type)
	{
		fileType = file;
		contentType = type;
		size++;
	}
};
int ContentTypes::size = 0;

//some global variables
 string RootDir = "";
 ContentTypes* contentTypes[20] ;
 int portNumber = 0;
 string serverFolder = "";
 map<string,string> users;
 map<string, string>::iterator it; //iterator to loop through the map
 //some useful string functions
 
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
class Request
{
	public:
	string httpVerb;
	string URL;
	float httpversion;
	
	public:
	Request(string rawRequest)
	{

		istringstream f(rawRequest);
		string line;
		string requestHeaderData[3];
		int i = 0; //since we need just the first three words here
		while (getline(f, line,' ') && i < 3) {
			requestHeaderData[i] = line;
			i++;
		    }
		httpVerb = requestHeaderData[0];
		URL = requestHeaderData[1];
		stringstream ss(requestHeaderData[2].substr(5,3));
		ss>> httpversion;
	}
	bool checkHTTPverb()
	{
	//return false if anything other than get

	//httpVerb = "POST"; //TO TEST
	return (httpVerb=="GET");
	}
	bool checkURL()
	{
	//return false if anything invalid - put specific enum codes here later as per error type, this is checked by the file pointer now.
	return true;
	}
	string getURLExtension()
	{
		if(URL == "/")
			URL = "/index.html";
		return "."+URL.substr(URL.find_last_of(".") + 1);
	}
	bool checkHTTPVersion()
	{
	//return false if anything other than 1.0 or 1.1
	//httpversion = 3.1;		//TO TEST
	return (((int)(httpversion*10-10) == 0)||((int)(httpversion*10-11) == 0));
	}
};

class Response
{
	public:
	int responseStatusCode;
	string responseStatus;
	string ContentType;
	int ContentLength;
	string content;
	char* c_content;
	string URL;
	bool validURL;
	bool validMethod;
	bool validVersion;
	bool validContentType;	
	string HTTPVersion;
	string HTTPMethod;
	string RequestedType;
	
};

string lookUpContentType(string fileType)
{
for(int i = 0; i < ContentTypes::size; i ++)
	{
	if(strcmp(fileType.c_str(),(contentTypes[i]->fileType).c_str())==0)
		return contentTypes[i]->contentType;
	}
return ""; //if this type if not found return empty. this is handled by caller to indicate file not implemented.
}

string openConfFile(string fileName)
{
	int indexConf = open("dfs.conf",O_RDONLY);//"/home/user/Documents/NetWorkSystems/images/wine3.jpg",O_RDONLY);
	if(indexConf == -1)
		return "";
		
	else
	{
	close(indexConf);
	ifstream file(fileName.c_str());//"/home/user/Documents/NetWorkSystems/rootdir/www/index.html");//change this hardcode
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

Response* getResponseVariables(string rawRequest) 
{

	Request* request = new Request(rawRequest);
	Response* response = new Response();

	response->validURL = request->checkURL();
	response->validMethod = request->checkHTTPverb();
	response->validVersion = request->checkHTTPVersion();


	ostringstream versionnumber;
	versionnumber << request->httpversion;
	string versionnumberS(versionnumber.str());

	response->HTTPVersion = versionnumberS;
	response->HTTPMethod = request->httpVerb;


	bool isValid = request->checkHTTPverb() && request->checkURL() && request->checkHTTPVersion();
	if(isValid)
	{

		response->responseStatusCode = 200;
		response->responseStatus="OK";
		string type = lookUpContentType(request->getURLExtension());
		response->RequestedType = request->getURLExtension();

		if(type == "")
			response->validContentType = false; 
		else
			response->validContentType = true;
		response->ContentType=type;


		if(strcmp(request->URL.c_str(),"/") == 0)
		  request->URL = "/index.html";

		response->URL = RootDir.c_str()+request->URL;

	}
	return response;
}
// this will load the conf content into the map 'users' to get the username and passwords
bool LoadConf()
{
  string confContent = openConfFile("dfs.conf");
  cout<<confContent<<endl;
  //exit(0);
  if(confContent == "")
	return false;
  vector<string> lines;
  lines = splitString(confContent, '\n'); // get the lines.
  for(int i = 0 ; i < lines.size(); i++)
  {
  		vector<string> userIdAndPassword;
  		userIdAndPassword = splitString(lines[i], ' ');
  		users[userIdAndPassword[0]] = userIdAndPassword[1];	
  }
  //to check if loaded userid and passwords correctly
  //for ( it = users.begin(); it != users.end(); it++ )
  //  cout << it->first<< ':'<< it->second<< endl ;

  
 return true;
}
void error501Message(int connfd, char* EnteredValue,int choice)
{
      char *contentHeader;
      contentHeader = (char*)malloc(50);
      strcpy(contentHeader, "Content-Type: ");
      strcat(contentHeader, "text/html");
      strcat(contentHeader, "\n");

      char * strPortNumber = (char*)malloc(10);
      sprintf(strPortNumber, "%d", portNumber);
      char *responseBody = (char*)malloc(200);
      strcpy(responseBody, "<html><body>501 Not	Implemented file type ");
      strcat(responseBody, EnteredValue);
      strcat(responseBody, "</body></html>\n");

      int lengthOfMessage = strlen(responseBody);
      char strLengthOfMessage[20];
      sprintf(strLengthOfMessage, "%d", lengthOfMessage);
      
      char *lengthOfContent;
      lengthOfContent = (char*)malloc(50);
      strcpy(lengthOfContent, "Content-Length: ");
      strcat(lengthOfContent, strLengthOfMessage);
      strcat(lengthOfContent, "\n");
      int lengthOfContentLength = strlen(lengthOfContent);

      send(connfd, "HTTP/1.1 501 Not Implemented\n",29,0);
      send(connfd, contentHeader, strlen(contentHeader),0);
      send(connfd, lengthOfContent, lengthOfContentLength, 0);
      send(connfd, "Connection: keep-alive\n\n",24,0);
      send(connfd, responseBody, lengthOfMessage, 0);


}
void error400Message(int connfd, char* EnteredValue,int choice)
{
      char *contentHeader;
      contentHeader = (char*)malloc(50);
      strcpy(contentHeader, "Content-Type: ");
      strcat(contentHeader, "text/html");
      strcat(contentHeader, "\n");

      char * strPortNumber = (char*)malloc(10);
      sprintf(strPortNumber, "%d", portNumber);      
      char *responseBody = (char*)malloc(200);
      strcpy(responseBody, "<html><body>400 Bad	Request	Reason:	");
      if(choice == 1)
      strcat(responseBody, "Invalid Method : ");
      else if(choice == 2)
      strcat(responseBody, "Invalid HTTP Version : ");
      strcat(responseBody, EnteredValue);
      strcat(responseBody, "</body></html>\n");

      int lengthOfMessage = strlen(responseBody);
      char strLengthOfMessage[20];

      sprintf(strLengthOfMessage, "%d", lengthOfMessage);
      char *lengthOfContent;
      lengthOfContent = (char*)malloc(50);
      strcpy(lengthOfContent, "Content-Length: ");
      strcat(lengthOfContent, strLengthOfMessage);
      strcat(lengthOfContent, "\n");
      int lengthOfContentLength = strlen(lengthOfContent);

      send(connfd, "HTTP/1.1 400 Bad Request\n",25,0);
      send(connfd, contentHeader, strlen(contentHeader),0);
      send(connfd, lengthOfContent, lengthOfContentLength, 0);
      send(connfd, "Connection: keep-alive\n\n",24,0);
      send(connfd, responseBody, lengthOfMessage, 0);


}
void error500Message(int connfd)
{
	char *contentHeader;
	contentHeader = (char*)malloc(50);
	strcpy(contentHeader, "Content-Type: ");
	strcat(contentHeader, "text/html");
	strcat(contentHeader, "\n");

	char * strPortNumber = (char*)malloc(10);
	sprintf(strPortNumber, "%d", portNumber);
	char *responseBody = (char*)malloc(200);
	strcpy(responseBody, "<html><body>500 Internal Server Error: cannot allocate memory");
	strcat(responseBody, "</body></html>\n");

	int lengthOfMessage = strlen(responseBody);
	char strLengthOfMessage[20];
	sprintf(strLengthOfMessage, "%d", lengthOfMessage);

	char *lengthOfContent;
	lengthOfContent = (char*)malloc(50);
	strcpy(lengthOfContent, "Content-Length: ");
	strcat(lengthOfContent, strLengthOfMessage);
	strcat(lengthOfContent, "\n");
	int lengthOfContentLength = strlen(lengthOfContent);

	send(connfd, "HTTP/1.1 500 Internal Server Error\n",35,0);
	send(connfd, contentHeader, strlen(contentHeader),0);
	send(connfd, lengthOfContent, lengthOfContentLength, 0);
	send(connfd, "Connection: keep-alive\n\n",24,0);
	send(connfd, responseBody, lengthOfMessage, 0);

}
void error404Message(int connfd, char* urlEntered)
{
	char *contentHeader;
	contentHeader = (char*)malloc(50);
	strcpy(contentHeader, "Content-Type: ");
	strcat(contentHeader, "text/html");
	strcat(contentHeader, "\n");
	char * strPortNumber = (char*)malloc(10);
	sprintf(strPortNumber, "%d", portNumber);
	char *responseBody = (char*)malloc(200);
	strcpy(responseBody, "<html><body>ERROR: 404. Not Found Reason URL does not exist: http://localhost:");
	strcat(responseBody, strPortNumber);
	strcat(responseBody, urlEntered);
	strcat(responseBody, "</body></html>\n");
	int lengthOfMessage = strlen(responseBody);
	char strLengthOfMessage[20];
	sprintf(strLengthOfMessage, "%d", lengthOfMessage);
	char *lengthOfContent;
	lengthOfContent = (char*)malloc(50);
	strcpy(lengthOfContent, "Content-Length: ");
	strcat(lengthOfContent, strLengthOfMessage);
	strcat(lengthOfContent, "\n");
	int lengthOfContentLength = strlen(lengthOfContent);
	send(connfd, "HTTP/1.1 404 Not Found\n",23,0);
	send(connfd, contentHeader, strlen(contentHeader),0);
	send(connfd, lengthOfContent, lengthOfContentLength, 0);
	send(connfd, "Connection: keep-alive\n\n",24,0);
	send(connfd, responseBody, lengthOfMessage, 0);

}
int main (int argc, char **argv)
{
 if (argc != 3)
	{
		cout<< "USAGE: <serverFolder> <port>"<<endl;
		exit(1);
	}
 serverFolder  = argv[1];
 portNumber = atoi(argv[2]);
 cout<<serverFolder<<" "<<portNumber<<endl;
 if(!LoadConf())
	{
	cout<<"Did not find server configuration file, terminating..."<<endl;
	return 1;
	}
 /*if(portNumber < 1024)
	{
	cout<<"Invalid port number, terminating..."<<endl;
	return 1;
	}*/
	
 int listenfd, connfd, n;
 pid_t childpid;
 socklen_t clilen;
 char buf[MAXLINE];
 struct sockaddr_in cliaddr, servaddr;

 //Create a socket for the soclet
 //If sockfd<0 there was an error in the creation of the socket
 if ((listenfd = socket (AF_INET, SOCK_STREAM, 0)) <0) {
  perror("Problem in creating the socket");
  exit(2);
 }


 //preparation of the socket address
 servaddr.sin_family = AF_INET;
 servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
 servaddr.sin_port = htons(portNumber);

 //bind the socket
 bind (listenfd, (struct sockaddr *) &servaddr, sizeof(servaddr));

 //listen to the socket by creating a connection queue, then wait for clients
 listen (listenfd, LISTENQ);
 cout<<endl<<"Server running...waiting for connections at "<<portNumber<<endl;

 for ( ; ; ) {
  int indexFd ;
  clilen = sizeof(cliaddr);
  //accept a connection
  connfd = accept (listenfd, (struct sockaddr *) &cliaddr, &clilen);

	send(connfd, "HTTP/1.1 200 OK\n",16,0);
  //printf("%s\n","Received request...");

  if ( (childpid = fork ()) == 0 ) {//if it’s 0, it’s child process

  //printf ("%s\n","Child created for dealing with client requests");

  close (listenfd);

  if ( (n = recv(connfd, buf, MAXLINE,0)) > 0)  { 
cout<<"received"<<buf;
   Response* s = getResponseVariables(buf);
try{	

//throw 20;//To test 500

if(!(s->validMethod))
{
cout<<"Invalid method "<<s->HTTPMethod<<endl;
error400Message(connfd, (char*)(s->HTTPMethod.c_str()),1);
}
else if(!s->validVersion)
{
cout<<"Invalid http version "<<s->HTTPVersion<<endl;
error400Message(connfd, (char*)(s->HTTPVersion.c_str()),2);
}
else if(!s->validContentType)
{
cout<<"Requested file type not supported "<<s->RequestedType<<endl;
error501Message(connfd, (char*)(s->RequestedType.c_str()),2);
}
else{
const char* path = const_cast<char*>(s->URL.c_str());
 indexFd = open(s->URL.c_str(),O_RDONLY);//"/home/user/Documents/NetWorkSystems/images/wine3.jpg",O_RDONLY);
//int indexFd = open("/home/user/Documents/NetWorkSystems/images/wine3.jpg",O_RDONLY);
  //send the response
  if(indexFd  != -1)
    {
      char *contentHeader;
      contentHeader = (char*)malloc(50);
      strcpy(contentHeader, "Content-Type: ");
      strcat(contentHeader, s->ContentType.c_str());
      strcat(contentHeader, "\n");
      int contentHeaderLength = strlen(contentHeader);
      int sizeOfFile = lseek(indexFd, 0, SEEK_END);
      lseek(indexFd,0, SEEK_SET);
      char fileSizeStr[20];
      sprintf(fileSizeStr, "%d", sizeOfFile);
      char *lengthOfContent;
      lengthOfContent = (char*)malloc(50);
      strcpy(lengthOfContent, "Content-Length: ");
      strcat(lengthOfContent, fileSizeStr);
      strcat(lengthOfContent, "\n");
      int lengthOfContentLength = strlen(lengthOfContent);
	send(connfd, "HTTP/1.1 200 OK\n",16,0);
	send(connfd, contentHeader, contentHeaderLength, 0);
	send(connfd, lengthOfContent, lengthOfContentLength, 0);
	send(connfd, "Connection: keep-alive\n\n",24,0);
	int bytesRead;
	char *indexDataToSend = (char*)malloc(MAXLINE);
	while((bytesRead = read(indexFd, indexDataToSend, MAXLINE))>0)
	  send(connfd, indexDataToSend, bytesRead,0);
    }
  else 
	{
	//file not found thing
	      //cout<<"ERROR: File not found "<<" "<<s->URL<<endl;
	      error404Message(connfd, (char*)(s->URL.c_str()));
	}}
  }
catch(...)
{
cout<<"Server internal error "<<endl;
error500Message(connfd);
}
  shutdown(connfd, SHUT_RDWR);
  close(connfd);
  close(indexFd);
  send(connfd, buf, n, 0);
  }
  if (n < 0)
   printf("%s\n", "Read error");
  //close (connfd);
  exit(0);
 }
 //close socket of the server
 close(connfd);
}
}
