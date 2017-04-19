#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>

int main()
{

    // Important declaration
    int serverSocket = 0, newSocket = 0;
    struct sockaddr_in serverAddr;
    struct sockaddr_in clientAddress;
    socklen_t addr_size;

    /*  Create the network socket.   */
    serverSocket = socket(PF_INET, SOCK_STREAM, 0);
    // Configure settings of the server address
    /* Address family = Internet */
    serverAddr.sin_family = AF_INET;
    /* Set port number */
    serverAddr.sin_port = htons(9000);
    /* Set IP address to localhost */
    serverAddr.sin_addr.s_addr = inet_addr ( "172.17.46.123" );
    /* Set all bits of the padding field to 0 */
    memset ( serverAddr.sin_zero, '\0', sizeof (serverAddr.sin_zero) );

    /*---- Bind the address struct to the socket ----*/
    if ( bind ( serverSocket, ( struct sockaddr *) &serverAddr, sizeof ( serverAddr ) )  < 0)
        printf("[ERROR] Socket binding Failed. \n");

    /* Listen on the socket, with 5 max connection requests queued */
    if ( listen ( serverSocket, 5 ) ==0 )
        printf ( "Server Scocket Initiated. Listening to its clients : \n" );
    else
        printf("[ERROR] More than limit.\n");

    /* Accept call creates a new socket for the incoming connection */
    newSocket = accept ( serverSocket, (struct sockaddr *) &clientAddress, &addr_size);
    while ( 1 )
    {
        addr_size = sizeof ( clientAddress );
        if ( newSocket < 0)
            printf("[ERROR] Failed to connect to game.\n");

        printf("Joining client:  %s\n", inet_ntoa(clientAddress.sin_addr), randomNumber );
        sleep ( 1 );
        break;
    }
    close ( newSocket );
    close ( serverSocket );
    return 0;
}
