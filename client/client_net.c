/*************************************
  client_net.c
  クライアントのネットワークモジュール
  徳島大学 工学部 知能情報工学科 27班
*************************************/

#include "client.h"
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>


static int myid;
static int sock;
static int num_sock;
static fd_set mask;


static void error_message(char *message);

void setup_client(char *server_name, u_short port)
{
    struct hostent *server;
    struct sockaddr_in sv_addr;

    fprintf(stderr, "Connecting server(name: %s, port: %d)... ", server_name, port);
    if ((server = gethostbyname(server_name)) == NULL) error_message("gethostbyname() failed!");
    
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) error_message("socket() failed!");

    sv_addr.sin_family = AF_INET;
    sv_addr.sin_port = htons(port);
    sv_addr.sin_addr.s_addr = *(u_int *)server->h_addr_list[0];

    if (connect(sock, (struct sockaddr *)&sv_addr, sizeof(sv_addr)) != 0) error_message("connect() failed!");
    fprintf(stderr, "done.\n");

    fprintf(stderr, "Waiting for other clients... ");
    recv_data(&myid, sizeof(int));
    fprintf(stderr, "done.\nYou are %dP\n", myid);

    FD_ZERO(&mask);
    FD_SET(0, &mask);
    FD_SET(sock, &mask);
    fprintf(stderr, "Client setup is done.");
}

void recv_data(void *data, int size)
{
    if ((data == NULL) || (size <= 0)) error_message("recv_data() failed!");
    read(sock, data, size);
}

void send_data(void *data, int size)
{
    if ((data == NULL) || (size <= 0)) error_message("send_data() failed!");
    if (write(sock, data, size) == -1) error_message("send_data() failed!");
}

void terminate_client(void)
{
    fprintf(stderr, "Connenction is closed.\n");
    close(sock);
}

void error_message(char *message)
{
    perror(message);
    exit(1);
}
