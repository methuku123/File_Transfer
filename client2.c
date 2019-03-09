#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

int main(int argc, char* argv[])
{
	if(argc != 3)
	{
		perror("Error in giving hostname or portno");
		exit(1);
	}

	int sockfd, temp, portno;

	struct sockaddr_in ser_addr;
	struct hostent *server; 

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	server = gethostbyname(argv[1]);

	if(sockfd < 0)
	{
		perror("Error opening socket");
		exit(1);
	}

	portno = atoi(argv[2]);

	bzero(&ser_addr, sizeof(ser_addr));

	ser_addr.sin_family = AF_INET;
	// ser_addr.sin_addr.s_addr = (char *)server-> h_addr;
	bcopy((char *)server-> h_addr, (char *) &ser_addr.sin_addr.s_addr, server-> h_length);
	ser_addr.sin_port = htons(portno);

	if(connect(sockfd, (struct sockaddr *) &ser_addr, sizeof(ser_addr)) < 0)
	{
		perror("Error in connecting");
		exit(1);
	}
	else
		printf("Connection success\n");

	char buffer[255];

	while(1)
	{
		bzero(buffer, 255);

		temp = read(sockfd, buffer, sizeof(buffer));

		if(temp < 0)
			perror("Error in reading");

		printf("Server- %s", buffer);

		int t;
		scanf("%d", &t);

		if(t <= 6 && t > 0)
		{
			temp = write(sockfd, &t, sizeof(t));

			if(temp < 0)
			{
				perror("Error in writing");
				break;
			}
		}

		else
		{
			printf("Server- You selected wrong option :(\n");
			break;
		}

		if(t == 6)
		{
			printf("Exiting...!\n");
			break;
		}

		int count;

		temp = read(sockfd, &count, sizeof(count));

		if(temp < 0)
		{
			perror("Error in writing");
			break;
		}

		int j = 0;

		FILE *fp;

		if((fp = fopen("newFile.txt", "w")) == NULL)
		{
			perror("Error in creating a file");
			break;
		}

		rewind(fp);

		while(j < count)
		{
			char ch;
			temp = read(sockfd, &ch, sizeof(ch));

			if(temp < 0)
				perror("Error in reading");

			fputc(ch, fp);
			j++;
		}

		printf("Server- Successfully transferred..!\n");
		fclose(fp);
	}

	close(sockfd);

	return 0;
}