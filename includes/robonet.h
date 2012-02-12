#ifndef __ROBONET_H
#define __ROBONET_H 1

// Setup message from client to server
// Client should ONLY set version and name
typedef struct {
	uint32 version;
	uint32 h;		// in bots heigh
	uint32 w;		// in bots wide
	float r;		// the radius of the bot
	uint32 per;	// messages per round
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
	uint16 messagesleft;	// zero means round is over
	uint16 hitpoints;	// zero means dead
        uint32 time;
        float retval;
} status_packet;


#endif
