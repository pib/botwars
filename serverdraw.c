#include "server.h"

void clamp(float* val, float min, float max) {
	if (*val < min)
		*val = min;
	else if (*val > max)
		*val = max;
}

void fix_collision(int bot) {
	int i;

	clamp(&(bots[bot]->x), bots[bot]->r+1, screen->w-bots[bot]->r-1);
	clamp(&(bots[bot]->y), bots[bot]->r+1, screen->h-bots[bot]->r-1);
	
	for (i=0; i<bot_count; i++) {
		if (i != bot) {
			int collision_distance = bots[bot]->r + bots[i]->r;
			int dist, adj;
			if ((dist = distance(bots[bot], bots[i])) < collision_distance){
				adj = (collision_distance - dist) / 2; // overlap amount
				float ang =  angleto(bots[bot], bots[i]);
				float x_adj = cos(ang);
				float y_adj = sin(ang);
				bots[i]->x += x_adj;
				bots[i]->y += y_adj;
				bots[bot]->x -= x_adj;
				bots[bot]->y -= y_adj;
			}
		}
	}
}
				
	
void move() {
	int i;
	for (i=0; i<bot_count; i++) {
		move_robot(screen, bots[i]);
		fix_collision(i);
	}
}
void
draw ()
{
	Uint32 color;
	SDL_Flip (screen);
	SDL_Delay (10);
	
	/* Create a black background */
	color = SDL_MapRGB (screen->format, 0, 0, 0);
	SDL_FillRect (screen, NULL, color);

	int i;
	for (i=0; i<dead_bot_count; i++) {
		dead_bots[i]->color = 0xff0000;
		draw_robot(screen, dead_bots[i]);
	}

	char msg[1024];
	SDL_Surface* buff;
	SDL_Rect pos;
	SDL_Color c = {255,0,0};

	sprintf(msg, "time: %ld", (long)time(NULL));

	buff = TTF_RenderText_Blended(font, msg, c);
	pos.y = (screen->h - buff->h);
	pos.x = (screen->w - buff->w) / 2; // center line horizontally
	SDL_BlitSurface(buff, NULL, screen, &pos);
	SDL_FreeSurface(buff);
		
	    
}

void silly_bot() {	
	
	static robot* bot = 0;
	if (bot == 0) {
		bot= new_robot("Welcome to Robot Battle Arena v0.0000000001", 0);
		bot->color = -1;
		bot->x = screen->w / 2;
		bot->y = screen->h / 2;
		bot->dir = -M_PI/2;
		bot->vel = 4;
	}
	
	bot->dir += (random()%30 - 15)*M_PI/180;
	if (bot->x+bot->r > screen->w) {
		bot->dir += M_PI/2;
		bot->x = screen->w - bot->r-1;
	}
	if (bot->x-bot->r < 0) {
		bot->x = bot->r+1;
		bot->dir += M_PI/2;
	}
	if (bot->y+bot->r > screen->h) {
		bot->dir += M_PI/2;
		bot->y = screen->h - bot->r-1;
	}
	if (bot->y+bot->r > screen->h || bot->y-bot->r < 0) {
		bot->dir += M_PI/2;
		bot->y = bot->r+1;
	}

	move_robot(screen, bot);
	//erase_robot(screen, bot);
	//draw_robot(screen, bot);
}



int server_draw_setup(int argc, char** argv)
{
		/* Initialize SDL */
	if (SDL_Init (SDL_INIT_VIDEO) < 0) {
		printf ("Couldn't initialize SDL: %s\n", SDL_GetError ());
		exit (1);
	}
	atexit (SDL_Quit);
	/* Set up fonts */
	if (TTF_Init()==-1) {
		printf("TTF_Init failed: %s\n", TTF_GetError());
		exit (2);
	}
	atexit (TTF_Quit);
	int size = atoi(argv[2]);
	font = TTF_OpenFont(argv[1], size);
	
	if (!font) {
		printf("Aww, no font! Error: %s\n", TTF_GetError());
		exit(2);
	}
	
	screen = SDL_SetVideoMode ( ARENA_WIDTH*ROBOT_RADIUS*2, ARENA_HEIGHT*ROBOT_RADIUS*2, 0, SDL_SWSURFACE | SDL_DOUBLEBUF);
	if (screen == NULL) {
		printf ("Couldn't set %dx%d video mode: %s\n", 
		(int)(ARENA_WIDTH*ROBOT_RADIUS*2), (int)(ARENA_HEIGHT*ROBOT_RADIUS*2),
		SDL_GetError ());
		exit (2);
	}
	SDL_WM_SetCaption ("Bot Server", NULL);

	return 0;

}

void server_draw_teardown()
{
	return;
}

void draw_startup_message() {
	/*
	   Waiting for robots to connect.
	   press the space bar to begin.
	   press 'q' to quit.
	*/
	char msg[1024];
	SDL_Surface* buff;
	SDL_Rect pos;
	SDL_Color color = {255,255,255};
	int lines = 4; // number of lines
	int y_skip = TTF_FontLineSkip(font); // amount to skip each line

	sprintf(msg, "%d robot%c connected so far.", bot_count, (bot_count==1)?' ':'s');

	pos.y = (screen->h - (y_skip * lines)) / 2; // center the block vertically
	buff = TTF_RenderText_Blended(font, "Waiting for robots to connect.", color);
	pos.x = (screen->w - buff->w) / 2; // center line horizontally
	SDL_BlitSurface(buff, NULL, screen, &pos);
	SDL_FreeSurface(buff);
	pos.y += y_skip;
	
	buff = TTF_RenderText_Blended(font, msg, color);
	pos.x = (screen->w - buff->w) / 2; // center line horizontally
	SDL_BlitSurface(buff, NULL, screen, &pos);
	SDL_FreeSurface(buff);
	pos.y += y_skip;

	buff = TTF_RenderText_Blended(font, "Press the space bar to begin.", color);
	pos.x = (screen->w - buff->w) / 2; // center line horizontally
	SDL_BlitSurface(buff, NULL, screen, &pos);
	SDL_FreeSurface(buff);
	pos.y += y_skip;

	buff = TTF_RenderText_Blended(font, "Press 'q' to quit.", color);
	pos.x = (screen->w - buff->w) / 2; // center line horizontally
	SDL_BlitSurface(buff, NULL, screen, &pos);
	SDL_FreeSurface(buff);
	pos.y += y_skip;
}

