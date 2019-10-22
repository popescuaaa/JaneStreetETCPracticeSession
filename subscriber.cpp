// Popescu Andrei-Gabriel 323CA 
// Implementarea clientului TCP care va primi mesaje de la server

#include "subscriber.h"

int main(int argc, char *argv[]){
    int portno = atoi(argv[3]);
    int ID = atoi(argv[1]);
    Subscriber subscriber(portno, argv[2], ID);
    subscriber.run();
    return 0;
}
