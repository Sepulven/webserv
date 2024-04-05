#include <arpa/inet.h>
#include <ctype.h>
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <pthread.h>
#include <regex.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <poll.h>

#define PORT 8080

int main(void)
{
	int server_fd;
	struct sockaddr_in server_addr;
	int enable = 1;

	server_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (server_fd < 0)
	{
		printf("sockt function failed!\n");
		exit(EXIT_FAILURE);
	}

	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	server_addr.sin_port = 8080;

	if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0)
	{
		printf("SO_REUSEADDR failed\n");
		exit(EXIT_FAILURE);
	}

	// int status = bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr));

	// printf("%d\n", status);
	if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
	{
		printf("bind failed\n");
		exit(EXIT_FAILURE);
	}

	if (listen(server_fd, 10) < 0)
	{
		printf("listen failed\n");
		exit(EXIT_FAILURE);
	}

	struct sockaddr_in client_addr;
	socklen_t client_addr_len = sizeof(client_addr);
	int client_fd = -1;
	while (1)
	{
		printf("%d\n", client_fd);
		client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &client_addr_len);
		printf("%d\n", client_fd);
		if (client_fd < 0)
		{
			printf("accept failed!");
			continue ;
		}
		else
		{
			printf("here we are!\n");
		}
	}

	return (0);
}