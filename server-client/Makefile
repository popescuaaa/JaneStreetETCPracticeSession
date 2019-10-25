CFLAGS = -Wall -g

# Portul pe care asculta serverul (de completat)
PORT=5004
#Id-ul cleintului -> nu este necesar pentru identificare 
ID=1
# Adresa IP a serverului (de completat)
IP_SERVER=127.0.0.1 

all: server subscriber

# Compileaza server.c
server: server.cpp

# Compileaza client.c
client: subscriber.cpp

.PHONY: clean run_server run_subscriber

# Ruleaza serverul
run_server:
	./server ${PORT}

# Ruleaza clientul
run_subscriber:
	./subscriber ${ID} ${IP_SERVER} ${PORT} 

clean:
	rm -f server subscriber
