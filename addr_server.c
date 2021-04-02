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
	int serv_sd, clnt_sd;
	char buf[30];
	struct sockaddr_in serv_addr, clnt_addr;
	socklen_t clnt_addr_size;

	if(argc != 2)
	{
		printf("Usage : %s <port>\n", argv[0]);
		exit(1);
	}

	serv_sd = socket(PF_INET, SOCK_STREAM, 0);
	if(serv_sd == -1)
		error_handling("socket() error");
	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_addr.sin_port = htons(atoi(argv[1]));
	if(bind(serv_sd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == -1)
		error_handling("bind() error");
	if(listen(serv_sd, 5) == -1)
		error_handling("listen() error");
	clnt_addr_size = sizeof(clnt_addr);

	clnt_sd = accept(serv_sd, (struct sockaddr*)&clnt_addr, &clnt_addr_size);
	if(clnt_sd == -1)
		error_handling("accept() error");
	else
		printf("new client connected...\n");

	while(1)
	{
		read(clnt_sd, buf, 30);
		if(strcmp(buf, "client disconnected...") == 0)
		{
			printf("%s\n", buf);
			break;
		}

		struct hostent *host;
		host = gethostbyname(buf);
		if(!host)
		{
			write(clnt_sd, "gethost... error", 30);
			continue;
		}

		char *ip = inet_ntoa(*(struct in_addr*)host->h_addr_list[0]);
		write(clnt_sd, ip, 30);
	}

	close(serv_sd);
	close(clnt_sd);
	return 0;
}


void error_handling(char *message)
{
	printf("%s\n", message);
	exit(1);
}
