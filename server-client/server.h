// Popescu Andrei-Gabriel 323CA
// Implementarea clasei server care poate accepta atat clienti de tip UDP
// cat si clienti de tip TCP 

#include <bits/stdc++.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <cstdint>
#include <time.h>
#include <arpa/inet.h>
#include "helpers.h"
#include "models.h"


#define NUMBER 42
#define NO_CLIENT 99999
using namespace std;

struct Client{
	vector<pair<string, int>> database;
	vector<pair<string, string>> old_messages;
	int client_id;
	bool status;
};

/**
 *  RANDOM
 * 
 * */
struct APPL{
	string name = "APPL";
	double current_price;
};

struct GOOG{
	string name = "APPL";
	double current_price;
};

struct MSFT{
	string name = "MSFT";
	double current_price;
};

struct NOKIA{
	string name = "NOKIA";
	double current_price;
};

/** 
 *  Stable
 * 
 **/

struct ETF{
	string name=  "ETF";
	double current_price;
};

struct BOND{
	string name = "BOND";
	double current_price;
};

class Server {
private:
	int sockfd; // file descriptor pentru socket
	int newsockfd; // file descriptor pentru noul socket
	int portno; // port number
	int sock;  // file descriptor pentru udp
    int status; // idicator de status pentru conexiunea udp

	char buffer[BUFLEN];
	struct sockaddr_in serv_addr, cli_addr, from_station; // adresa serverului si a clientului
	int n,ret;
	socklen_t clilen;

	fd_set read_fds;	// multimea de citire folosita in select()
	fd_set tmp_fds;		// multime folosita temporar
	int fdmax;			// valoare maxima fd din multimea read_fds

	vector<Client> clients; // clientii prezenti pe server in orice moment de timp 
	vector<string> topics; // topicurile mesajelor
    vector< pair<string, string> > store; // aici vor fi stocate toate mesajele primite 
    											// de server de la udp
    vector<pair<int, string>> old_messages;

public:
	Server(int portno) {
		this->portno = portno;
	} 

	void init(){
	// Setup Sockets pentru TCP si pentru UDP 

		FD_ZERO(&read_fds);
		FD_ZERO(&tmp_fds);

		sockfd = socket(AF_INET, SOCK_STREAM, 0); // type = SOCK_STREAM => TCP
		DIE(sockfd < 0, "socket tcp");

		sock = socket(PF_INET, SOCK_DGRAM, 0); // type = SOCK_DGRAM => UDP
		DIE(sock < 0, "socket udp");

		memset((char *) &serv_addr, 0, sizeof(serv_addr));
		serv_addr.sin_family = AF_INET;
		serv_addr.sin_port = htons(portno);
		serv_addr.sin_addr.s_addr = INADDR_ANY;

	    from_station.sin_family = AF_INET;
	    inet_aton("127.0.0.1", &from_station.sin_addr);
	    from_station.sin_port = htons(portno);

	    status = bind(sock,(struct sockaddr*)&from_station, sizeof(from_station));
	    DIE(status < 0, "bind udp");

		ret = bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(struct sockaddr));
		DIE(ret < 0, "bind tcp");

		ret = listen(sockfd, MAX_CLIENTS);
		DIE(ret < 0, "listen");

		FD_SET(sockfd, &read_fds);
		FD_SET(sock, &read_fds); 

		fdmax = (sock > sockfd ? sock : sockfd) + 1; // update la numarul maxim de conexiuni active 

	}

	void run() {
			init();
			while (1) {
			tmp_fds = read_fds;  
			
			ret = select(fdmax + 1, &tmp_fds, NULL, NULL, NULL);
			DIE(ret < 0, "select");

			for (int i = 0; i <= fdmax; i++) {	
				if (FD_ISSET(i, &tmp_fds)) {

					if (i != 0) {
						if (i == sockfd) {
							connect_TCP();
						} else if (i == sock) {
							handle_UDP();
						} else {
							recv_TCP(i);
						}
					} else {
						memset(buffer, 0 , BUFLEN);
	                    fgets(buffer, BUFLEN-1, stdin);
	                    if (strcmp(buffer, "exit") == 0) {
	                    	exit(1);
                    	} 	
                    }
				}
			}
		}

		close(sockfd);
		close(sock);
	}

