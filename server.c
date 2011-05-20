#include "server.h"

int robot_send_status(robot* bot, time_t time, float retval)
{
	status_packet sp;
	memcpy(&sp,bot,sizeof(float)*4 + sizeof(uint16)*2);
//	printf("sending status %0.04f\n",retval);
	sp.time = time;
	sp.retval = retval;
	if(send(bot->sock,&sp,sizeof(status_packet),MSG_NOSIGNAL) < sizeof(status_packet))
		return -1;
	return 0;
}

int robot_kill(robot* bot, time_t time)
{
	bot->hitpoints = 0;
	if(bot->timeofdeath == 0)
		bot->timeofdeath = time;
	robot_send_status(bot, time, 0);
	close(bot->sock);
	return botlist_add(dead_bots, botlist_del(bots, bot));
}

float robot_turn_right(robot* bot, float rads)
{
	/*
	 *  The faster you go the less you can turn
	 */
	rads = rads *(1-(bot->vel/MAX_VEL));
	if(rads > MAX_TURN)
		rads = MAX_TURN;
	else if(rads < - MAX_TURN)
		rads = - MAX_TURN;
	bot->dir += rads;
	return rads;
}

float robot_turn_left(robot* bot, float degrees)
{
	return robot_turn_right(bot,-degrees);
}

float robot_accel(robot* bot, float accel)
{
	bot->vel += accel;
	if(bot->vel > MAX_TURN)
		bot->vel = MAX_TURN;
	else if(bot->vel < 0)
		bot->vel = 0;
	return bot->vel;
}

float robot_decel(robot* bot, float decel)
{
	return robot_accel(bot,-decel);
}

robot* robot_load(int sock)
{
	start_packet pac;
	robot* bot;
	if(sock<0)
		return NULL;

	if(fcntl(sock,F_SETFL, 0)<0){
		printf("robot_load: Socket blocking option setting failed\n");
		close(sock);
		return NULL;
	}
	if(recv(sock, &pac, sizeof(start_packet), 0) < sizeof(start_packet)) {
		perror("recv of start_packet");
		close(sock);
		return NULL;
	}
	if(VERSION != pac.version) {
		close(sock);
		return NULL;
	}
	pac.h = ARENA_HEIGHT;
	pac.w = ARENA_WIDTH;
	pac.r = ROBOT_RADIUS;
	pac.per = MESSAGES_PER;
	if(send(sock, &pac, sizeof(pac), 0) < sizeof(start_packet)) {
		perror("send of start_packet");
		close(sock);
		return NULL;
	}
	bot = new_robot(pac.name,sock);
	return bot;
}

int robot_read(robot* bot, msg_packet* msg)
{
	int ret;
	//msg->msg_type = 0; // zero message type
	if(bot->messagesleft > 0) {
		ret = recv(bot->sock, (void*)msg, (size_t)sizeof(msg_packet),0);
		//perror("robot_read::");
		//printf("read message of length: %d\n", ret);
		if(ret == -1) {
			perror("robot_read");
			printf("ret: %d\n", ret);
		} else if (ret == 0) { // orderly shutdown
			robot_kill(bot, time(NULL));
		} else if (ret < sizeof(msg_packet)) {
			printf("only read %d bytes...wtf?\n", ret);
		} else {
			bot->messagesleft--;
			return bot->messagesleft;
		}
	}
	return -1;
}

float robot_step(robot* bot, msg_packet* msg)
{
	float rv = 0.0;
	//printf("%s sent us message %c\n", bot->name, msg->msg_type);
	switch(msg->msg_type)
	{
		case 'R':	// R - rotate right 
			rv = robot_turn_right(bot, (float)msg->param);
			break;
		case 'L':	// L - rotate left
			rv = robot_turn_left(bot, (float)msg->param);
			break;
		case 'F':	// F - increase forward velocity
			rv = robot_accel(bot, (float)msg->param);
			//printf("robot_accel returned %f\n", rv);
			break;
		case 'B':	// B - decrease forward velocity
			rv = robot_decel(bot, (float)msg->param);
			//printf("robot_decel returned %f\n", rv);
			break;
		case 'X':	// X - fire!
			rv = robot_fire(bot);
			break;
		case 'S':	// S - Scan for enemies.
			rv = robot_scan(bot, (float)msg->param);
			break;
		case 'W':	// W - wait (noop)
			break;
		default:
			printf("Robot '%s' sent invalid message: '%c' (%d)\n", 
					bot->name,msg->msg_type, msg->msg_type);
			robot_kill(bot, time(NULL));
			return 1.0/0.0;
	}
	return rv;
}
void robots_step()
{
	int i;
	float ret;
	msg_packet msg;
	for(i=0;i<bot_count;i++) {
		if(bots[i]->hitpoints <= 0) {
			robot_kill(bots[i--],time(NULL));
			continue;
		}
		if((ret = robot_step(bots[i], &msg)) != -2.0)
			robot_send_status(bots[i], time(NULL), ret);
	}
}

float distance(robot* bot1, robot* bot2)
{
	float x = bot1->x - bot2->x, y = bot1->y - bot2->y;
	return sqrt( x*x + y*y);
}

float angleto(robot* bot1, robot* bot2)
{
	float x = bot2->x - bot1->x, y = bot1->y - bot2->y;
	return atan2f(-y,x);
}

float robot_scan(robot* bot, float rads)
{
	int i,j=0;
	float dist, d;
	if(rads <= 0.0) {
		rads = 0.0;
		dist = MAX_SCAN;
	} else {
		if(rads >= M_PI*2)
			rads = M_PI*2;
		dist = (MAX_SCAN-2*ROBOT_RADIUS)/(rads/M_PI*2)+2*ROBOT_RADIUS;
	}
	for(i=0;i<bot_count;i++)
		if(bot == bots[i]) continue;
		else if((d = distance(bot,bots[i])) <= dist)
			if(angleto(bot,bots[i])<rads) {
				j = 1;
				dist=d;
			}
	if(j == 1) return dist;
	return -1.0;
}

/*
 *  -1 is miss
 */
float shot_hit(robot* src, robot* dst)
{
	float dist = distance(src,dst);
	float ang = angleto(src,dst) - src->dir;
	float hit;
	//if(ang == 0.0)
	//	return ang;
	if(ang < 0.0)
		ang = -ang;
	hit = dst->r / dist -ang;
	if(hit < 0.0)
		return -1.0;
	return hit;
}
float robot_fire(robot* bot)
{
	int x = cos(bot->dir) * screen->h;
	int y = sin(bot->dir) * screen->h;
	SDL_drawLine_AA(screen, bot->x, bot->y, bot->x+x, bot->y+y, 0xff0000);
	int i, hits = 0;
	float d;
	for(i=0;i<bot_count;i++)
		if(bot == bots[i]) continue;
		else if((d = shot_hit(bot,bots[i])) > 0) {
			bots[i]->hitpoints--;
			if(bots[i]->hitpoints <= 0)
				robot_kill(bots[i],time(NULL));
			hits++;
		}
	if (hits>0) printf("fire hit %d times\n", hits);
	return (float)hits;
}
