/**
 * @file exfiltration.c
 * @brief Implémentation des fonctions d'exfiltration de fichiers
 * @author Valentin Pizzio
 * @date 21/12/2024
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include "exfiltration.h"
#include "protocol.h"

/**
 * Lit le contenu d'un fichier.
 *
 * @param filename Le nom du fichier à lire.
 * @return Le contenu du fichier sous forme de chaîne de caractères, ou NULL en cas d'erreur.
 */
char* read_file_content(const char* filename) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        return NULL;
    }

    // Détermine la taille du fichier
    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    fseek(file, 0, SEEK_SET);

    // Alloue la mémoire pour le contenu
    char* content = (char*)malloc(size + 1);
    if (!content) {
        fclose(file);
        return NULL;
    }

    // Lit le contenu du fichier
    fread(content, 1, size, file);
    content[size] = '\0';
    fclose(file);

    return content;
}

/**
 * Exfiltre le contenu d'un fichier dans un buffer.
 *
 * @param filename Le nom du fichier à exfiltrer.
 * @param buffer Le buffer où stocker le contenu du fichier.
 * @param buffer_size La taille du buffer.
 */
void exfiltrate_file(const char* filename, char* buffer, size_t buffer_size) {
    FILE* file = fopen(filename, "rb");
    if (!file) {
        snprintf(buffer, buffer_size, "Error: Cannot open file %s", filename);
        return;
    }

    // Détermine la taille du fichier
    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    fseek(file, 0, SEEK_SET);

    // S'assure de ne pas dépasser la taille du buffer
    size_t read_size = (file_size < buffer_size - 1) ? file_size : buffer_size - 1;
    
    // Lit le contenu du fichier dans le buffer
    size_t bytes_read = fread(buffer, 1, read_size, file);
    buffer[bytes_read] = '\0';  // Termine la chaîne

    fclose(file);
}
