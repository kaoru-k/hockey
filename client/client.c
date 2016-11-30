/*************************************
  client_main.c
  徳島大学 工学部 知能情報工学科 27班
*************************************/

#include "client.h"

int main(int argc, char *argv[])
{
    u_short port = DEFAULT_PORT;
    char server_name[50];
    int flag = 1;

    sprintf(server_name, "localhost");

    switch(argc) {
    case 1:
        break;
    case 2:
        sprintf(server_name, "%s", argv[1]);
        break;
    case 3:
        sprintf(server_name, "%s", argv[1]);
        break;
    default:
        fprintf(stderr, "Too many argument!\n");
        return 1;
    }
    
    setup_client(server_name, port);

    while(flag) {
        
    }

    terminate_client();

    return 0;
}
    
