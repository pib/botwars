#include <stdio.h>
#include <time.h>
#include <string.h>
#include <malloc.h>
#include <math.h>
#include "mytypes.h"
#include <SDL.h>
#include <SDL_prim.h>
#include <SDL_ttf.h>
#include "robonet.h"

#ifndef __ROBOT_H
#define __ROBOT_H 1

extern int VERSION;
extern int ARENA_HEIGHT;
extern int ARENA_WIDTH;
extern float ROBOT_RADIUS;
extern int MESSAGES_PER;
extern TTF_Font* font;
extern int MAX_HITPOINT;


// This is robot.h!
typedef struct {
        float x;
        float y;
        float dir;
        float vel;
	uint16 messagesleft;
	uint16 hitpoints;
	float r;
	int sock;
	time_t timeofdeath;
	uint32 color;
        char* name;
	msg_packet* msg_packet_queue;
	uint16 messages;
} robot;

robot* new_robot(char* name, int sock);
void draw_robot(SDL_Surface*, robot* bot);
void erase_robot(SDL_Surface* surf, robot* bot);
void move_robot(SDL_Surface* surf, robot* bot);
void del_robot(robot* bot);

#endif
