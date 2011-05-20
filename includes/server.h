#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <SDL.h>
#include <SDL_ttf.h>
#include "SDL_prim.h"
#include "robot.h"
#include "robonet.h"
#include <unistd.h>
#include <math.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <errno.h>
//#include <sys/select.h>
#ifndef __SERVER_H
#define __SERVER_H 1

extern SDL_Surface *screen;
extern int VERSION;
extern int ARENA_HEIGHT;
extern int ARENA_WIDTH;
extern float ROBOT_RADIUS;
extern int MESSAGES_PER;
extern float MAX_SCAN;
extern int MAX_HITPOINT;
extern float MAX_VEL;
extern float MAX_TURN;
extern int MAX_BOTS;
extern TTF_Font* font;
extern int in_sock;
extern int port;

extern robot** bots;
extern robot** dead_bots;
extern int bot_count;
extern int dead_bot_count;

void move();
void draw ();
float angleto(robot* bot1, robot* bot2);
float distance(robot* bot1, robot* bot2);
robot* robot_load(int sock);
float robot_turn_left(robot* bot, float degrees);
float robot_turn_right(robot* bot, float degrees);
int robot_kill(robot* bot, time_t time);
int robot_send_status(robot* bot, time_t time, float retval);
float shot_hit(robot* src, robot* dst);
float robot_accel(robot* bot, float accel);
float robot_decel(robot* bot, float decel);
float robot_fire(robot* bot);
void robots_step();
float robot_step(robot* bot, msg_packet* msg);
float robot_scan(robot* bot, float rads);
void silly_bot();
int robot_read(robot* bot, msg_packet* msg);

int botlist_add(robot** list, robot* bot); //add a bot to a list
robot* botlist_del(robot** list, robot* bot); //del a bot from a list

int server_draw_setup(int argc, char** argv);
void server_draw_teardown();
int server_net_setup(int argc, char** argv);
void server_net_teardown();
void server_net_accept();
#endif
