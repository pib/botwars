#include "client.h"
#include <errno.h>

int get_sock(struct sockaddr_in* sockaddr)
{
	int sock;

	// Create a socket
	if((sock = socket(PF_INET, SOCK_STREAM, 0))< 0) {
		perror("Socket creation failed");
		return -1;
	}
		int yes=1;
	if(setsockopt(sock,SOL_SOCKET,SO_REUSEADDR,&yes,sizeof(int))<0) {
		perror("Socket option setting failed");
		close(sock);
		return -1;
	}

	// Bind to the address, so that the ports are set
	if(connect(sock,(struct sockaddr *) sockaddr,	sizeof(struct sockaddr_in)) < 0) {
		perror("Socket binding failed");
		close(sock);
		return -1;
	}

	return sock;
}

int local_sock(int port)
{
	struct sockaddr_in sockaddr;
	// Zero out the address' memeory
	memset(&sockaddr, 0, sizeof(struct sockaddr));

	sockaddr.sin_family = AF_INET;

	// Set the address port and address
	sockaddr.sin_port = htons(port);
	sockaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	return get_sock(&sockaddr);
	
}

int send_start_packet(int sock, int version, char* name, start_packet* sp)
{
	int ret;
	sp->version = version;
	strcpy(sp->name,name);

	ret = send(sock, sp, sizeof(start_packet), 0);
	if(ret < sizeof(start_packet)) {
		perror("send start_packet failed");
		printf("sent %d, should have been %d\n", ret, (int)sizeof(start_packet));
	} else
		printf("send start_packet succeeded.\n");

	ret = recv(sock, sp, sizeof(start_packet), 0);
	if(ret < sizeof(start_packet)) {
		perror("recv start_packet failed");
		printf("recv %d, should have been %d\n", ret, (int)sizeof(start_packet));
	} //else
	//	printf("recv start_packet succeeded.\n");
	return ret;
}

int send_message(int sock, char cmd, float arg, status_packet* stat_pac)
{
	msg_packet msg_pac;

	msg_pac.msg_type = cmd;
	msg_pac.param = arg;
	int ret = send(sock, &msg_pac, sizeof(msg_packet),0);
	if (ret < sizeof(msg_packet) ) {
		perror("send message failed");
		printf("%c, %0.04f sent %d, should have sent: %d\n", cmd, arg, ret, (int)sizeof(msg_packet));
	}
	//else printf("send forward succeeded.\n");

	ret = recv(sock, stat_pac, sizeof(status_packet),0);
	if(ret < sizeof(status_packet)) {
		perror("recv status failed");
		printf("recv'ed %d should have recv'd %d\n", ret, (int)sizeof(status_packet));
		ret = -1;
	} //else printf("recv status %0.04f succeeded.\n", stat_pac->retval);

	return ret;
}

