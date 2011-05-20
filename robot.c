#include "robot.h"

robot* new_robot(char* name, int sock) {
       //printf("Robot is size: %d\n", (int)sizeof(robot));
       robot* bot = malloc(sizeof(robot));
       /*bot->x = x;
       bot->y = y;
       bot->dir = dir;
       bot->vel = vel;
       bot->r = 20;
*/
	bot->dir = (float)(random()%360)*(M_PI/180); // pick a random angle between 0 and 359 degrees..convert to radians
	bot->r = ROBOT_RADIUS;
	bot->name = strdup(name);
	bot->color = -1;
	bot->messagesleft = MESSAGES_PER;
	bot->hitpoints = MAX_HITPOINT;
	bot->msg_packet_queue = malloc(sizeof(msg_packet) * bot->messagesleft);
	bot->messages = 0;
	bot->sock = sock;
       
       return bot;
}

void draw_robot(SDL_Surface* screen, robot* bot) {
	//printf("Robot:    %s\n\tposition: %f, %f\n\theading:  %f\n\tvelocity: %f\n\tcolor: 0x%0x", bot->name, bot->x, bot->y, bot->dir, bot->vel, bot->color);
	SDL_drawCircle_AA(screen, bot->x, bot->y, bot->r, bot->color);
	int dirx = (cos(bot->dir)*(bot->r-1))+bot->x;
	int diry = (sin(bot->dir)*(bot->r-1))+bot->y;
	SDL_drawLine_AA(screen, bot->x, bot->y, dirx, diry, bot->color);
	SDL_Color c = {255,255,255,255};
	if (bot->hitpoints == 0.0) {
		c.g = 0;
		c.b = 0;
	}
	char msg[512];
	sprintf(msg, "%s, HP: %d", bot->name, bot->hitpoints);
	SDL_Surface* txt = TTF_RenderText_Blended(font, msg, c); // render bot's name
	SDL_Rect loc = {(bot->x - (txt->w / 2)), (bot->y - bot->r - txt->h), txt->w,txt->h}; // position text centered above bot
	// if text will go off screen..move it...
	if (loc.x < 0) loc.x = 0; // off the left edge? fix!
	if (loc.y < 0) loc.y = bot->y + bot->r; // off top? move below bot
	if (loc.x+loc.w >= screen->w) loc.x = screen->w-loc.w; // off the right edge? fix!
	SDL_BlitSurface(txt, NULL, screen, &loc); // draw it.
	SDL_FreeSurface(txt);

}

void erase_robot(SDL_Surface* screen, robot* bot) {
	SDL_Rect r;
	r.x = bot->x - bot->r - 1;
	r.y = bot->y - bot->r - 1;
	r.w = bot->r * 2 + 2;
	r.h = bot->r * 2 + 2;
	
	SDL_FillRect(screen, &r, 0); // fill with black
	int w,h;
	TTF_SizeText(font, bot->name, &w, &h); // size of bot's name
	SDL_Rect loc = {(bot->x - (w / 2)), (bot->y - bot->r - h), w,h}; // position text centered above bot
	if (loc.x < 0) loc.x = 0; // off the left edge? fix!
	if (loc.y < 0) loc.y = bot->y + bot->r; // off top? move below bot
	if (loc.x+loc.w >= screen->w) loc.x = screen->w-loc.w; // off the right edge? fix!
	SDL_FillRect(screen, &loc, 0); // draw it.
}

void move_robot(SDL_Surface* screen, robot* bot) {
//	erase_robot(screen, bot);
	
	float x = cos(bot->dir) * bot->vel;
	float y = sin(bot->dir) * bot->vel;

	bot->x += x;
	bot->y += y;

	draw_robot(screen, bot);
}
	

void del_robot(robot* bot) {
	free(bot->name);
	free(bot->msg_packet_queue);
	free(bot);
}

