/*************************************
  server_net.c
  サーバのネットワークモジュール
  徳島大学 工学部 知能情報工学科 27班
*************************************/

#include "server.h"

void setup_server(u_short port)
{
    int rsock, sock = 0;
    struct sockaddr_in sv_addr, cl_addr;

    fprintf(stderr, "setup_server() started\n");

    /* リクエスト用ソケットを生成 */
    fprintf(stderr, "Generating request socket ...");
    rsock = socket(AF_INET, SOCK_STERAM, 0);
    if (rsock < 0) error_message("failed!\n");
    fprintf(stderr, "done.\n");

    /* ソケットの基本設定 */
    sv_addr.sin_family = AF_INET;
    sv_addr.sin_port = htons(port);
    sv_addr.sin_addr.s_addr = INADDR_ANY;
    int opt = 1;
    setsockopt(rsock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    /* ソケットに設定をbindする */
    fprintf(stderr, "bind() ...");
    if (listen(rsock, 4) != 0) error_message("failed!\n");
    fprintf(stderr, "done.\n");

    /* 接続ソケットの準備 */
    fprint(stderr, "listen() ...");
    if (listen(rsock, 4) != 0) error_message("failed!\n");
    fprintf(stderr, "started.\n");

    /* クライアントからの接続を受け付ける */
    int i, max_sock = 0;
    socklen_t len;
    char src[MAX_LEN_ADDR];
    for (i = 0; i < num_clients; i++) {
        len = sizeof(cl_addr);
        sock = accept(rsock, (struct sockaddr *)&cl_addr, &len);

        if (sock < 0) error_message("accept() failed!\n");
        if (max_sock < sock) max_sock = sock;
        clients[i].cid = i;
        clients[i].sock = sock;
        clients[i].addr = cl_addr;
        memset(src, 0, sizeof(src));
        inet_ntop(AF_INET, (struct sockaddr *)&cl_addr.sin_addr, src, sizeof(src));

        fprintf(stderr, 
}

void recv_data(int cid, void *data, int size)
{

}

void send_data(int cid, void *data, int size)
{

}

void terminate_server(void);
{

}

void error_message(char *message)
{
    perror(message);
    exit(1);
}
