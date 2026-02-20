# Projet BertheVario
Firmware récent du GnuVario (https://prunkdump.github.io/GNUVario-TTGO-T5-website/).
- Code C++ Visual Studio/PlateformIO pour alti-vario gps parapente (avec gestion des TMA CTR...).
- Plateforme Espressif ESP32 Dev Module, TTGO T5, Lilygo v2.4, Arduino (processeur 32bits, bi-core 240mhz).

**Utilisation / description :**
- documentation utilisateur dans /doc
- projet PlateformIO dans /BertheVarioPlatformIO
- editer/modifier avec codeblocs dans /BertheVarioCdb
- documentation developpeur dans ../../DoxyBertheVario avec le commande "doxygen Doxyfile.BertheVario" depuis /Codeblocks
- generation de fichier de zones aériennes avec /CompZoneAerienne depuis codeblocks/linux ou windows

**Important :**
- penser à bien modifier votre fichier de configuration pour un fonctionnement correct. En particulier le coefficient du filtre alti-baro (valeurs par defaut dans la documentation ou lorsque le fichier est absent/recréé mode mini-éditeur)

**Pour les développeur :**
- voire la documentation doxygen generée.
- modifier le #define de configuration du capteur magnetique pour position a plat, suspente droite ou gauche.
- fichier version.txt pour modification apportées aux differentes versions.
- modifier "BertheVarioPlatformIO/.pio/libdeps/esp32dev/ESP32 File Manager for Generation Klick ESPFMfGK/src/ESPFMfGKGa.cpp" ligne 6 : crc32.h => CRC32.h
- modifier "BertheVarioPlatformIO/.pio/libdeps/esp32dev/MPU9250/MPU9250.h" ligne 85 : 0x06 => 0x02
