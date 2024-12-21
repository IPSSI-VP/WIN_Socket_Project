#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "server.h"
#include "protocol.h"

void clear_stdin(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

int main(void) {
    init_server();
    
    SOCKET socket_fd;
    struct sockaddr_in server_addr, client_addr;
    Message msg;
    char current_client_id[ID_LENGTH] = "";
    
    socket_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (socket_fd == INVALID_SOCKET) {
        printf("Could not create socket\n");
        return 1;
    }
    
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);
    
    if (bind(socket_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == SOCKET_ERROR) {
        printf("Bind failed\n");
        closesocket(socket_fd);
        WSACleanup();
        return 1;
    }
    
    printf("Server listening on UDP port %d...\n", PORT);
    
    while(1) {
        int client_addr_size = sizeof(client_addr);
        int recv_len = recvfrom(socket_fd, (char*)&msg, sizeof(Message), 0,
                               (struct sockaddr*)&client_addr, &client_addr_size);
        
        if (recv_len > 0) {
            if (strlen(current_client_id) == 0) {
                strncpy(current_client_id, msg.client_id, ID_LENGTH);
                printf("\nClient connected with ID: %s\n", current_client_id);
            }
            
            handle_client(socket_fd, client_addr, current_client_id);
        }
    }
    
    closesocket(socket_fd);
    WSACleanup();
    return 0;
}
