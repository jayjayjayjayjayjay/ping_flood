#ifndef MAIN
#define MAIN

#include <stdio.h>

#define PRINT_STR(x) printf("\n%s\n", x)
#define CLI_ARGUMENTS 2

#define PING_MIN_LENGTH 24u

#define PACKET_MIN_LENGTH 8
#define PACKET_MAX_LENGTH 8511u

#define ICMP_v4_INDEX = 4

#define ECHO_REQUEST_HEADER_BIT = 8;

#define ICMP_TYPE_INDEX 0
#define ICMP_CODE_INDEX 1
#define ICMP_CHECKSUM_INDEX 2
#define ICMP_IDENTIFIER_INDEX 4
#define ICMP_SEQUENCE_NUMBER_INDEX 6
#define ICMP_PAYLOAD_INDEX 8

#endif
