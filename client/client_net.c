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

int  myid;
int  control_id;
int  latest_frame;
char send_flag;
int  recv_flag = 0;
int  point[2];
CONTAINER_C send_con;
CONTAINER_S recv_con;

static void set_con(char command);
static char out_con(void);
static void copy_pad(PAD *a, const PAD *b);
static void copy_player(PLAYER *a, const PLAYER2 *b);
static void send_data(void *data, int size);
static int  recv_data(void *data, int size);
static void error_message(char *message);
static int  send_thread(void* args);

void setup_client(char *server_name, u_short port)
{
    struct hostent *server;
    struct sockaddr_in sv_addr;

    fprintf(stderr, "Connecting to server (name = %s, port = %d)...", server_name, port);
    if ((server = gethostbyname(server_name)) == NULL)
        error_message("failed!\ngethostbyname()");
    
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0)
        error_message("failed!\nsocket()");
    
    sv_addr.sin_family = AF_INET;
    sv_addr.sin_port = htons(port);
    sv_addr.sin_addr.s_addr = *(u_int *)server->h_addr_list[0];

    if(connect(sock, (struct sockaddr *)&sv_addr, sizeof(sv_addr)) != 0)
        error_message("failed!\nconnect()");
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

void setting_client(void)
{
    int i;
    SETTING2 setting2;
    
    fprintf(stderr, "Send settings... ");
    send_data(&setting, sizeof(SETTING));
    fprintf(stderr, "done.\n");

    recv_data(&control_id, sizeof(int));
    recv_data(&setting2, sizeof(SETTING2));
    for (i = 0; i < 6; i++)
        p[i].type = setting2.type[i];
    fprintf(stderr, "point:%d type:%d\n", setting2.point, p[control_id].type);
}

int network_send(void)
{
    SDL_Thread *thr1;
    if (endflag == 1)
        set_con(COM_EXIT);
    else
        set_con(send_flag);
    //fprintf(stderr, "send_data()\n");
    send_data(&send_con, sizeof(CONTAINER_C));
    //thr1 = SDL_CreateThread(send_thread, NULL);
}

int network_recv(void)
{
    fd_set read_flag = mask;
    struct timeval timeout;
    timeout.tv_sec = 0;
    timeout.tv_usec = 4;

    if (select(num_sock, (fd_set *)&read_flag, NULL, NULL, &timeout) == -1)
        error_message("select()");

    else if (FD_ISSET(sock, &read_flag)) {
        //fprintf(stderr, "recv_data() ");
        recv_data(&recv_con, sizeof(CONTAINER_S));
        switch (out_con()) {
        case COM_EXIT:
            endflag = 1; return 0;
        case COM_WIN:
            //fprintf(stderr,"WIN  [%d]-[%d]\n", point[0], point[1]);
            recv_flag = 10; break;
        case COM_LOSE:
            //fprintf(stderr,"LOSE [%d]-[%d]\n", point[0], point[1]);
            recv_flag = -1; break;
        case COM_LAUNCH:
            recv_flag = 100; break;
	case COM_BOUND:
            play_sound(M_BOUND); break;
        case COM_SPECIAL:
            recv_flag = 1; break;
	case COM_S_AND_B:
            play_sound(M_BOUND);
            recv_flag = 1; break;
        default:
            recv_flag = 0; break;
        }
    }

    int i;
    /*
    fprintf(stderr, "***********************************\n");
    for (i = 0; i < 6; i++) {
        fprintf(stderr, "    p%d  x:%f\n", i, p[i].x);
    }
    fprintf(stderr, "    PAD x:%f y:%f\n", pad.x, pad.y);
    fprintf(stderr, "***********************************\n");
    */
    return 1;
}

static void set_con(char command)
{
    send_con.com = command;
    send_con.frame = current_frame;
    send_con.x = p[control_id].x;
}

static char out_con(void)
{
    int i;
    if (recv_con.com == COM_WIN || recv_con.com == COM_LOSE) {
        point[0] = recv_con.p[0].x;
        point[1] = recv_con.p[1].x;
        current_frame = 0;
        latest_frame = 0;
        return recv_con.com;
    }

    if (recv_con.frame > latest_frame) {
        recv_con.frame = latest_frame;
        copy_pad(&pad, &recv_con.pad);
        for (i = 0; i < 6; i++)
            copy_player(&p[i], &recv_con.p[i]);
        //printf(stderr, "com=%d\n", recv_con.com);
    }
    //else fprintf(stderr, "pass\n");

    return recv_con.com;
}

static void copy_pad(PAD *a, const PAD *b)
{
    a->x = b->x;
    a->y = b->y;
}

static void copy_player(PLAYER *a, const PLAYER2 *b)
{
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

static int send_thread(void* args)
{
    fprintf(stderr, "send_data()\n");
    send_data(&send_con, sizeof(CONTAINER_C));
    return 0;
}
