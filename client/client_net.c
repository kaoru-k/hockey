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
CONTAINER send_con;
CONTAINER recv_con;

/*旧
static void send_command(int command);
static int  recv_command(void);
static void send_pos(void);
static void recv_pos(void);
*/
static void set_con(int command);
static int  out_con(void);
static void copy_pad(PAD *a, const PAD *b);
static void copy_player(PLAYER *a, const PLAYER *b);
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

    if (endflag == 0)
        set_con(N);
    else
        set_con(X);

    fprintf(stderr, "send_data()\n");
    send_data(&send_con, sizeof(CONTAINER));
    
    if (select(num_sock, (fd_set *)&read_flag, NULL, NULL, &timeout) == -1)
            error_message("select()");
    
    if (FD_ISSET(sock, &read_flag)) {
        fprintf(stderr, "recv_data() :");
        recv_data(&recv_con, sizeof(CONTAINER));
        if (out_con() == X) {
            endflag = 1;
            return 0;
        }
    }
    return 1;
}

static void set_con(int command)
{
    send_con.com = command;
    copy_player(&send_con.p0, &p[myid]);
}

static int out_con(void)
{
    copy_pad(&pad, &recv_con.pad);
    if (0 != myid) copy_player(&p[0], &recv_con.p0);
    if (1 != myid) copy_player(&p[1], &recv_con.p1);
    if (2 != myid) copy_player(&p[2], &recv_con.p2);
    if (3 != myid) copy_player(&p[3], &recv_con.p3);
    if (4 != myid) copy_player(&p[4], &recv_con.p4);
    if (5 != myid) copy_player(&p[5], &recv_con.p5);

    fprintf(stderr, "com=%d\n", recv_con.com);
    return recv_con.com;
}

static void copy_pad(PAD *a, const PAD *b)
{
    a->x = b->x;
    a->y = b->y;
}

static void copy_player(PLAYER *a, const PLAYER *b)
{
    a->type = b->type;
    a->hp   = b->hp;
    a->ap   = b->ap;
    a->x    = b->x;
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
