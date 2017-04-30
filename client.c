#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#define IP 		"127.0.0.1"
#define PORT 	8081

struct g_card {
	int suite,rank;
};
typedef struct g_card card;

typedef struct g_player {
	int score;
	int cfd;
	char name[20];
	int cardcount;
	card cardsgiven[30];
	int bid;
}player;

void printcard(card c)
{
	if(c.suite==1)
		printf("SPADES-");
	else if(c.suite==2)
		printf("CLUBS-");
	else if(c.suite==3)
		printf("HEARTS-");
	else
		printf("DIAMONDS-");
	if(c.rank==11)
		printf("J ");
	else if(c.rank==12)
		printf("Q ");
	else if(c.rank==13)
		printf("K ");
	else if(c.rank==14)
		printf("A ");
	else
		printf("%d ", c.rank);
}

int cfd;

int main()
{
	struct sockaddr_in sock_var;
	int serverfd = socket(AF_INET, SOCK_STREAM, 0);
	char str_client[20], str_server[20];

	sock_var.sin_addr.s_addr = inet_addr(IP);
	sock_var.sin_port = PORT;
	sock_var.sin_family = AF_INET;

	if(connect(serverfd, (struct sockaddr *)&sock_var, sizeof(sock_var))>=0)
	{
		printf("Connected to server..\n");
		printf("Enter your name: \n");
		scanf("%s", str_client);
		int sen = send(serverfd, str_client, strlen(str_client),0);
		int rec = recv(serverfd, str_server, strlen(str_server),0);
		if(rec==0)
		{
			printf("Unable to join game.\n");
		}
		else if(rec>0)
		{
			if(str_server[0]=='E')
				printf("Joined game...\n");
		}
		player currentplayer;
		/*int cc=0;
		rec = recv(serverfd, &cc, sizeof(int),MSG_WAITALL);
		if(rec>0){
			//Show cards
			printf("Round %d started.\n",cc);
			printf("Your cards..\n");
			int p,suite,rank;
			for(p=0;p<cc;p++){
				rec = recv(serverfd, &suite, sizeof(int),MSG_WAITALL);
				if(rec>0)	printsuite(suite);	
				rec = recv(serverfd, &rank, sizeof(int),MSG_WAITALL);
				if(rec>0)	printrank(rank);		
			}
			//Take bid
			printf("Enter your bid:");
			int bid;
			scanf("%d",&bid);
			send(serverfd,&bid,sizeof(bid),0);		
		}*/
		char str[200];
		rec = recv(serverfd, &str, sizeof(str),MSG_WAITALL);
		//printf("%s\n",str);
		rec = recv(serverfd, &currentplayer, sizeof(currentplayer),MSG_WAITALL);
		if(rec>0){
			//Show cards
			printf("Round %d started.\n",currentplayer.cardcount);
			printf("Your cards..\n");
			int p;
			for(p=0;p<currentplayer.cardcount;p++){
				printcard(currentplayer.cardsgiven[p]);
				printf("\n");		
			}
			//Take bid
			printf("Enter your bid:");
			int bid;
			scanf("%d",&bid);
			send(serverfd,&bid,sizeof(bid),0);		
			close(serverfd);
		}
		else
		{
			printf("Socket creation failed\n");
		}
	}
	return 0;
}
