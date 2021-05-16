#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/select.h>

#define BUF_SIZE 200
void error_handling(char *buf);

int main(int argc, char *argv[])
{
	int serv_sock, clnt_sock;
	struct sockaddr_in serv_adr, clnt_adr;
	struct timeval timeout;
	fd_set reads, cpy_reads;
	socklen_t adr_sz;
	int fd_max, str_len, fd_num, i, j ,cnt = 0;
	char buf[BUF_SIZE], client[20] = "client ", *str, welcome[20] = "Server: Welcome~\n", join[30] = "has joined this chatting room\n", left[30] = "has left this chatting room\n";


	if(argc != 2)
	{
		printf("Usage : %s <port>\n",argv[0]);
		exit(1);
	}

	serv_sock = socket(PF_INET, SOCK_STREAM, 0);
	memset(&serv_adr, 0, sizeof(serv_adr));
	serv_adr.sin_family = AF_INET;
	serv_adr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_adr.sin_port = htons(atoi(argv[1]));

	if(bind(serv_sock, (struct sockaddr*)&serv_adr, sizeof(serv_adr)) == -1)
		error_handling("bind() error");
	if(listen(serv_sock, 5) == -1)
		error_handling("listen() error");
	FD_ZERO(&reads);
	FD_SET(serv_sock, &reads);
	fd_max = serv_sock;

	while(1)
	{
		char client[30] = "client ", *str, n[5];

		cpy_reads = reads;
		timeout.tv_sec = 1;
		timeout.tv_usec = 0;
		if((fd_num = select(fd_max + 1, &cpy_reads, 0, 0, &timeout)) == -1)
			break;
		if(fd_num == 0)
			continue;
		for(i = 0; i < fd_max + 1; i++)
		{
			if(FD_ISSET(i, &cpy_reads))
			{
				if(i == serv_sock)
				{
					cnt++;
					adr_sz = sizeof(clnt_adr);
					clnt_sock = accept(serv_sock, (struct sockaddr*)&clnt_adr, &adr_sz);
					FD_SET(clnt_sock, &reads);
					if(fd_max < clnt_sock)
						fd_max = clnt_sock;
					printf("connected client: %d\n", clnt_sock);
					
					n[0] = clnt_sock + '0'; n[1] = ' ';
					str = strcat(client, n);
					str = strcat(str, join);
					
					for(j = serv_sock + 1; j <= fd_max; j++)
					{
						if (j == clnt_sock)
						{
							write(clnt_sock, welcome, sizeof(welcome));
							write(clnt_sock, "Server: The num of clients is ", 30);
							write(clnt_sock, n, sizeof(n));
							write(clnt_sock, "now\n", 8);			
						}
						else	
							write(j, str, 80);		
					}
					write(clnt_sock, "Server: The num of clients is ", 30);
					write(clnt_sock, n, sizeof(n));
					write(clnt_sock, "now\n", 10);			
				}
				
				else
				{
					str_len = read(i, buf, BUF_SIZE);	
						
					if(str_len == 0)
					{
						n[0] = i + '0'; n[1] = ' ', n[2] = '\0';
						cnt--;
						
						FD_CLR(i, &reads);
						close(i);
						printf("closed client: %d\n", i);
						for(j = serv_sock + 1; j <= fd_max; j++)
						{
							if(i == j)
								continue;
							str = strcat(client, n);
							str = strcat(str, left);
							write(j, str, 50);
						}
					}
					else
					{
						n[0] = i + '0'; n[1] = ' '; n[2] = ':', n[3] = ' ';
						for(j = serv_sock + 1; j <= fd_max; j++)
						{
							if(i == j)
								continue;
							str = strcat(client, n);
							str = strcat(str, buf);
							write(j, str, str_len + 11);
						}
					}
				}
			}
		}
	}
	close(serv_sock);
	return 0;
}

void error_handling(char *buf)
{
	fputs(buf, stderr);
	fputc('\n', stderr);
	exit(1);
}
				
