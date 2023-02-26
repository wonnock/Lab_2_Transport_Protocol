#include "Sim_Engine.h"

#include <strings.h> // strcpy

#define A 0
#define B 1

#define INCREMENT 20.0
#define BYTES 20

struct pkt *reference_packet;
struct pkt *last_pkt;

int arrived = 1;
int a_timer = 0;

/* Called from layer 5, passed the data to be sent to other side */
void A_output(struct msg message) {
  /* TODO */
  // Allocate packet size

  struct pkt *packet = (struct pkt *)malloc(sizeof(struct pkt));
  for (int i = 0; i < BYTES; i++) {
    // copy message data to packet payload
    packet->payload[i] = message.data[i];
  }

  // reference_packet->acknum = (reference_packet->acknum + 1) % 2;
  // reference_packet->seqnum = (reference_packet->seqnum + 1) % 2;

  packet->acknum = reference_packet->acknum;
  packet->seqnum = reference_packet->seqnum;
  packet->checksum = cksum((unsigned char *)packet->payload, BYTES);

  // Start timer
  if (!a_timer) {
    starttimer(A, INCREMENT);
    a_timer = 1;
  }

  if (arrived) {
    printf("\nNEW LAST PACKET: %c\n", message.data[0]);
    last_pkt = packet;
  }

  arrived = 0;
  // Send packet to Host B

  printf("HOST_A Packet %c seq: %d\tack: %d\tck: %d\n", message.data[0], reference_packet->seqnum,
         reference_packet->acknum, cksum((unsigned char *)message.data, BYTES));
  tolayer3(A, *packet);
}

/* Called from layer 3, when a packet arrives for layer 4 */
void A_input(struct pkt packet) {
  /* TODO */

  printf("packetAseqnum %d\n", packet.seqnum);
  
  arrived = 1;
  if (packet.checksum != cksum((unsigned char *)packet.payload, BYTES)) {
    perror("Checksum fault A");
    tolayer3(A, *last_pkt);
    // stoptimer(A);
  }

  if (packet.acknum != reference_packet->acknum) {
    perror("Acknum fault A");
    printf("A: %d\tB: %d\n", reference_packet->acknum, packet.acknum);
    tolayer3(A, *last_pkt);
  }

  if (packet.seqnum != reference_packet->seqnum) {
    perror("Seqnum fault A");
    printf("A: %d\tB: %d\n", reference_packet->seqnum, packet.seqnum);
    tolayer3(A, *last_pkt);
  }

  if (a_timer) {
    stoptimer(A);
    a_timer = 0;
  }

  reference_packet->acknum = (reference_packet->acknum + 1) % 2;
  reference_packet->seqnum = (reference_packet->seqnum + 1) % 2;

  // Check for corruption, ack and seq
}

/* Called when A's timer goes off */
void A_timerinterrupt() {
  /* TODO */
  printf("A TIMEOUT\n");
  a_timer = 0;
  tolayer3(A, *last_pkt);
}

/* The following routine will be called once (only) before any other */
/* Host A routines are called. You can use it to do any initialization */
void A_init() {
  /* TODO */
  reference_packet = (struct pkt *)malloc(sizeof(struct pkt));
  reference_packet->acknum = 0;
  reference_packet->seqnum = 0;

  last_pkt = (struct pkt *)malloc(sizeof(struct pkt));
}
