/**
 * @file file_utils.c
 * @brief Implémentation des utilitaires de gestion de fichiers
 * @author Valentin Pizzio
 * @date 21/12/2024
 */

#include <stdio.h>
#include <string.h>
#include <time.h>
#include <direct.h>
#include "file_utils.h"
#include "protocol.h"

#define EXFILTRATION_DIR "exfiltration_result"

/**
 * Sauvegarde le contenu exfiltré dans un fichier.
 * 
 * @param content Le contenu à sauvegarder.
 * @param filename Le nom du fichier d'origine.
 * @param client_id L'identifiant du client.
 */
void save_exfiltrated_content(const char* content, const char* filename, const char* client_id) {
    char filepath[MAX_PATH];
    time_t now;
    struct tm* t;
    const char* dot = strrchr(filename, '.');
    char basename[MAX_FILENAME_LENGTH];
    
    // Extrait le nom de base du fichier (sans extension)
    strncpy(basename, filename, dot ? (dot - filename) : strlen(filename));
    basename[dot ? (dot - filename) : strlen(filename)] = '\0';
    
    // Génère le nom de fichier avec timestamp
    time(&now);
    t = localtime(&now);
    
    snprintf(filepath, sizeof(filepath), "%s\\%s_%s_%02d%02d%02d%02d%02d%02d%s",
             EXFILTRATION_DIR, basename, client_id,
             t->tm_year + 1900, t->tm_mon + 1, t->tm_mday,
             t->tm_hour, t->tm_min, t->tm_sec,
             dot ? dot : "");
    
    // Sauvegarde le contenu dans le nouveau fichier
    FILE* file = fopen(filepath, "w");
    if (file) {
        fputs(content, file);
        fclose(file);
    }
}

/**
 * Liste les fichiers dans le répertoire parent.
 * 
 * @param buffer Le tampon pour stocker la liste des fichiers.
 * @param buffer_size La taille du tampon.
 */
void list_files(char* buffer, size_t buffer_size) {
    WIN32_FIND_DATA findData;
    HANDLE hFind = INVALID_HANDLE_VALUE;
    char search_path[MAX_PATH];
    int count = 1;

    buffer[0] = '\0';
    char line[256];

    // Recherche les fichiers dans le répertoire parent
    snprintf(search_path, sizeof(search_path), "..\\*");
    hFind = FindFirstFile(search_path, &findData);
    
    if (hFind != INVALID_HANDLE_VALUE) {
        do {
            // Liste uniquement les fichiers (pas les répertoires)
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

/**
 * Liste les répertoires dans le répertoire courant.
 * 
 * @param buffer Le tampon pour stocker la liste des répertoires.
 * @param buffer_size La taille du tampon.
 */
void list_directories(char* buffer, size_t buffer_size) {
    WIN32_FIND_DATA findData;
    HANDLE hFind = INVALID_HANDLE_VALUE;
    char search_path[MAX_PATH];
    int count = 1;

    buffer[0] = '\0';
    strcat(buffer, "Available directories:\n");
    char line[256];

    // Recherche les répertoires dans le répertoire courant
    snprintf(search_path, sizeof(search_path), "*");
    hFind = FindFirstFile(search_path, &findData);
    
    if (hFind != INVALID_HANDLE_VALUE) {
        do {
            // Liste uniquement les répertoires
            if ((findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
                // Exclut les répertoires spéciaux et le répertoire build
                if (strcmp(findData.cFileName, ".") != 0 && 
                    strcmp(findData.cFileName, "..") != 0 &&
                    strcmp(findData.cFileName, "build") != 0) {
                    snprintf(line, sizeof(line), "%d. %s\n", count++, findData.cFileName);
                    strncat(buffer, line, buffer_size - strlen(buffer) - 1);
                }
            }
        } while (FindNextFile(hFind, &findData) != 0 && strlen(buffer) < buffer_size - 100);
        FindClose(hFind);
    }
}
