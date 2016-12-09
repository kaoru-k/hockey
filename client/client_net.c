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

static int sock;
static int num_sock;
static fd_set mask;

int myid;
CLIENT clients[4];

static void send_command(int command);
static int  recv_command(void);
static void send_pos(void);
static void recv_pos(void);
static void send_data(void *data, int size);
static int  recv_data(void *data, int size);
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
    num_sock = sock + 1;
    FD_ZERO(&mask);
    FD_SET(0, &mask);
    FD_SET(sock, &mask);
    fprintf(stderr, "Client setup is done.\n");
}

int network(void)
{
    
    fd_set read_flag = mask;
    struct timeval timeout;
    timeout.tv_sec = 0;
    timeout.tv_usec = 4;

    if (endflag == 1) {
        send_command(X);
        return 1;
    }
    else {
        send_command(N);

        send_pos();
        
        if (select(num_sock, (fd_set *)&read_flag, NULL, NULL, &timeout) == -1)
            error_message("select()");
    
        if (FD_ISSET(sock, &read_flag)) {
            
            if (recv_command() == N) {

                recv_pos();
                
                return 1;
            }
            else
                return 0;
        }

    }
        return 1;
}

static void send_command(int command)
{
    send_data(&command, sizeof(int));
}

static int recv_command(void)
{
    int data;
    recv_data(&data, sizeof(int));
    return data;
}

static void send_pos(void)
{
    send_data(&p[myid], sizeof(PLAYER));
    fprintf(stderr, "send_data()\n");
}

static void recv_pos(void)
{
    int i;

    for (i = 0; i < 6; i++) {
        if (i != myid) {
            recv_data(&p[i], sizeof(PLAYER));
            fprintf(stderr, "recv_data() %d\n", i);
        }
    }
    recv_data(&pad, sizeof(PAD));
    fprintf(stderr, "recv_data()\n%f %f\n", pad.x, pad.y);
}


static void send_data(void *data, int size)
{
    if ((data == NULL) || (size <= 0)) error_message("send_data()");
    if (write(sock, data, size) == -1) error_message("send_data()");
}

static int recv_data(void *data, int size)
{
    if ((data == NULL) || (size <= 0)) error_message("recv_data()");
    return(read(sock, data, size));
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
