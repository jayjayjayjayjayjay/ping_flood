#include <sys/socket.h>
#include <netinet/ip.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include "main.h"



//position in header
//#define DESTINATION_IP 192.168.20.1
//#define SOURCD_IP 192.168.20.16

//position in icmp header


//payload index
//
//
//ip_header = b'\x45\x00\x00\x1c' # Version, IHL, Type of Service | Total Length
//ip_header += b'\xab\xcd\x00\x00' # Identification | Flags, Fragment Offset
//ip_header += b'\x40\x01\x6b\xd8' # TTL, Protocol | Header Checksum
//ip_header += b'\xc0\xa8\x92\x83' # Source Address
//ip_header += b'\x08\x08\x08\x08' # Destination Address

//icmp_header = b'\x08\x00\xe5\xca' # Type of message, Code | Checksum
//icmp_header += b'\x12\x34\x00\x01' # Identifier | Sequence Number`k_

void u32_to_bytes(uint8_t * bytes,uint32_t value){
		bytes[0] = value >> 24;
		bytes[1] = value >>  16;
		bytes[2] = value >>  8;
		bytes[3] = value;

}
bool create_packet_buffer(uint8_t * buffer){
		uint32_t DESTINATION_IP = htonl(inet_addr("192.168.20.1"));
		uint32_t SOURCE_IP = htonl(inet_addr("192.168.20.16"));
		uint8_t dest_ip_buffer[4],source_ip_buffer[4]; 
		u32_to_bytes(&dest_ip_buffer,DESTINATION_IP);
		u32_to_bytes(&source_ip_buffer,SOURCE_IP);
		buffer[VERSION_INDEX]= 0x45;
		buffer[LENGTH_INDEX+1]= 0x1c;
		buffer[IDENTIFICATION_INDEX]= 0xab;
		buffer[FLAGS_AND_OFFSETS_INDEX]= 0xcd;
		buffer[TTL_INDEX]= 0x40;
		buffer[PROTOCOL_INDEX]= 0x01;
		//buffer[HEADER_CHECKSUM_INDEX] =  checksum(buffer,10);
		memcpy(&buffer[SOURCE_IP_INDEX],&source_ip_buffer,4);
		memcpy(&buffer[DESTINATION_IP_INDEX],&dest_ip_buffer,4);
}

int main(){
uint32_t DESTINATION_IP =inet_addr("192.168.20.6");// ||htons(inet_addr("192.168.20.1")) << 16 ;
uint32_t SOURCE_IP = htonl(inet_addr("192.168.20.16"));
uint8_t dest_ip_buffer[4],source_ip_buffer[4],dest_reverse; 
u32_to_bytes(&dest_ip_buffer,DESTINATION_IP);
u32_to_bytes(&source_ip_buffer,SOURCE_IP);
printf("ip:%d",DESTINATION_IP);
uint16_t port = 0;
sa_family_t net_type = AF_INET;
//struct in_addr destination_ip_addr;
//destination_ip_addr.s_addr = DESTINATION_IP;
//rename to something better for var
struct sockaddr_in serv_addr;
serv_addr.sin_family = AF_INET;
serv_addr.sin_addr.s_addr = DESTINATION_IP;
//memcpy(&serv_addr.sa_data,&dest_ip_buffer,4 );
printf("\n");
//serv_addr.sa_data = dest_ip_buffer;
//serv_addr.sa_data[0]=4;
for (int i = 0; i < 4; i++){
//		printf("\nSERV:%u",serv_addr.sa_data[i]);
		printf("\nDEST:%u",dest_ip_buffer[i]);

}// printf("\n");

//strcpy(serv_addr.,"127.0.0.1");
//serv_addr.sin_port = htons(port);
//struct sockaddr_in {     
//	sa_family_t net_type;   // e.g. AF_INET, AF_INET6
//	in_port_t port;
 //   struct in_addr destination_ip_addr;     // see struct in_addr, below
//} host_socket ;

//if (status == -1){
//printf("Failed Creating Socket:%d",status);
//}
uint8_t buffer[40];
create_packet_buffer(&buffer);
for (int i = 0; i < 20; i++){
printf("%x\n",buffer[i]);

}
int sockfd = socket(AF_INET,SOCK_RAW,IPPROTO_ICMP); //ICMP=1 in /etc/sockets
if (sockfd < 0){
printf("Socket Error:%d:%d\n",sockfd,errno);
}

printf("size: %u",sizeof(serv_addr));
int send_status = -2;
send_status = sendto(sockfd,&buffer,sizeof(buffer),0,(struct sockaddr*)&serv_addr,32);


//bool send_status = send(sockfd,buffer,sizeof(buffer),NULL);
if (send_status < 0){
		printf("Error with Sending to Socket:%d:%d\n",send_status,errno);

}

return 1;


}
