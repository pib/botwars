#include "server.h"

	
//Global stuff
SDL_Surface *screen = NULL;
TTF_Font* font = NULL;

int VERSION = 1;
int ARENA_HEIGHT = 30;
int ARENA_WIDTH = 40;
float ROBOT_RADIUS = 10;
int MESSAGES_PER = 1;
float MAX_SCAN;
float MAX_VEL = 10;
float MAX_TURN = M_PI/2.0;
int MAX_HITPOINT = 10;

int MAX_BOTS = 32;

robot** bots;
robot** dead_bots;
int bot_count;
int dead_bot_count;

// Local stuff
int in_sock; // our inbound socket
int port = 42000;

int server_setup(int argc, char** argv);
void server_teardown();
void draw_startup_message(); // draw a status while waiting to start...

int
main (int argc, char *argv[])
{
	
	int done, running;
	SDL_Event event;
	SDLKey key;
	server_setup(argc, argv);

	running = done = 0;
	while (!done)
	{

		/* Check for events */
		while (SDL_PollEvent (&event))
		{
			switch (event.type)
			{
			case SDL_KEYDOWN:
				key = event.key.keysym.sym;
				if (key == SDLK_q || key == SDLK_ESCAPE) {
					done = 1;
				} else if (key == SDLK_SPACE) {
					printf("Now would be the time to start running.\n");
					running = 1;
				}
				break;
			case SDL_QUIT:
				done = 1;
				break;
			default:
				break;
			}
		}
		if (running) {
			//do run stuff here...update bots, get messages, etc...
			// HERE IS WHERE IT ALL HAPPENS!!!
			// DO WHILE LOOP UNTIL ROUND MESSAGES...or time out...
			int i, t;
			msg_packet msg;
			float ret;
			for(i=0;i<bot_count;i++) {
				t = robot_read(bots[i], &msg); 
				if(t < 0) continue;
				//printf("t: %d\n",t);
				ret = robot_step(bots[i], &msg);
				if (ret != 1.0/0.0)
					robot_send_status(bots[i],time(NULL),ret);
				

				// Horrible, remove later
				bots[i]->messagesleft++;
			}

		} else {
			//get bot connections, set them up, etc...
			silly_bot();
			draw_startup_message();
			server_net_accept();

		}
		// Draw to screen 
		move();
	//	static int count = 0;
	//char filename[1024];
	//sprintf(filename, "ss%08d", count);
	//SDL_SaveBMP(screen, filename);
	//count++;

		draw();
	}

	TTF_CloseFont(font);

	return 0;
}

int server_setup(int argc, char** argv) {
	
	MAX_SCAN = sqrt(ARENA_HEIGHT*ARENA_HEIGHT+ARENA_WIDTH*ARENA_WIDTH)/2.0f;

	bots = (robot**) malloc(sizeof(robot*)*MAX_BOTS);
	dead_bots = (robot**) malloc(sizeof(robot*)*MAX_BOTS);

	dead_bot_count = 0;
	bot_count = 0;


	

	srandom(time(NULL)); // randomize for whatever needs it..

	server_draw_setup(argc,argv);
	server_net_setup(argc,argv);
	return 0;	
}

void server_teardown()
{
	server_draw_teardown();
	server_net_teardown();
	int i;
	for(i=0;i<bot_count;i++)
		del_robot(bots[i]);
	free(bots);
	for(i=0;i<dead_bot_count;i++)
		del_robot(dead_bots[i]);
	free(dead_bots);
}

int botlist_add(robot** list, robot* bot) {
	int* count;
	if (list == bots) count = &bot_count;
	else if (list == dead_bots) count = &dead_bot_count;
	else return -1;
	
	if (*count == MAX_BOTS) return -1;
	
	list[(*count)++] = bot;
	return 0;
}
	
robot* botlist_del(robot** list, robot* bot) {
	int* count;
	if (list == bots) count = &bot_count;
	else if (list == dead_bots) count = &dead_bot_count;
	else return NULL;

	if (*count == 1) {
		(*count)--;
		return bot;
	}
	

	int i;
	for(i=0; i < *count; i++)
		if (list[i] == bot) {
			list[i] = list[--(*count)];
			return bot;
		}
	return NULL;
}	









void do_test_stuff() {
	int done;
	char name[16];
	robot* bot[9];
	for(done=0;done<9;done++) {
		sprintf(name, "Bot %d", done);
		bot[done]=new_robot(name,0);
		if(bot[done]->dir > M_PI)
			bot[done]->dir = bot[done]->dir - M_PI*2;
	}
	bot[0]->x = 32*4;
	bot[0]->y = 32*4;
	bot[1]->x = 32*4;
	bot[1]->y = 48*4;
	bot[2]->x = 32*4;
	bot[2]->y = 64*4;
	bot[3]->x = 48*4;
	bot[3]->y = 64*4;
	bot[4]->x = 64*4;
	bot[4]->y = 64*4;
	bot[5]->x = 64*4;
	bot[5]->y = 48*4;
	bot[6]->x = 64*4;
	bot[6]->y = 32*4;
	bot[7]->x = 48*4;
	bot[7]->y = 32*4;
	bot[8]->x = 48*4;
	bot[8]->y = 48*4;
	bot[8]->dir = 0.150;

	for(done=0;done<8;done++) {
		float t = shot_hit(bot[8],bot[done]);
		char n[20];
		(t >=0)?sprintf(n,"%0.04f",t*180/M_PI):sprintf(n,"MISS");
		printf("angleto(bot[%i],bot[%i])=%0.04f,%s\n",8,done,
			angleto(bot[8],bot[done])*180/M_PI,n);
	}
	for(done=0;done<9;done++)
		printf("bot[%i],%0.02f,%0.02f,%0.02f,%s\n",done,
			bot[done]->x,bot[done]->y,bot[done]->dir*180/M_PI,
			bot[done]->name);
	for(done=0;done<8;done++) 
		printf("bot[%i] %0.04f,%0.04f,%0.04f\n",done,bot[done]->x,bot[done]->y,bot[done]->dir);

}
/* this went into the loop near draw:

		int i;
		for(i=0;i<9;i++) {
			draw_robot(screen, bot[i]);
		}
		SDL_drawLine(screen,bot[8]->x,bot[8]->y,bot[8]->x + 300 * cos(bot[8]->dir),bot[8]->y + 300 * sin(bot[8]->dir),0xff00ff00);	
		*/
