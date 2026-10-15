Projet pour creer le fichier des zones aeriennes : zonesaer.txt. A placer dans SDCard/config.

Permet aussi de visionner les zones originales/compressees dans ./zones_gnuplot* et ./zones_kml*

liens de telechargement des zones initiales a enregistrer ./dans data et ./data/openair/ : 
 - pour Pascal Bazille, ./data/20250417_ffvl-cfd.geojson : 
   http://pascal.bazile.free.fr/paraglidingFolder/divers/GPS/OpenAir-Format/
 
 - pour openair, ./data/openair/france.txt, 
                 ./data/openair/sensitivearea_openair.txt, 
                 ./data/openair/spotair-ffvl-zsm.txt : 
   https://planeur-net.github.io/airspace/ 
   https://biodiv-sports.fr/login/?next=/
   https://www.chvd.org/2024/04/26/vol-libre-et-rapace-zsm-et-bulles-de-quietude/
   https://biodiv-sports.fr/api/fr/sensitiveareas/openair?period=ignore
   https://data.spotair.mobi/airspaces/spotair-ffvl-zsm.txt

 - exemple d'utilisation :
    ex pour clermont : ./CompZoneAerienne --point 45.783329 3.08333 120 > zonesaer.txt
    ex pour paillaret: ./CompZoneAerienne --point 45.503415 2.852121 5
    ex mode batch    : ./CompZoneAerienne --batch SDCardAllVario/config/terconnu.txt 60 > zonesaer.txt
    ex mode valid    : ./CompZoneAerienne --valid zonesaer.txt SDCardAllVario/valid/zonvalin.txt
                       pour le mode valid, mettre les fichier *.hgt dans : ../../hgtdata4compzoneaer
                       puis diff valid/zonvalou.txt SDCardAllVario/valid/zonvalou.ref.txt
                       ou ./valid/diff-valid.sh sous linux

 - pour windows, compiler avec code::blocks : 
    https://www.codeblocks.org/downloads/binaries/
    mais CompZoneAerienne n'a ete validé que autour de clermont-ferrand.
