#ifndef TCP_H
#define TCP_H

#include <stdint.h>
#include "net.h"

typedef enum {
    TCP_STATE_CLOSED,
    TCP_STATE_LISTEN,
    TCP_STATE_SYN_SENT,
    TCP_STATE_SYN_RECEIVED,
    TCP_STATE_ESTABLISHED,
    TCP_STATE_FIN_WAIT_1,
    TCP_STATE_FIN_WAIT_2,
    TCP_STATE_CLOSE_WAIT,
    TCP_STATE_CLOSING,
    TCP_STATE_LAST_ACK,
    TCP_STATE_TIME_WAIT
} tcp_state_t;

typedef struct {
    uint8_t remote_ip[4];
    uint16_t local_port;
    uint16_t remote_port;
    tcp_state_t state;
    uint32_t seq;
    uint32_t ack;
    uint32_t remote_seq;
    uint32_t remote_ack;
} tcp_conn_t;

void tcp_init();
void tcp_connect(const uint8_t* ip, uint16_t port);

#endif
