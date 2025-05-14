#include "main.h"
#include <arpa/inet.h>
#include <errno.h>
#include <netinet/ip.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>

uint16_t get_checksum(uint8_t *buffer) {
  return 0xffff - buffer[ICMP_CODE_INDEX] - buffer[ICMP_TYPE_INDEX];
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

void create_packet_buffer(uint8_t *buffer, size_t buffer_size) {
  buffer[ICMP_TYPE_INDEX] = 0x08;
  buffer[ICMP_CODE_INDEX] = 0x00;
  buffer[ICMP_CHECKSUM_INDEX] = 0;
  buffer[ICMP_CHECKSUM_INDEX + 1] = 0;
  uint16_t checksum_result = calculate_checksum(buffer, buffer_size);
  buffer[ICMP_CHECKSUM_INDEX] = checksum_result >> 8;
  buffer[ICMP_CHECKSUM_INDEX + 1] = checksum_result;
}

int fill_args(int argc, const char *argv[], uint32_t *destination_ip,
              int *packet_size) {

  // Check if '--help' was the first arg
  if (memcmp(argv[1], "--help", strlen(argv[1])) == 0) {
    PRINT_STR("Help Prompt");
    return 1u;
  }

  // Check if too many args
  if (argc > 3) {
    PRINT_STR("Too many args");
    return 1u;
  }

  // Check if no args passed
  if (argc == 0) {
    PRINT_STR("Must pass host target ip");
    return 1u;
  }

  // Check the payload size
  if (argv[2] != NULL) {
    *packet_size = atoi(argv[2]);
    if (*packet_size > PACKET_MAX_LENGTH) {
      PRINT_STR("Packet size is too large");
      return 1u;
    }
  }
}

int main(int argc, char *argv[]) {
  int payload_size = 0;
  uint32_t destination_ip = 0;

  if (fill_args(argc, argv, &destination_ip, &payload_size) != 0) {
    return 1u;
  }

  uint8_t buffer[PACKET_MAX_LENGTH] = {0};
  create_packet_buffer(buffer, payload_size + PACKET_MIN_LENGTH);

  uint16_t port = 0;
  sa_family_t net_type = AF_INET;

  uint32_t DESTINATION_IP = inet_addr(argv[1]);

  struct sockaddr_in serv_addr;
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = DESTINATION_IP;

  int sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);

  if (sockfd < 0) {
    printf("Socket Error:%d:%d\n", sockfd, errno);
    return 1;
  }

  int send_status = -2;
  printf("\nSending Now. Press ctrl+c to stop.\n");
  while (1) {
    send_status = sendto(sockfd, buffer, payload_size + PACKET_MIN_LENGTH, 0,
                         (struct sockaddr *)&serv_addr, 32);
    if (send_status < 0) {
      printf("Error with Sending to Socket:%d:%d\n", send_status, errno);
      return 1u;
    }
  }

  printf("\nFinished\n");
  return 0;
}
