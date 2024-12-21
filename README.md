# Control Your Malware (CYM)

Un projet client-serveur en C implémentant différentes fonctionnalités basique d'un malware.

## Fonctionnalités

1. **Ransomware**
   - Chiffrement de fichiers dans un répertoire spécifié
   - Génération d'une clé de chiffrement aléatoire
   - Signature unique à la fin de chaque fichier chiffré
   - Journalisation des clés de chiffrement avec horodatage et ID client

2. **Exfiltration de Fichiers**
   - Liste et exfiltration de fichiers depuis le client
   - Sauvegarde des fichiers exfiltrés avec horodatage
   - Support de tous types de fichiers

3. **Fork Bomb**
   - Création récursive de processus
   - Surcharge de la mémoire du système cible

## Structure du Projet

```
.
├── build/             # Fichiers exécutables
│   └── logs/         # Fichiers de logs
├── include/          # Fichiers d'en-tête (.h)
├── src/             # Code source (.c)
└── exfiltration_result/ # Fichiers exfiltrés
```

## Compilation

```bash
# Compiler le client
gcc -Wall -Wextra -I./include src/client.c src/ransomware.c src/exfiltration.c -o build/client.exe -lws2_32

# Compiler le serveur
gcc -Wall -Wextra -I./include src/server_main.c src/server.c src/file_utils.c src/logging.c -o build/server.exe -lws2_32
```

## Utilisation

1. Démarrer le serveur :
   ```bash
   ./build/server.exe
   ```

2. Démarrer le client :
   ```bash
   ./build/client.exe
   ```

3. Commandes disponibles :
   - 1: Ransomware
   - 2: Exfiltration
   - 3: Fork bomb
   - 4: Quitter

## Détails Techniques

### Ransomware
- Chiffrement XOR avec clé aléatoire
- Signature : "Rendez-vous tous ou ce sera la guerre - By TR - tel : 04.22.52.10.10"
- Les clés sont stockées dans `build/logs/ransomware_keys.log`

### Exfiltration
- Communication via UDP sur le port 4242
- Les fichiers exfiltrés sont sauvegardés dans `exfiltration_result/`
- Format du nom de fichier : `[nom]_[client_id]_[timestamp].[extension]`

### Journalisation
- Toutes les interactions sont enregistrées dans `build/logs/server_log.txt`
- Format : `[Date/Heure] [Client ID] [Type] [Message]`

## Sécurité

⚠️ AVERTISSEMENT : Ce projet est à des fins éducatives uniquement. L'utilisation malveillante est strictement interdite.

## Dépendances

- Windows (utilise Winsock2)
- GCC (MinGW pour Windows)
- ws2_32.lib (bibliothèque Winsock)

## Auteur

Valentin Pizzio
