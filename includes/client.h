#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <math.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#ifndef __CLIENT_H
#define __CLIENT_H 1
// Setup message from client to server
// Client should ONLY set version and name
typedef struct {
	int version;
	int h;		// in bots heigh
	int w;		// in bots wide
	float r;		// the radius of the bot
	int per;	// messages per round
	char name[128];
} start_packet;

// Message packet, sent from the client to the server each round.
typedef struct {
        char msg_type; // Type of message:
		// W - wait (noop)
             // R - rotate right 
             // L - rotate left
             // F - increase forward velocity
             // B - decrease forward velocity
             // X - fire!
             // S - Scan for enemies.
        float param;
} msg_packet;
        

// Status packet, which is returned by the server to the client after each message
typedef struct {
        float x;
        float y;
        float dir;
        float vel;
	short messagesleft;	// zero means round is over
	short hitpoints;	// zero means dead
        int time;
        float retval;
} status_packet;

int get_sock(struct sockaddr_in* sockaddr);
int local_sock(int port);
int send_start_packet(int sock, int version, char* name, start_packet* sp);
int send_message(int sock, char cmd, float arg, status_packet* stat_pac);

#endif
