# Projets BertheVario/GnuVario, BertheVarioTac et BertheVario213. 
Vario Gps parapente developés en C/C++ sous Platformio/VisualStudioCode/Code::Blocks.

**BertheVario/GnuVario :**
- firmware pour GnuVario (https://prunkdump.github.io/GNUVario-TTGO-T5-website/0-Accueil.html)
- pieces indisponibles donc pour mise a jour vario existant uniquement.

**BertheVarioTac :**
- Vario a base de ESP32-2432S028 sans la coque.
- documentation pas a jour, un peut lourd pour voler, pieces disponibles.

**BertheVario213 :**
- Vario et coque 3d a base de GDEM0213T5BN.
- documentation pas a jour, le plus abouti des 3 vario.

**Photo des 3 vario :**

<img src="./photos/20260220_083439.jpg" width="250"/>

**Logiciels annexes :**
- CompZoneAerienne : logiciel pour la creation du fichier de zone aerienne (TMA, CTR...). Visualisation des zones par fichier *.png et .kml.
- GestHgt : logiciel pour les fichier *.hgt d'altitude sol et creation/verification checksum.
- IgcReader : logiciel pour relire un fichier igc et le transformer en .kml pour googleearth.
- igc2kmz : logiciel python disponible sur le web pour creer un fichier .kmz googleearth, a partir d'un .igc (couleur fonction de Vz, altitude...)
