/*************************************
  server_net.c
  サーバのネットワークモジュール
  徳島大学 工学部 知能情報工学科 27班
*************************************/

#include "server.h"
#include <SDL/SDL.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>

CLIENT clients[4];
PLAYER p[6];

CONTAINER send_con;
CONTAINER recv_con;

static fd_set mask;
static int num_socks;
static int endflag = 0;

static void set_con(char command);
static char out_con(int cid);
static void copy_pad(PAD *a, const PAD *b);
static void copy_player(PLAYER *a, const PLAYER *b);
static int  recv_data(int cid, void *data, int size);
static void send_data(int cid, void *data, int size);
static void error_message(char *message);
static int recv_thread(void* args);

void setup_server(u_short port)
{
    int rsock, sock = 0;
    struct sockaddr_in sv_addr, cl_addr;
    
    fprintf(stderr, "setup_server() started.\n");
    
    fprintf(stderr, "Generating request socket ...");
    rsock = socket(AF_INET, SOCK_STREAM, 0); 
    if (rsock < 0) error_message("failed!\nsocket()");
    fprintf(stderr, "done.\n");

    sv_addr.sin_family = AF_INET;
    sv_addr.sin_port = htons(port);
    sv_addr.sin_addr.s_addr = INADDR_ANY;
    int opt = 1;
    setsockopt(rsock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    
    fprintf(stderr, "bind() ...");
    if (bind(rsock, (struct sockaddr *)&sv_addr, sizeof(sv_addr)) != 0) error_message("failed!\nbind()");
    fprintf(stderr, "done.\n");

    fprintf(stderr, "listen() ...");
    if (listen(rsock, 4) != 0) error_message("failed!\nlisten()");
    fprintf(stderr, "started.\n");

    int i, max_sock = 0;
    socklen_t len;
    char src[MAX_LEN_ADDR];
    for (i = 0; i < 4; i++) {
        len = sizeof(cl_addr);
        sock = accept(rsock, (struct sockaddr *)&cl_addr, &len);

        if (sock < 0) error_message("accept()");
        
        if (max_sock < sock) max_sock = sock;

        clients[i].cid = i;
        clients[i].sock = sock;
        clients[i].addr = cl_addr;
        memset(src, 0, sizeof(src));
        inet_ntop(AF_INET, (struct sockaddr *)&cl_addr.sin_addr, src, sizeof(src));

        fprintf(stderr, "Client %d is accepted (address=%s, port=%d).\n", i, src, ntohs(cl_addr.sin_port));
    }
    
    close(rsock);

    for (i = 0; i < 4; i++)
        send_data(i, &i, sizeof(int));
    
    num_socks = max_sock + 1;
    FD_ZERO(&mask);
    FD_SET(0, &mask);
    
    for(i = 0; i < 4; i++) {
        FD_SET(clients[i].sock, &mask);
    }
    fprintf(stderr, "Server setup is done.\n");
}

int network(void)
{   
/*
    fd_set read_flag = mask;
    struct timeval timeout;
    timeout.tv_sec = 0;
    timeout.tv_usec = 4;
    int i;

    if (select(num_socks, (fd_set *)&read_flag, NULL, NULL, NULL) == -1)
        error_message("select()");

    for (i = 0; i < 4; i++) {
        if (FD_ISSET(clients[i].sock, &read_flag)) {
            recv_data(i, &recv_con, sizeof(CONTAINER));
            fprintf(stderr, "recv_data() from:%d\n", i);
            if (out_con(i) == COM_EXIT) endflag = 1;

            if (endflag == 0) set_con(COM_NONE);
            else              set_con(COM_EXIT);

            send_data(BROADCAST, &send_con, sizeof(CONTAINER));
            fprintf(stderr, "send_data()   to:%d\n", i);
        }
    }
*/
    SDL_Thread *thread1;
    SDL_Thread *thread2;
    SDL_Thread *thread3;
    SDL_Thread *thread4;
    int arg[4] = {0,1,2,3};

    thread1 = SDL_CreateThread(recv_thread, &arg[0]);
    thread2 = SDL_CreateThread(recv_thread, &arg[1]);
    thread3 = SDL_CreateThread(recv_thread, &arg[2]);
    thread4 = SDL_CreateThread(recv_thread, &arg[3]);
    while(1){
    }
}

static void set_con(char command)
{
    int i;

    send_con.com = command;
    copy_pad(&send_con.pad, &pad);
    for (i = 0; i < 6; i++)
        copy_player(&send_con.p[i], &p[i]);
}

static char out_con(int cid)
{
    p[cid].x = recv_con.p[cid].x;
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

static int recv_data(int cid, void *data, int size)
{
    if ((cid != BROADCAST) && (0 > cid || cid >= 4)) {
        fprintf(stderr,"recv_data(): Client ID is illeagal!\n");
        exit(1);
    }
    if ((data == NULL) || (size <= 0)) {
        fprintf(stderr, "receive_data(); data is illeagal!\n");
        exit(1);
    }
    return read(clients[cid].sock, data, size);
}

static void send_data(int cid, void *data, int size)
{
    if ((cid != BROADCAST) && (0 > cid || cid >= 4))
        ("send_data() failed!\n");
    if ((data == NULL) || (size <= 0))
        error_message("send_data() failed!\n");

    if (cid == BROADCAST) {
        int i;
        for (i = 0; i < 4; i++) {
            if (write(clients[i].sock, data, size) < 0) error_message("write()");
        }
    }
    else {
        if (write(clients[cid].sock, data, size) < 0) error_message("write()");
    }
}

void terminate_server(void)
{
    int i;
    for (i = 0; i < 4; i++) {
        close(clients[i].sock);
    }
    fprintf(stderr, "All connections are closed.\n");
}

static void error_message(char *message)
{
    perror(message);
    fprintf(stderr, "%d\n", errno);
    exit(1);
}

static int recv_thread(void* args)
{
    int i = (int)args;
    while(1) {
        recv_data(i, &recv_con, sizeof(CONTAINER));
        fprintf(stderr, "recv_data() from:%d\n", i);
        if (out_con(i) == COM_EXIT) endflag = 1;

        if (endflag == 0){
            set_con(COM_NONE);
            send_data(i, &send_con, sizeof(CONTAINER));
            fprintf(stderr, "send_data()   to:%d\n", i);
        }
        else {
            set_con(COM_EXIT);
            send_data(i, &send_con, sizeof(CONTAINER));
            fprintf(stderr, "send_data()   to:%d\n", i);
            return 0;
        }
        
    }
    return 0;
}

