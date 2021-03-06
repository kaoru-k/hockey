/*************************************
  server_net.c
  サーバのネットワークモジュール
  徳島大学 工学部 知能情報工学科 27班
*************************************/

#include "server.h"
#include <time.h>
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
CONTAINER_S send_con;
CONTAINER_C recv_con;
int client_frame[4] = {0};
int current_frame = 0;

static fd_set mask;
static int num_socks;
int s_flag[4] = {0};
int start_flag = 0;
int reset_flag = 0;
int sound_flag = 0;
int max_point;

static void set_con(char command);
static char out_con(int cid);
static void copy_pad(PAD *a, const PAD *b);
static void copy_player(PLAYER2 *a, const PLAYER *b);
static int  recv_data(int cid, void *data, int size);
static void send_data(int cid, void *data, int size);
static void error_message(char *message);
static int  rand_type(int x);

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
    if (listen(rsock, num_clients) != 0) error_message("failed!\nlisten()");
    fprintf(stderr, "started.\n");

    int i, max_sock = 0;
    socklen_t len;
    char src[MAX_LEN_ADDR];
    for (i = 0; i < num_clients; i++) {
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

    for (i = 0; i < num_clients; i++)
        send_data(i, &i, sizeof(int));

    num_socks = max_sock + 1;
    FD_ZERO(&mask);
    FD_SET(0, &mask);
    
    for(i = 0; i < num_clients; i++) {
        FD_SET(clients[i].sock, &mask);
    }
    
    fprintf(stderr, "Server setup is done.\n");
}

void setting_server(void)
{
    int i, j;
    int flag[2][2] = {0};
    SETTING  setting[4];
    SETTING2 setting2;

    fprintf(stderr, "Receive settings...");
    for (i = 0; i < num_clients; i++) {
        recv_data(i, &setting[i], sizeof(SETTING));
        //fprintf(stderr, "%d ", i);
    }

    for (i = num_clients; i < 4; i++)
        setting[i].chara = 0;
    
    fprintf(stderr, "done.\n");

    max_point = setting[0].point;
    
    for (i = 0; i < 4; i++) {
        if (i == 0 || i == 1) j = 0;
        else                  j = 1;
        
        switch(setting[i].chara) {
        case 0:
        case 1:
            if (!flag[j][0]) {
                if (j == 0) {
                    clients[i].control = 0;
                    p[0].cid  = i;
                    p[0].type = setting[i].chara;
                }
                else {
                    clients[i].control = 2;
                    p[2].cid  = i;
                    p[2].type = setting[i].chara;
                }
                flag[j][0] = 1;
            }
            else {
                if (j == 0) {
                    clients[i].control = 1;
                    p[1].cid  = i; 
                    p[1].type = rand_type(1);
                }
                else {
                    clients[i].control = 3;
                    p[3].cid  = i;
                    p[3].type = rand_type(1);
                }
            }
            break;
        case 2:
        case 3:
            if (!flag[j][1]) {
                if (j == 0) {
                    clients[i].control = 1;
                    p[1].cid  = i;
                    p[1].type = setting[i].chara;
                }
                else {
                    clients[i].control = 3;
                    p[1].cid  = i;
                    p[3].type = setting[i].chara;
                }
                flag[j][1] = 1;
            }
            else {
                if (j == 0) {
                    clients[i].control = 0;
                    p[0].cid  = i;
                    p[0].type = rand_type(0);
                }
                else {
                    clients[i].control = 2;
                    p[0].cid  = i;
                    p[2].type = rand_type(0);
                }
            }
            break;
        }
    }
    p[4].type = 4;
    p[5].type = 4;

    //fprintf(stderr, "max_point: %d\n", max_point);
    setting2.point = max_point;
    for (i = 0; i < 6; i++) {
        //fprintf(stderr, "p[%d] cid:%d type:%d\n", i, p[i].cid, p[i].type);
        setting2.type[i] = p[i].type;
    }
    
    fprintf(stderr, "Sending settings... ", max_point);
    for (i = 0; i < num_clients; i++) {
        send_data(i, &clients[i].control, sizeof(int));
        send_data(i, &setting2, sizeof(SETTING2));
    }
    fprintf(stderr, "done.\nWaiting clients... ", max_point);
    
    for (i = 0; i < num_clients; i++) {
        recv_data(i, &setting[i], sizeof(SETTING));
        //fprintf(stderr, "%d ", i);
    }
    fprintf(stderr, "done\n", max_point);

}

