# Projet BertheVario
Firmware récent du GnuVario (https://prunkdump.github.io/GNUVario-TTGO-T5-website/).
- Code C++ Visual Studio/PlateformIO pour alti-vario gps parapente (avec gestion des TMA CTR...).
- Compatible bluetooth / XCTrack si nécessaire.
- Plateforme Espressif ESP32 Dev Module, TTGO T5, Lilygo v2.4, Arduino (processeur 32bits, bi-core 240mhz).

**GnuVario/BertheVario mode vol :**

<img src="./GnuVario-BertheVario-EcranVz.jpg" width="250"/>

**GnuVario/BertheVario mode randonnée :**

<img src="./GnuVario-BertheVario-Rando.jpg" width="250"/>

**Utilisation / description :**
- documentation utilisateur dans /doc
- projet PlateformIO dans /BertheVarioPlatformIO
- editer/modifier avec codeblocs dans /BertheVarioCdb
- documentation developpeur dans ../../DoxyBertheVario avec le commande "doxygen Doxyfile.BertheVario" depuis /Codeblocks
- generation de fichier de zones aériennes avec /CompZoneAerienne depuis codeblocks/linux ou windows
- firmware compilé pour une version "standard écran 1.54 Pcb2 de GnuVario" dans /firmware

**Important :**
- penser à bien modifier votre fichier de configuration pour un fonctionnement correct. En particulier le coefficient du filtre alti-baro (valeurs par defaut dans la documentation ou lorsque le fichier est absent/recréé mode mini-éditeur)

**Pour les développeur :**
- voire la documentation doxygen generée.
- attention a bien positionner "#define GNU_VARIO_GRIS" dans src/BertheVario.h avant compilation pour un écran "standard".
- verifier les #define de debugage et en partitculier le #define SIMU_VOL qui simule un vol pour validation.
- modifier le #define de configuration du capteur magnetique pour position a plat, suspente droite ou gauche.
- fichier version.txt pour modification apportées aux differentes versions.
- modifier "BertheVarioPlatformIO/.pio/libdeps/esp32dev/ESP32 File Manager for Generation Klick ESPFMfGK/src/ESPFMfGKGa.cpp" ligne 6 : crc32.h => CRC32.h
- modifier "BertheVarioPlatformIO/.pio/libdeps/esp32dev/MPU9250/MPU9250.h" ligne 85 : 0x06 => 0x02
- voir le #define XC_TRACK à désactiver si non utilisé car consomme 40ko de memoire.