// --------------------------------------------------------------------------------------------------
	/**
	 * Generating random values to simulate the stock market 
	 * changes for a specific stock or good
	 * 
	 * */

	double fRand(double fMin, double fMax){
		double f = (double)rand() / RAND_MAX;
		return fMin + f * (fMax - fMin);
	}

	void generate_prices(Client c){
		
		int pos = 0;
		while (true){
			APPL appl;
			appl.current_price = fRand(500.0f, 1000.0f);
			
			MSFT msft;
			msft.current_price = fRand(500.0f, 800.0f);

			NOKIA nokia;
			nokia.current_price = fRand(400.0f, 600.0f);

			GOOG goog;
			goog.current_price = fRand(500.0f, 1500.0f);


			ETF etf;
			etf.current_price = (nokia.current_price + 
								appl.current_price + 
								goog.current_price + 
								msft.current_price )/ 4;

			BOND bond;
			bond.current_price = fRand(90.0f, 115.0f);
			string to_send;
			const char *msg;
			int s;
			to_send = to_string(pos) + "," + to_string(appl.current_price);
			pos++;
			msg = to_send.c_str();
			s = send(c.client_id, msg, strlen(msg), 0);
			DIE(s < 0, "send");	
			
		}
	}

// --------------------------------------------------------------------------------------------------
	
	void connect_TCP() {
		// a venit o cerere de conexiune pe socketul inactiv (cel cu listen),
		// pe care serverul o accepta

		clilen = sizeof(cli_addr);
		newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
		DIE(newsockfd < 0, "accept");

		// se adauga noul socket intors de accept() la multimea descriptorilor de citire
		FD_SET(newsockfd, &read_fds);  

		Client c;
		c.client_id = newsockfd;
		c.status = true;
		clients.push_back(c);		

	    if (newsockfd > fdmax) { 
			fdmax = newsockfd;
		}

		cout << "New client " 
			 << newsockfd 
			 << " connected from " 
			 << inet_ntoa(cli_addr.sin_addr) << ":" 
			 << ntohs(cli_addr.sin_port) << "\n";

		generate_prices(c);		
	}


	

	void handle_UDP(){
		int s = recvfrom(sock, buffer, BUFLEN, 0,(struct sockaddr*)&from_station, &clilen);
		DIE(s < 0, "upd client is not able to send -> handle_UDP()");
		
		// facem o mapare a continutului din buffer la structura
		// data de protocol pentru un mesaj de tip udp 
		udp_client_message udp_msg = *((udp_client_message*)buffer);

		string topic(udp_msg.topic);
		string host = inet_ntoa(from_station.sin_addr);
		string port = to_string(ntohs(from_station.sin_port));
		string to_send = process(udp_msg.content, udp_msg.type);

		to_send = host + ":" + port + 
				  " - " + topic + to_send;

		
		store.push_back(make_pair(topic, to_send));

        
		for (int i = 0; i < clients.size(); i++) {
			// verific daca un client are in lista de topicuri
			// topicul curent
			int sf;
			bool has_topic = false;
			Client current_client = clients[i];
			for (int j = 0; j < current_client.database.size(); j++) {
			if (current_client.database[j].first == topic) {
				has_topic = true;
				sf = current_client.database[j].second;
				}
			}

			if (clients[i].status == true) {
				if (has_topic) {
					const char* msg = to_send.c_str();
					int s = send(clients[i].client_id, msg, strlen(msg), 0);
			   		DIE(s < 0, "send");	
		   		}

		   		// daca clientul mai are in coada mesaje mai vechi
		   		// i le trimit in ordinea postarii
		   		for (pair<string, string> e : current_client.old_messages)
			   			if (e.first == topic) {
			   			const char* old_msg = e.second.c_str();
						int s = send(clients[i].client_id, old_msg, strlen(old_msg), 0);
				   		DIE(s < 0, "send");
			   		}
		   		

	   		} else {
	   				// daca nu este activ dar este abonat cu store and forward 1
	   				// retin mesajele intr-o coada 
	   				if(has_topic && (sf == 1)) {
	   					current_client.old_messages.push_back(make_pair(topic, to_send));
	   				}
	   			}
		}
		
	}

	

	void recv_TCP(int client_id) {
		// s-au primit date pe unul din socketii de client TCP 
		// asa ca serverul trebuie sa le receptioneze
		memset(buffer, 0, BUFLEN);
		n = recv(client_id, buffer, sizeof(buffer), 0);
		DIE(n < 0, "recv");

		if (n == 0) {
			// conexiunea s-a inchis
			cout << "Client " << client_id << " disconnected\n";
			close(client_id);

			for (Client c : clients) 
				if (c.client_id  == client_id) 
					c.status = false;

			// se scoate din multimea de citire socketul inchis 
			FD_CLR(client_id, &read_fds);
		} else {		
			string command(buffer);
			vector<string> strings;
    		istringstream f(command);
    		string s;    
   			 while (getline(f, s, ' ')) {
        		 strings.push_back(s);
   			 }
 			
 			if(strings.size() >= 2) {

 				// caut index-ul clientului 
   				int client_index;
	   			for (int i = 0; i < clients.size(); i++){
	   				if (clients[i].client_id == client_id) {
	   					client_index = i;
	   				}
	   			}

	   			// parsez comanda 
	   			if(strings[0] == "subscribe") {
	   				stringstream convertor(strings[2]);
	   				int sf;
	   				convertor >> sf;
	   				bool exist = false;

	   				// verific daca exista un topic cu acest nume 
	   				for (pair<string, string> p : store) {
	   					if (p.first == strings[1]){
	   						exist = true;
	   					}
	   				}
	   				
	   				if (exist == true) { 
	   					// daca exista il abonez
		   				clients[client_index].database.push_back(make_pair(strings[1], sf));
		   				if (sf ==  1) {
		   					vector<string> v;
		   					for (pair<string, string> e : store) {
		   						if(e.first == strings[1]) {
		   							v.push_back(e.second);
		   						}
		   					}
		   					// daca au mai fost postate mesaje il tin la curent
		   					if (v.size() != 0){
		   						int s = send(client_id, "Lately on that topic: ", 30, 0);
		   						DIE(s < 0, "send");
		   						for(string str : v) {
		   							const char* msg = str.c_str();
		   							int s = send(client_id, msg, strlen(msg), 0);
		   							DIE(s < 0, "send");
		   							s = send(client_id, "\n", 3, 0);
					   				DIE(s < 0, "send");	
		   						}
		   					}
		   				}
	   				} else {
	   					int s = send(client_id, "No topic with that name!", 50, 0);
		   				DIE(s < 0, "send");
	   				}

	   			} else if(strings[0] == "unsubscribe") {
	   				bool has_topic = false;
	   				for (int i = 0; i < clients[client_index].database.size(); i++){
	   					if (clients[client_index].database[i].first == strings[1])
	   						has_topic = true;
	   				}

	   				if (has_topic) {
	   					int index;
						for (int i = 0; i < clients[client_index].database.size(); i++) {
							if(clients[client_index].database[i].first == strings[1]) {
								index = i;
								clients[client_index].database.erase(clients[client_index].database.begin() + 
																	 index, 
																	 clients[client_index].database.begin() + 
																	 index + 1);
								break;
							}
						}

	   				} else{
	   					int s = send(client_id, "There is no such topic in your list!", 60, 0);
	   					DIE(s < 0, "send");
	   				} 
	   			} else {
	   				int s = send(client_id, "Wrong command!", 30, 0);
	   				DIE(s < 0, "send");
	   			}	

   			} else {
   				int s = send(client_id, "Wrong command!", 30, 0);
	   			DIE(s < 0, "send");
   			}
   		}
 	}

 	string process(char content[1500], unsigned char type) {

 		// ASCII 0 -> INT
 		// ASCII 1 -> SHORT_REAL
 		// ASCII 2 -> FLOAT
 		// ASCII 3 -> STRING

 		type = type + 48;  // ASCII 0 -> fac o conversie
 		switch(type) {
 			case 48 : {
 				type_int new_int = *((type_int *)content);
                new_int.value = ntohl(new_int.value);
 				string result_int = to_string(new_int.value);
 				if (new_int.sign + 48 == '0'){
 				  result_int = " - INT - " + result_int;
 				  return result_int;
 				} else {
 			      result_int = " - INT - -" + result_int;
 				  return result_int;
 				}
 			}

 			case 49 : {
 				type_short_real new_short_real = *((type_short_real*)content);
 				new_short_real.value = ntohs(new_short_real.value);
 				
 				// floating point de doua zecimale 
 				int whole = new_short_real.value / 100;
 				int fractional = new_short_real.value % 100; 

 				string result_short = to_string(whole) + "." + to_string(fractional);
 				result_short = " - SHORT_REAL - " + result_short;
 				return result_short;
 			}

 			case 50 : {
 				type_float new_float = *((type_float*)content);
 				new_float.whole_part_and_fractional_part = 
 					ntohl(new_float.whole_part_and_fractional_part);
 				int value = new_float.whole_part_and_fractional_part;
				int power = new_float.comma_movement;
				
				int whole_part = value / pow(10,power);
				int fractional_part = value % (int)pow(10, power);  
				
 				string result_float = to_string(whole_part) + "." + 
 									  to_string(fractional_part);

 				if (new_float.sign + 48 == '0') {	
 					result_float = " - FLOAT - " + result_float;
 					return result_float;
 				} else {
 					result_float = " - FLOAT - -" + result_float;
 					return result_float;
 				}

 			}

 			case 51 : {
 				type_string new_string = *((type_string*)content);
 				string result_str = new_string.content;
 				result_str  = " - STRING - " + result_str;
 				return result_str;
 			}

 			default:
 				break;
 		}

  	}

  
};