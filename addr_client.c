#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netdb.h>

void error_handling(char *message);
int main(int argc, char *argv[])
{
	int sd;
	char buf[30];
	struct sockaddr_in serv_addr;

	if(argc != 3)
	{
		printf("Usage : %s <IP>  <port>\n", argv[0]);
		exit(1);
	}

	sd = socket(PF_INET, SOCK_STREAM, 0);
	if(sd == -1)
		error_handling("socket() error");
	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = inet_addr(argv[1]);
	serv_addr.sin_port = htons(atoi(argv[2]));
	
	connect(sd, (struct sockaddr*)&serv_addr, sizeof(serv_addr));

	while(1)
	{
		printf("Enter a host name(q to quit): ");
		scanf("%s", buf);
		if(strcmp(buf, "q") == 0 || strcmp(buf, "Q") == 0)
		{
			write(sd, "client disconnected...",30);
			break;
		}

		write(sd, buf, 30);
		read(sd, buf, 30);
		printf("%s\n", buf);
	}

	close(sd);
	return 0;
}


void error_handling(char *message)
{
	printf("%s\n", message);
	exit(1);
}
