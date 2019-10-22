/*
		Popescu Andrei-Gabriel 323CA
		Implementare pentru structurile care definesc mesajele 

		Am ales sa folosesc structuri pentru ca
		este mult mai usor sa faci mapare de la una la alta 
		facand apel la pointeri

		Comform enuntului mesajul este standard si trebuie
		sa respecte urmatoare structura 
		topic -> char array[maximum 50] 
		type -> un unsigned char
		contentul -> char array[maximium 1500]
		am ales sa folosec char array si nu std::string pentru ca apar uneori probleme
		
		*pentru conversii: 
		
		The htonl() function converts the unsigned integer hostlong from host byte order to network byte order.
		
		The htons() function converts the unsigned short integer hostshort from host byte order to network byte order.

		The ntohl() function converts the unsigned integer netlong from network byte order to host byte order.

		The ntohs() function converts the unsigned short integer netshort from network byte order to host byte order.

		****uint8_t is a typedef in c++ for unsigned char (asta ar fi trebuit precizat in enunt..zic)****
*/


struct __attribute__((packed)) udp_client_message {
	char topic[50];
	unsigned char type;
	char content[1500];
};

// toate cele mai jos vor fi mapate la content
struct __attribute__((packed)) type_int{
	unsigned char sign;
	uint32_t value;
};

struct __attribute__((packed)) type_short_real {
	uint16_t value;
};

struct __attribute__((packed)) type_float {
	unsigned char sign;
	uint32_t whole_part_and_fractional_part;
	unsigned char comma_movement;
};

struct __attribute__((packed)) type_string {
	char content[1500];
};