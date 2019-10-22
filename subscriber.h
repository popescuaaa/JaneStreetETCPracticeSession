// Popescu Andrei-Gabriel 323CA
// Implementare pentru clientul TCP

#include <bits/stdc++.h>
#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <algorithm>
#include <netdb.h> 
#include <time.h>



#define BUFLEN 256

using namespace std;

void error(char *msg) {
    perror(msg);
    exit(0);
}

class Subscriber {
private:
	int sockfd, n;
    struct sockaddr_in serv_addr;
    struct hostent *server;

    fd_set read_fds;  //multimea de citire folosita in select()
    fd_set tmp_fds;    //multime folosita temporar 
    int fdmax;     //valoare maxima file descriptor din multimea read_fds
    int ID; // id-ul de client pentru identificarea la nivel de client TCP 
    char buffer[BUFLEN];
    char* host;
    int portno;
    int i;

    
public:

	Subscriber(int portno, char* host, int ID) {
		this->portno = portno;
		this->host = host;
		this->ID =  ID;
	}	

	void init() {
		
		FD_ZERO(&read_fds);
    	FD_ZERO(&tmp_fds);
		
		sockfd = socket(AF_INET, SOCK_STREAM, 0);

	    if (sockfd < 0) 
	        error((char *)"ERROR opening socket");
	    
	    serv_addr.sin_family = AF_INET;
	    serv_addr.sin_port = htons(portno);
	    inet_aton(host, &serv_addr.sin_addr);
	    
	    // Conectarea la server
	    if (connect(sockfd,(struct sockaddr*) &serv_addr,sizeof(serv_addr)) < 0) 
	        error((char *)"ERROR connecting");    
	    
	    FD_SET(sockfd, &read_fds);
	    FD_SET(0, &read_fds);
	    fdmax = sockfd;
	}

	void run(){
		init();

		while (1) {
        tmp_fds = read_fds; 
        if (select(fdmax + 1, &tmp_fds, NULL, NULL, NULL) == -1) 
            error((char *)"ERROR in select");
    
        for(i = 0; i <= fdmax; i++) {
            if (FD_ISSET(i, &tmp_fds)) {
                if(i != 0)
                {
                    memset(buffer, 0, BUFLEN);
                    if ((n = recv(i, buffer, sizeof(buffer), 0)) <= 0) {
                        if (n == 0) {
                            cout << "The server is down!";
                            return;
                        } else {
                            error((char *)"ERROR in recv");
                        }
                        close(i); 
                        FD_CLR(i, &read_fds);  
                    }
                    else{
                        cout << buffer << endl;
                       
                    }
                }
                else
                {   
					
                    memset(buffer, 0 , BUFLEN);
                    fgets(buffer, BUFLEN-1, stdin);
                    if (strcmp(buffer, "exit") == 0) {
                    	exit(1);
                    }

                    n = send(sockfd,buffer,strlen(buffer), 0);
                    if (n < 0) 
                        error((char *)"ERROR writing to socket");
                }
            }
        }

   

    }
   }

   
   
    
	
};
