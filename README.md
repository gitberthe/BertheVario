# Projet BertheVario
Firmware récent du GnuVario (https://prunkdump.github.io/GNUVario-TTGO-T5-website/).
- Code C++ Visual Studio/PlateformIO pour alti-vario gps parapente.
- Plateforme Espressif EPS32 Dev Module, TTGO T5, Lilygo v2.4, Arduino (processeur 32bits, bi-core 240mhz).

<img src="./GnuVario-Fin.jpg" width="250"/>

* utilisation / description :
- documentation utilisateur dans /doc
- projet PlateformIO dans /BertheVario
- editer/modifier avec codeblocs dans /BertheVarioCdb
- documentation developpeur dans ../../DoxyBertheVario avec le commande "doxygen Doxyfile.BertheVario" depuis /BertheVarioCdb
- generation de fichier reduit de zones aériennes avec /BVZoneAerienne depuis codeblocks/linux
- generation de fichier reduit de zones aériennes avec /BVZoneAerienneW10.exe executable binaire compilé pour Winwows 10
- firmware compilé pour une version "standard écran 1.54 Pcb2 de GnuVario" dans /firmware
 
- pour les developpeur :
 . voire la documentation doxygen generée.
 . attention a bien positionner "#define GNU_VARIO_GRIS" dans BertheVario/src/BertheVario.h avant compilation pour un écran "standard".
 . verifier les #define de debbugage et en partitculier le #define SIMU_VOL qui simul un vol pour validation.
 . fichier version.txt pour modification apportées aux differentes versions.
