#include "server.h"
int server_net_setup(int argc,char ** argv)
{
	struct sockaddr_in sockaddr;

	// Create a socket
	if((in_sock = socket(PF_INET, SOCK_STREAM, 0))< 0) {
		printf("%s: Socket creation failed\n",argv[0]);
		exit(1);
	}

	// Zero out the address' memeory
	memset(&sockaddr, 0, sizeof(struct sockaddr));

	sockaddr.sin_family = AF_INET;

	// Set the address port and address
	sockaddr.sin_port = htons(port);
	sockaddr.sin_addr.s_addr = htonl(INADDR_ANY);

	int yes=1;
	if(setsockopt(in_sock,SOL_SOCKET,SO_REUSEADDR,&yes,sizeof(int))<0) {
		printf("%s: Socket option setting failed\n",argv[0]);
		close(in_sock);
		exit(1);
	}
	if(fcntl(in_sock,F_SETFL, O_NONBLOCK)<0){
		printf("%s: Socket blocking option setting failed\n",argv[0]);
		close(in_sock);
		exit(1);
	}

	// Bind to the address, so that the ports are set
	if(bind(in_sock,(struct sockaddr *) &sockaddr,
			sizeof(struct sockaddr_in)) < 0) {
		printf("%s: Socket binding failed\n",argv[0]);
		close(in_sock);
		exit(1);
	}

	if(listen(in_sock, 20) < 0) {
		printf("%s: Socket listen failed\n",argv[0]);
		perror("Failed");
		close(in_sock);
		exit(1);
	}

	return 0;
}
void server_net_accept()
{
	struct sockaddr addr;
	socklen_t s = sizeof(struct sockaddr);
	robot* bot = robot_load(accept(in_sock,&addr,&s));
	if(bot != NULL) {
		bot->x = random()%screen->w;
		bot->y = random()%screen->h;
		printf("Not NULL!!!\n");
		if(botlist_add(bots,bot)) {
			printf("botlist_add failed. Botcount: %d\n", bot_count);
			close(bot->sock);
			del_robot(bot);
		}
		printf("botlist_add 'succeeded'. Botcount: %d\n", bot_count);
	}
}
void server_net_teardown()
{
	close(in_sock);
}