int network(void)
{
    fd_set read_flag = mask;
    struct timeval timeout;
    timeout.tv_sec  = 0;
    timeout.tv_usec = 4;
    int i;
    int result = 1;

    if (select(num_socks, (fd_set *)&read_flag, NULL, NULL, &timeout) == -1)
        error_message("select()");
    
    for (i = 0; i < num_clients; i++) {
        if (FD_ISSET(clients[i].sock, &read_flag)) {
            recv_data(i, &recv_con, sizeof(CONTAINER_C));
            
            switch (out_con(i)) {
            case COM_EXIT :
                endflag = 1;
                break;
            case COM_SPECIAL :
                if (reset_flag == -1) endflag = 1;
		else s_flag[i] = 1;
                break;
            case COM_START :
                if (reset_flag == -1) reset_flag = 1;
                else start_flag = 1;
                break;
            default:
                s_flag[i] = 0;
                break;
            }

            
            if (endflag) {
		set_con(COM_EXIT);
                result = 0;
            }
            else if (game.scene == 2 && (game.point[0] != 0 || game.point[1] != 0) && reset_flag != -1) {
                for (i = 0; i < num_clients; i++) {
                    if      (win == 0 && clients[i].cid == 2)
                        set_con(COM_LAUNCH);
                    else if (win == 1 && clients[i].cid == 0)
                        set_con(COM_LAUNCH);
                    else if (num_clients < 3)
                        set_con(COM_LAUNCH);
                    else
                        set_con(COM_NONE);

                    send_data(i, &send_con, sizeof(CONTAINER_S));
                }
                return 1;
            }
            else if (reset_flag == -1) {
                for (i = 0; i < num_clients; i++) {
                    if ( (win == 0 && (i == 0 || i == 1)) || (win == 1 && (i == 2 || i == 3)) ) 
                        send_con.com = COM_QUE_Y;
                    else
                        send_con.com = COM_QUE_Z;
                    
                    send_data(i, &send_con, sizeof(CONTAINER_S));
                }
                return 1;
            }
            else if (reset_flag == 1) {
                reset_flag = 0;
                for (i = 0; i < 4; i++)
                    client_frame[i] = 0;
		current_frame = 0;

                set_con(COM_ALLRESET);
            }
            else if (game.scene == 1) {
                for (i = 0; i < 4; i++)
                    client_frame[i] = 0;
		current_frame = 0;

                send_con.p[0].x = game.point[0];
                send_con.p[1].x = game.point[1];
                for (i = 0; i < num_clients; i++) {
                    if ( (win == 0 && (i == 0 || i == 1)) || (win == 1 && (i == 2 || i == 3)) ) 
                        send_con.com = COM_WIN;
                    else
                        send_con.com = COM_LOSE;
                    
                    send_data(i, &send_con, sizeof(CONTAINER_S));
                }
                return 1;
            }
            else if (hst_flag()) {
                if (sound_flag) {
                    sound_flag = 0;
                    set_con(COM_S_AND_B);
                }
		else
                    set_con(COM_SPECIAL);
                result = 1;
            }
            else if (sound_flag) {
                sound_flag = 0;
                set_con(COM_BOUND);
                result = 1;
            }
            else {
                set_con(COM_NONE);
                result = 1;
            }

            send_data(BROADCAST, &send_con, sizeof(CONTAINER_S));
            //fprintf(stderr, "send_data()   to:%d com:%c\n", i, send_con.com);
        }
    }
    return result;
}

int s_on(void)
{
    int i;
    for (i = 0; i < num_clients; i++) {
        if (s_flag[i] == 1) return i;
    }
    return -1;
}


static void set_con(char command)
{
    int i;
    send_con.frame = current_frame;
    send_con.com = command;
    copy_pad(&send_con.pad, &pad);
    for (i = 0; i < 6; i++)
        copy_player(&send_con.p[i], &p[i]);
}

static char out_con(int cid)
{
    if (client_frame[cid] < recv_con.frame) {
        client_frame[cid] = recv_con.frame;
        if (p[clients[cid].control].hp > 0)
            p[clients[cid].control].x = recv_con.x;
        //fprintf(stderr, "recv_data() from:%d com:%c\n", cid, recv_con.com);
    }
    //else fprintf(stderr, "recv_data() from:%d pass\n", cid);
    
    return recv_con.com;
}

static void copy_pad(PAD *a, const PAD *b)
{
    a->x = b->x;
    a->y = b->y;
}

static void copy_player(PLAYER2 *a, const PLAYER *b)
{
    a->hp   = b->hp;
    a->ap   = b->ap;
    a->x    = b->x;
}

static int recv_data(int cid, void *data, int size)
{
    if ((cid != BROADCAST) && (0 > cid || cid >= num_clients)) {
        fprintf(stderr,"recv_data(): Client ID is illeagal!\n");
        exit(1);
    }
    if ((data == NULL) || (size <= 0)) {
        fprintf(stderr, "recv_data(); data is illeagal!\n");
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
        for (i = 0; i < num_clients; i++) {
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
    for (i = 0; i < num_clients; i++) {
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

static int rand_type(int x)
{
    srand((unsigned) time(NULL));
    
    switch(x) {
    case 0:
        return (rand() % 2);
    case 1:
        return (rand() % 2) + 2;
    }
}
