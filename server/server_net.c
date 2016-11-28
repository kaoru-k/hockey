/*************************************
  server_net.c
  サーバのネットワークモジュール
  徳島大学 工学部 知能情報工学科 27班
*************************************/

#include "server.h"
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>

void setup_server(u_short port);
void error_message(char *message);

static int num_socks;
static fd_set mask;

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
    
    num_socks = max_sock + 1;
    FD_ZERO(&mask);
    FD_SET(0, &mask);
    
    for(i = 0; i < 4; i++) {
        FD_SET(clients[i].sock, &mask);
    }
    fprintf(stderr, "Server setup is done.\n");
}

void recv_data(int cid, void *data, int size)
{
    if ((cid != BROADCAST) && (0 > cid || cid >= 4))
        error_message("recv_data(): Client ID is wrong!\n");
    
}

void send_data(int cid, void *data, int size)
{

}

void terminate_server(void)
{

}

void error_message(char *message)
{
    perror(message);
    exit(1);
}
