#include "Sim_Engine.h"

#define A 0
#define B 1

#define BYTES 20

struct pkt *Host_B_reference_packet;

/* Called from layer 5, passed the data to be sent to other side */
void B_output(struct msg message) { /* DON'T IMPLEMENT */
}

/* Called from layer 3, when a packet arrives for layer 4 */
void B_input(struct pkt packet) {
  /* TODO */

  printf("\nREACHED B HOST\n");

  struct pkt *ack_packet = (struct pkt *)malloc(sizeof(struct pkt));

  // Check for corruption, ack and seq

  if (packet.checksum != cksum((unsigned char *)packet.payload, BYTES)) {
    perror("Checksum fault B");
    return;
  }

  if (packet.seqnum != Host_B_reference_packet->seqnum) {
    perror("Seqnum fault B");
    printf("PACKET AT FAULT: %c seq: %d ref_seq: %d\n", packet.payload[0],
           packet.seqnum, Host_B_reference_packet->seqnum);
    return;
  }

  if (packet.acknum != Host_B_reference_packet->acknum) {
    perror("Acknum fault B");
    return;
  }

  ack_packet->acknum = Host_B_reference_packet->acknum;
  ack_packet->seqnum = Host_B_reference_packet->seqnum;
  ack_packet->checksum = cksum((unsigned char *)packet.payload, BYTES);
  for (int i = 0; i < BYTES; i++) {
    ack_packet->payload[i] = packet.payload[i];
  }

  // printf("HOST_B Packet seq: %d\tack: %d\tck: %d\n",
  // Host_B_reference_packet->seqnum, Host_B_reference_packet->acknum,
  // cksum((unsigned char *)packet.payload, BYTES));
  printf("ack_packet: ack %d seq %d\n", ack_packet->acknum, ack_packet->seqnum);
  tolayer3(B, *ack_packet);

  Host_B_reference_packet->acknum = (Host_B_reference_packet->acknum + 1) % 2;
  Host_B_reference_packet->seqnum = (Host_B_reference_packet->seqnum + 1) % 2;

  tolayer5(B, packet.payload);
}

/* Called when B's timer goes off */
void B_timerinterrupt() { /* TODO */
}

/* The following routine will be called once (only) before any other */
/* Host B routines are called. You can use it to do any initialization */
void B_init() {
  /* TODO */
  Host_B_reference_packet = (struct pkt *)malloc(sizeof(struct pkt));
  Host_B_reference_packet->acknum = 0;
  Host_B_reference_packet->seqnum = 0;
}
