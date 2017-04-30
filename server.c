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

//suites 1-SPADES, 2-CLUBS, 3-HEARTS, 4-DIAMONDS, rank 2345678910JQKA A-14
struct g_card {
	int suite,rank;
};
typedef struct g_card card;
card deck[52];

typedef struct g_player {
	int score;
	int cfd;
	char name[20];
	int cardcount;
	card cardsgiven[30];
	int bid;
}player;

//Shuffle Deck - Fisher-Yates Algorithm
void shuffleDeck(card * deck)
{
	int i;
	for(i=0; i<52; i++)
	{
		card temp = deck[i];
		int j = rand()%52;
		deck[i] = deck[j];
		deck[j] = temp;
	}
}

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

char myret(card c) {
	if(c.rank==11)
		return 'J';
	else if(c.rank==12)
		return 'Q';
	else if(c.rank==13)
		return 'K';
	else if(c.rank==14)
		return 'A';
	else if(c.rank==10)
		return 'T';
	else
		return ('0' + c.rank);
}

char retSuit(int a) {
	if(a==1)
		return 'S';
	else if(a==2)
		return 'C';
	else if(a==3)
		return 'H';
	else
		return 'D';
}

int distributeCards(int players,player plist[],int roundno){
		int iter;
		for(iter=0; iter<52; iter++)
		{
			deck[iter].suite = iter/13+1;
			deck[iter].rank = iter%13+2;
		}
		shuffleDeck(deck);
		int curr,i=0;		
		for(curr=0; curr<players; curr++)
		{
			plist[curr].cardcount=0;
			int cc;		
			for(cc=0; cc<roundno; cc++)
			{
				plist[curr].cardsgiven[cc]=deck[i];
				i++;
				plist[curr].cardcount++;
			}
			//String generate
			char cstr[200]="Trump card is ";
			//memset(cstr, 0, sizeof(cstr));
			player currentplayer=plist[curr];
			/*int abc=0,p;
			for(p=0;p<currentplayer.cardcount;p++){
				cstr[abc] = retSuit(currentplayer.cardsgiven[p].suite);
				cstr[abc + 1] = myret(currentplayer.cardsgiven[p]);
				cstr[abc + 2] = '\n';
				abc += 3;	
			}*/
			cstr[14]= retSuit(deck[i].suite);
			cstr[15]=' ';
			cstr[16]=myret(deck[i]);
			send(plist[curr].cfd,&cstr,sizeof(cstr),0);			
			/*printf("%s's cards..\n",plist[curr].name);
			for(p=0;p<currentplayer.cardcount;p++){
				printcard(currentplayer.cardsgiven[p]);
				printf("\n");		
			}*/
			printf("Trump card is ");
			printcard(deck[i]);
			printf("\n");
			send(plist[curr].cfd,&plist[curr],sizeof(plist[curr]),0);
			/*int ccount=plist[curr].cardcount;
			printf("%d",ccount);
			send(plist[curr].cfd,&ccount,sizeof(int),0);			
			int p;
			for(p=0;p<plist[curr].cardcount;p++){
				send(plist[curr].cfd,&(plist[curr].cardsgiven[p].suite),sizeof(int),0);
				send(plist[curr].cfd,&(plist[curr].cardsgiven[p].rank),sizeof(int),0);
			}*/
			int bid;
			int rec = recv(plist[curr].cfd, &bid, sizeof(bid),0);
			if(rec>0)
			{
				plist[curr].bid=bid;
				printf("%s's bid is %d\n",plist[curr].name,bid);		
			}
			else if(rec==0)
			{
				//Disconnected from game
				printf("%s disconnected from the game!!\n",plist[curr].name);
			}
		}
	return i;
}

void showScoreBoard(int players,player plist[]){
	int cn;
	printf("        Scoreboard start\n");
	for(cn=0;cn<players;cn++){
		printf("%s's score is %d\n",plist[cn].name,plist[cn].score);
	}
	printf("        Scoreboard end\n");
}

void giveScores(int players,player plist[],int roundno,int level,int trump){
	int cn;
	card trumpcard = deck[trump];
	int sum=0;
	for(cn=0;cn<players;cn++){
		int count;
		for(count=0;count<roundno;count++){
			if(plist[cn].cardsgiven[count].suite==trumpcard.suite){
				sum++;
			}
			int bid=plist[cn].bid;
			if(bid==sum){
				if(bid==0)	plist[cn].score+=(roundno*level);
				else	plist[cn].score+=(bid*(roundno*level));
			}else{
				if(bid>sum)
					plist[cn].score-=((bid-sum)*level);
				else
					plist[cn].score-=((sum-bid)*level);
			}	
			
		}
	}
}

int main()
{
	struct sockaddr_in sock_var;
	int serverfd = socket(AF_INET, SOCK_STREAM, 0);
	int clientfd, cfd;

	sock_var.sin_addr.s_addr = inet_addr(IP);
	sock_var.sin_port = PORT;
	sock_var.sin_family = AF_INET;

	struct sockaddr_in socket2;
	int serverSocket2 = socket(AF_INET, SOCK_STREAM, 0);
	int cfd2;

	if(bind(serverfd,(struct sockaddr *)&sock_var, sizeof(sock_var))>=0)
	{
		printf("socket created successfully\n");
		listen(serverfd,7);
		int p,i;
		printf("No of players present: \n");
		scanf("%d",&p);
		player plist[p];		
		for(i=0; i<p; i++)
		{
			char str[20];
			clientfd = accept(serverfd, NULL, NULL);
			plist[i].score=0;
			plist[i].cfd=clientfd;
			if(recv(clientfd,str,20,0)>0)
			{
				strcpy(plist[i].name,str);
				printf("%s joined the game.\n",str);
				send(clientfd,"Exit",4,0);
			}
			else if(recv(clientfd,str,20,0)==0)
			{
				printf("Client %d has disconnected.\n", clientfd);
			}
		}
		printf("All players connected...Starting game.\n");
		int rounds,level;
		printf("How many rounds do you need in the game: \n");
		scanf("%d",&rounds);
		printf("Choose a difficulty level.. 1:Easy 2:Moderate 3:Hard\n");
		scanf("%d",&level);
		close(serverfd);
		//Game logic
		int roundno;		
		for(roundno=1;roundno<=100;roundno++){
			int trump;
			trump=distributeCards(p,plist,roundno);
			roundno--;
			giveScores(p,plist,roundno,level,trump);
			showScoreBoard(p,plist);			
		}	
	}
	else
	{
		printf("socket creation failed\n");
	}



	return 0;
}
