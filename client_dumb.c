#include "client.h"
#include <errno.h>

int main(int argc, char ** argv)
{
	struct sockaddr_in sockaddr;
	int in_sock, port = 42000;
	int done = 2000;
	start_packet sp;
	msg_packet msg_pac;
	status_packet stat_pac;
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

	// Bind to the address, so that the ports are set
	if(connect(in_sock,(struct sockaddr *) &sockaddr,
			sizeof(struct sockaddr_in)) < 0) {
		printf("%s: Socket binding failed\n",argv[0]);
		close(in_sock);
		exit(1);
	}
	sp.version = 1;
	sprintf(sp.name,"dumb client");
	if(send(in_sock,&sp,sizeof(start_packet),0) < sizeof(start_packet))
		printf("%s: send start_packet failed\n",argv[0]);
	else printf("%s: send start_packet succeeded.\n", argv[0]);
	if(recv(in_sock,&sp,sizeof(start_packet),0) < sizeof(start_packet))
		printf("%s: recv start_packet failed\n",argv[0]);
	else printf("%s: recv start_packet succeeded.\n", argv[0]);

	msg_pac.msg_type = 'F';
	msg_pac.param = 5.0;
	int sent = send(in_sock,&msg_pac,sizeof(msg_packet),0);
	if (sent < sizeof(msg_packet) ) {
		perror("send forward failed");
		printf("Only sent %d, should have sent: %d\n", sent, (int)sizeof(msg_packet));
		}
	else printf("%s: send forward succeeded.\n", argv[0]);

	if(recv(in_sock,&stat_pac,sizeof(status_packet),0) < sizeof(status_packet))
		printf("%s: recv status forward failed\n",argv[0]);
	else printf("%s: recv status forward succeeded.\n", argv[0]);

	msg_pac.msg_type = 'L';
	while(done-- > 0) {
		printf("msg_pac.msg_type: %c, msg_pac.param: %f\n", msg_pac.msg_type, msg_pac.param);
		printf("loop...%d\n", done);
	
		sent = send(in_sock,&msg_pac,sizeof(msg_packet),0);
		if(sent < 8){
			perror("Sending L");
			printf("%s: send L failed. Sent: %d, should've sent %d\n",argv[0], sent, (int)sizeof(msg_packet));
			break;
		}
		else {
			printf("%s: send turn succeeded.\n", argv[0]);
			sent = send(in_sock,&stat_pac,sizeof(status_packet),0);
			if(sent < sizeof(status_packet)){
				printf("%s: recv L failed. Sent: %d\n",argv[0], sent);
				break;
			} else printf("%s: recv status turn succeeded.\n", argv[0]);
		}

	}
	close(in_sock);
	printf("%s: leaving...\n", argv[0]);
	return 0;
}
