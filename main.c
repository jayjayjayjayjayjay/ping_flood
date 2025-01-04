#include "main.h"
#include <errno.h>
#include <netinet/ip.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>

// position in header
//#define DESTINATION_IP 192.168.20.1
//#define SOURCD_IP 192.168.20.16

// position in icmp header

// payload index
//
//
// ip_header = b'\x45\x00\x00\x1c' # Version, IHL, Type of Service | Total
// Length ip_header += b'\xab\xcd\x00\x00' # Identification | Flags, Fragment
// Offset ip_header += b'\x40\x01\x6b\xd8' # TTL, Protocol | Header Checksum
// ip_header += b'\xc0\xa8\x92\x83' # Source Address
// ip_header += b'\x08\x08\x08\x08' # Destination Address

// icmp_header = b'\x08\x00\xe5\xca' # Type of message, Code | Checksum
// icmp_header += b'\x12\x34\x00\x01' # Identifier | Sequence Number`k_
uint16_t get_checksum(uint8_t *buffer) {
  return 0xffff - buffer[ICMP_CODE_INDEX] -
         buffer[ICMP_TYPE_INDEX]; // - buffer[IDENTIFICATION_INDEX];//flags
                                  // offsets left out as 0
}
uint16_t calculate_checksum(unsigned char *buffer, int bytes) {
  uint32_t checksum = 0;
  unsigned char *end = buffer + bytes;

  // odd bytes add last byte and reset end
  if (bytes % 2 == 1) {
    end = buffer + bytes - 1;
    checksum += (*end) << 8;
  }

  // add words of two bytes, one by one
  while (buffer < end) {
    checksum += buffer[0] << 8;
    checksum += buffer[1];
    buffer += 2;
  }

  // add carry if any
  uint32_t carray = checksum >> 16;
  while (carray) {
    checksum = (checksum & 0xffff) + carray;
    carray = checksum >> 16;
  }

  // negate it
  checksum = ~checksum;

  return checksum & 0xffff;
}

void u32_to_bytes(uint8_t *bytes, uint32_t value) {
  bytes[0] = value >> 24;
  bytes[1] = value >> 16;
  bytes[2] = value >> 8;
  bytes[3] = value;
}
bool create_packet_buffer(uint8_t *buffer, size_t buffer_size) {
  buffer[ICMP_TYPE_INDEX] = 0x08;
  buffer[ICMP_CODE_INDEX] = 0x00;
  buffer[ICMP_CHECKSUM_INDEX] = 0;
  buffer[ICMP_CHECKSUM_INDEX + 1] = 0;
  uint16_t checksum_result = calculate_checksum(buffer, buffer_size);
  buffer[ICMP_CHECKSUM_INDEX] = checksum_result >> 8;
  buffer[ICMP_CHECKSUM_INDEX + 1] = checksum_result;
}

int main(int argc, char *argv[]) {
  if (argc != CLI_ARGUMENTS + 1) {
    printf("Please pass destination ip and payload length\n");
    printf("argc of :%u\n", argc);
    printf("argv of :%s\n", argv[1]);
    return 1u;
  }

  uint8_t buffer[PACKET_MIN_LENGTH];
  create_packet_buffer(buffer, sizeof(buffer));

  uint16_t port = 0;
  sa_family_t net_type = AF_INET;

  // uint32_t DESTINATION_IP = inet_addr("192.168.1.1");
  uint32_t DESTINATION_IP = inet_addr(argv[1]);

  struct sockaddr_in serv_addr;
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = DESTINATION_IP;

  int sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP); // ICMP=1 in
                                                        // /etc/sockets
  if (sockfd < 0) {
    printf("Socket Error:%d:%d\n", sockfd, errno);
    return 1;
  }

  printf("size: %u", sizeof(serv_addr));
  int send_status = -2;
  // for (int i = 0; i < 10; i++) {
  printf("\nSending Now. Press ctrl+c to stop.\n");
  while (1) {
    send_status = sendto(sockfd, buffer, sizeof(buffer), 0,
                         (struct sockaddr *)&serv_addr, 32);
    if (send_status < 0) {
      printf("Error with Sending to Socket:%d:%d\n", send_status, errno);
      return 1u;
    }
  }

  // bool send_status = send(sockfd,buffer,sizeof(buffer),NULL);
  printf("\nFinished\n");
  return 0;
}
