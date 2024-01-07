//
// Created by B Pan on 12/3/23.
//

//Sample echo client program - CSS 432 - Winter 2024

#include <cstdio>
#include <iostream>
#include <fstream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <cstdlib>
#include <cstring>
#include <unistd.h>


#define SERV_UDP_PORT 61125
#define SERV_HOST_ADDR "127.0.0.1"

/* Size of maximum message to send.                                */
#define MAX_LINE 512

/* A pointer to the name of this program for error reporting.      */
char *program;

/* The dg_cli function reads lines from the terminal, sends them   */
/* to the echo server pointed to by pserv_addr, and prints to the  */
/* terminal the echoed data. The local endpoint is sockfd. The     */
/* function returns when an EOF is seen. Note that the server's    */
/* address in pserv_addr is already initialized. Its size is held  */
/* in serv_len, so that the function works with other protocol     */
/* families that have different address sizes.                     */

void dg_cli(int sockfd, struct sockaddr *pserv_addr, int serv_len) {

/* Various counter and buffer variables. The extra byte in the     */
/* receive buffer is used to add a null to terminate the string,   */
/* as the network routines do not use nulls but I/O functions do.  */

    size_t n;
    char send_line[MAX_LINE], recv_line[MAX_LINE + 1];

/* Main client loop. Terminates on EOF. Get terminal input on the  */
/* send_line buffer (up to MAX_LINE bytes).                        */

    while (fgets(send_line, MAX_LINE, stdin) != nullptr) {

/* Find the string's length to pass it as a parameter to the send  */
/* calls (excluding the null in the end).                          */

        n = strlen(send_line);

/* Send data using the sockfd socket, to the server at pserv_addr. */
/* We also pass the send buffer and its size (send_line and n), an */
/* unused flag byte (0) and the server's address size (serv_len).  */
/* The returned number is the number of bytes sent. If it is not n */
/* either an error or an interrupt has occurred.                   */
        printf("Sending data to server: %s\n", send_line);
        if (sendto(sockfd, send_line, n, 0, pserv_addr, serv_len) != n) {
            perror("sendto error on socket.");
            exit(3);
        }

/* Read the echoed data from the same socket into the recv_line    */
/* buffer (up to MAX_LINE bytes). The server's address and its size*/
/* are returned, but we do not need them, so we use null pointers. */
/* The return value is the number of bytes received.               */

        std::cout << "Waiting to receive data from server" << std::endl;

        n = recvfrom(sockfd, recv_line, MAX_LINE, 0, nullptr, nullptr);
        if (n < 0) {
            perror("recvfrom error.");
            exit(4);
        } else {
            std::cout << "Received data from server: " << std::endl;

/* The exchanged data is not null terminated, as the string length */
/* is explicitly sent. We need to null terminate the string before */
/* using fputs to output it on the terminal.                       */

            recv_line[n] = 0;
            fputs(recv_line, stdout);
            std::cout << "\n************\n" << std::endl;

            // Writing received output to file. DO NOT CHANGE.
            std::ofstream output;
            output.open("output_client.txt", std::ios::app);
            output << recv_line;
            output.close();
        }
    }
}

/* The main program sets up the local socket for communication     */
/* and the server's address and port (well-known numbers) before   */
/* calling the dg_cli main loop.                                   */

int main(int argc, char *argv[]) {
    program = argv[0];

    int sockfd;

/* We need to set up two addresses, one for the client and one for */
/* the server.                                                     */

    struct sockaddr_in cli_addr, serv_addr;

/* Initialize first the server's data with the well-known numbers. */

    memset(&serv_addr, 0, sizeof(serv_addr));


/* Fill in serv_addr information */

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr(SERV_HOST_ADDR);
    serv_addr.sin_port        = htons(SERV_UDP_PORT);

/* Create the socket for the client side.                          */

    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("client socket creation failed.");
        exit(EXIT_FAILURE);
    }

/* Initialize the structure holding the local address data to      */
/* bind to the socket.                                             */

    memset(&cli_addr, 0, sizeof(cli_addr));
    cli_addr.sin_family = AF_INET;

/* Let the system choose one of its addresses for you. You can     */
/* use a fixed address as for the server.                          */

    cli_addr.sin_addr.s_addr = htonl(INADDR_ANY);

/* The client can also choose any port for itself (it is not       */
/* well-known). Using 0 here lets the system allocate any free     */
/* port to our program.                                            */

    cli_addr.sin_port = htons(0);

/* The initialized address structure can be now associated with    */
/* the socket we created. Note that we use a different parameter   */
/* to exit() for each type of error, so that shell scripts calling */
/* this program can find out how it was terminated. The number is  */
/* up to you, the only convention is that 0 means success.         */

    if (bind(sockfd, (struct sockaddr *) &cli_addr, sizeof(cli_addr)) < 0) {
        perror("Can't bind local address.");
        exit(EXIT_FAILURE);
    }

/* Call the main client loop. We need to pass the socket to use    */
/* on the local endpoint, and the server's data that we already    */
/* set up, so that communication can start from the client.        */
    std::cout << "Bind socket complete" << std::endl;

    std::cout << "Type a message to send to server..." << std::endl;
    dg_cli(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr));

/* We return here after the client sees the EOF and terminates.    */
/* We can now release the socket and exit normally.                */

    close(sockfd);
    exit(0);
}
