#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <pthread.h>

struct args
{
	int newfd;
	int clientNo;
};

void *function(void *input)
{
	int newsockfd = ((struct args*)input)-> newfd, temp;

	int i = ((struct args*)input)-> clientNo;

	char buffer[255], buffer1[255];

	while(1)
	{
		int flag = 0;

		bzero(buffer, 255);

		strcpy(buffer, "Select from the following options-\n          1.server.c\n          2.server1.c\n          3.server2.c\n          4.client.c\n          5.client2.c\n          6.exit\n");
		temp = write(newsockfd, buffer, strlen(buffer));

		if(temp < 0)
			perror("Error in writing");

		int t;
		temp = read(newsockfd, &t, sizeof(t));

		printf("\nClient(%d)- %d\n", i, t);

		bzero(buffer1, 255);

		switch(t)
		{
			case 1:
				strcpy(buffer1, "server.c");
				break;
			case 2:
				strcpy(buffer1, "server1.c");
				break;
			case 3:
				strcpy(buffer1, "server2.c");
				break;
			case 4:
				strcpy(buffer1, "client.c");
				break;
			case 5:
				strcpy(buffer1, "client2.c");
				break;
			default:
				flag = 1;
				break;			
		}

		if(flag)
		{
			printf("Exiting client(%d)\n", i);
			break;
		}

		FILE *fp;

		if((fp = fopen(buffer1, "r")) == NULL)
		{
			perror("Error in opening file");
			break;
		}

		int count = 0; char c;

		rewind(fp);

		c = fgetc(fp);

		while(c != EOF)
		{
         	count++;
         	c = fgetc(fp);
    	}

    	temp = write(newsockfd, &count, sizeof(count));

    	rewind(fp);

    	c = fgetc(fp);

	    while (c != EOF) 
	    { 
	        write(newsockfd, &c, sizeof(c)); 
	        c = fgetc(fp); 
	    }

	    printf("Client(%d)- Successfully received..!\n", i);
   		fclose(fp);
	}

	close(newsockfd);
}

int main(int argc, char* argv[])
{
	if(argc != 3)
	{
		perror("Error in giving port");
		exit(1);
	}
	
	int sockfd, temp, portno, n;

	struct sockaddr_in ser_addr, cli_addr[n];

	sockfd = socket(AF_INET, SOCK_STREAM, 0);

	if(sockfd < 0)
	{
		perror("Error opening socket");
		exit(1);
	}

	portno = atoi(argv[2]);
	n = atoi(argv[1]);

	bzero(&ser_addr, sizeof(ser_addr));

	ser_addr.sin_family = AF_INET;
	ser_addr.sin_addr.s_addr = INADDR_ANY;
	ser_addr.sin_port = htons(portno);

	if(bind(sockfd, (struct sockaddr *) &ser_addr, sizeof(ser_addr)) < 0)
	{
		perror("Error in binding");
		exit(1);
	}
	else
		printf("Binding Success\n");

	listen(sockfd, n);

	int clilen = sizeof(cli_addr);

	int i = 0; 

	pthread_t client[n];

	while(i < n)
	{
		int newsockfd = accept(sockfd, (struct sockaddr *)& cli_addr[i], &clilen);

		if(newsockfd < 0)
		{
			perror("Error in accepting");
			exit(0);
		}

		struct args *input = (struct args *)malloc(sizeof(struct args));
		input-> newfd = newsockfd;
		input-> clientNo = i;

		pthread_create(&client[i++], NULL, function, input);
	}

	i = 0;

	while(i < n)
		pthread_join(client[i++], NULL);

	close(sockfd);

	return 0;
}