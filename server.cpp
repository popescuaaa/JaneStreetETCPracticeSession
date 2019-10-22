// Popescu Andrei-Gabriel 323Ca
// Implementarea serverului

#include "server.h"

int main(int argc, char const *argv[]){
	int portno = atoi(argv[1]); // numarul portului pe care va rula serverul
	Server server(portno);
	server.run();
	return 0;
}
