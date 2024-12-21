/**
 * @file logging.h
 * @brief Gestion des logs du serveur
 * @author Valentin Pizzio
 * @date 21/12/2024
 * 
 * Ce fichier définit les fonctions et constantes pour la journalisation
 * des événements du serveur, notamment les interactions avec les clients
 * et les clés de chiffrement du ransomware.
 */

#ifndef LOGGING_H
#define LOGGING_H

#include "protocol.h"

// Chemins des fichiers de logs
#define LOG_DIR "build/logs"
#define LOG_FILE "build/logs/server_log.txt"
#define RANSOMWARE_KEYS_FILE "build/logs/ransomware_keys.log"

/**
 * @brief Crée les répertoires nécessaires pour les logs et les fichiers exfiltrés
 * 
 * Cette fonction crée les répertoires build/, build/logs/ et exfiltration_result/
 * s'ils n'existent pas déjà.
 */
void ensure_directories_exist(void);

/**
 * @brief Enregistre une interaction avec un client dans le fichier de log
 * 
 * @param client_id Identifiant du client
 * @param type Type d'interaction (COMMAND, RANSOMWARE, EXFILTRATION, etc.)
 * @param message Description de l'interaction
 */
void log_interaction(const char* client_id, const char* type, const char* message);

/**
 * @brief Enregistre une clé de chiffrement ransomware dans le fichier dédié
 * 
 * @param client_id Identifiant du client
 * @param directory Répertoire chiffré
 * @param key Clé de chiffrement utilisée
 */
void log_ransomware_key(const char* client_id, const char* directory, const char* key);

#endif // LOGGING_H
