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
#include <errno.h>

static int myid;
static int sock;
static int num_sock;
static fd_set mask;
CLIENT clients[4];

void setup_client(char *server_name, u_short port);
void terminate_client(void);
static int  recv_data(void *data, int size);
static void send_data(void *data, int size);
static void error_message(char *message);

void setup_client(char *server_name, u_short port)
{
    struct hostent *server;
    struct sockaddr_in sv_addr;

    fprintf(stderr, "Connecting to server (name = %s, port = %d)...", server_name, port);
    if ((server = gethostbyname(server_name)) == NULL) error_message("failed!\ngethostbyname()");
    
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        error_message("failed!\nsocket()");
    }
    
    sv_addr.sin_family = AF_INET;
    sv_addr.sin_port = htons(port);
    sv_addr.sin_addr.s_addr = *(u_int *)server->h_addr_list[0];

    if(connect(sock, (struct sockaddr *)&sv_addr, sizeof(sv_addr)) != 0) error_message("failed!\nconnect()");
    fprintf(stderr, "done.\n");

    fprintf(stderr, "Waiting for other clients... ");
    recv_data(&myid, sizeof(int));
    fprintf(stderr, "done.\nYour ID = %d.\n", myid);
    int i;
    for(i = 0; i < 4; i++) {
        recv_data(&clients[i], sizeof(CLIENT));
    }
}

static int recv_data(void *data, int size)
{
    if ((data == NULL) || (size <= 0)) error_message("recv_data()");
    return(read(sock, data, size));
}

void send_data(void *data, int size)
{
    if ((data == NULL) || (size <= 0)) error_message("send_data()");
    if (write(sock, data, size) == -1) error_message("send_data()");
}

void terminate_client(void)
{
    fprintf(stderr, "Connenction is closed.\n");
    close(sock);
}

static void error_message(char *message)
{
    perror(message);
    fprintf(stderr, "%d\n", errno);
    exit(1);
}
