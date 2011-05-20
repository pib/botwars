#include "client.h"
#include <errno.h>

int main(int ac, char ** av)
{
	int sock = local_sock(42000), ret, done = 4000;
	start_packet sp;
	status_packet stat_pac;
	char* name;
	if(sock < 0) {
		perror("failed to get a socket");
		exit(1);
	}
	if(ac > 1)
		name = av[1];
	else
		name = "client stupid";

	ret = send_start_packet(sock, 1, name, &sp);
	
	if(ret < 0) {
		printf("error in startup\n");
		exit(1);
	}

	ret = send_message(sock, 'F', 1.0, &stat_pac);

	while(done-- > 0) {
		ret = send_message(sock, 'X', 0.0, &stat_pac);
		ret = send_message(sock, 'W', 0.0, &stat_pac);
		ret = send_message(sock, 'W', 0.0, &stat_pac);
		ret = send_message(sock, 'W', 0.0, &stat_pac);
		if((done % 10) == 0)
		ret = send_message(sock, 'R', 0.00001, &stat_pac);
		ret = send_message(sock, 'F', 0.1, &stat_pac);
	}
	return 0;
}
