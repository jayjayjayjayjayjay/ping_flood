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

uint16_t calculate_checksum(uint8_t *buffer, size_t buffer_size) {
  uint32_t checksum = 0u;

  for (size_t idx = 0u; idx < 10u; idx++) {
    if (idx == 5) {
      continue;
    }
    checksum += (buffer[idx * 2] << 8) | buffer[idx * 2 + 1];
  }

  checksum = ((checksum & 0xf0000) >> 16) + (checksum & 0xffff);
  return ~((uint16_t)checksum);
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
  return 0u;
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

    // TODO check if errno is same across mac osx and linux
    // TODO Some errors can be ignored but need to write logic for that
    if (send_status < 0) {
      printf("Error with Sending to Socket:%d:%d\n", send_status, errno);
      return 1u;
    }
  }

  printf("\nFinished\n");
  return 0;
}
