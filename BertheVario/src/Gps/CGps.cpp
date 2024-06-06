////////////////////////////////////////////////////////////////////////////////
/// \file CGps.cpp
///
/// \brief
///
/// \date creation     : 03/03/2024
/// \date modification : 06/06/2024
///

#include "../BertheVario.h"

////////////////////////////////////////////////////////////////////////////////
/// \brief Initialisation du port serie avec les pin qui vont bien.
void CGps::InitGps()
{
Serial2.begin( 9600 , SERIAL_8N1 , pinGpsRXD , pinGpsTXD ) ;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Lance les tache de gestion du Gps : acquisition serie et gestion du
/// temps de vol / fichier igc.
void CGps::LanceTacheGps( bool AvecPortSerie )
{
#ifdef _LG_DEBUG_
 Serial.println("lancement tache gps");
#endif

// tache de port serie
if ( AvecPortSerie )
    xTaskCreatePinnedToCore(TacheGpsSerial, "GpsSerial", SERIAL_GPS_STACK_SIZE, this, SERIAL_GPS_PRIORITY,NULL, SERIAL_GPS_CORE);

// tache de calcul du temps de vol
xTaskCreatePinnedToCore(TacheGpsTempsVol, "GpsTempsVol", TEMPS_STACK_SIZE , this, TEMPS_PRIORITY, NULL, TEMPS_CORE);
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Fonction static qui lit les caracteres du port serie Gps et traite la trame.
void CGps::TacheGpsSerial(void *param)
{
while (g_GlobalVar.m_TaskArr[SERIAL_NUM_TASK].m_Run)
    {
    while (Serial2.available())
        {
        char c = Serial2.read() ;
        #ifdef GPS_DEBUG
         Serial.print(c);
        #endif
        g_GlobalVar.Push( c ) ;
        }
    delay(2) ;
    }
g_GlobalVar.m_TaskArr[SERIAL_NUM_TASK].m_Stopped = true ;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Fonction static qui calcul le temps de vol. Elle assure aussi une
/// surveillance de la position pour detection de faux vol.
/// Lancement tache enregistrement IGC.
void CGps::TacheGpsTempsVol(void *param)
{
#ifdef _LG_DEBUG_
 Serial.println("tache temps de vol lancee");
#endif

// raz temps premier GGA
CTrame::m_MillisPremierGGA = 0 ;
g_GlobalVar.m_VitesseKmh = 0 ;

// affichage gps non pret
g_GlobalVar.m_DureeVolMin = ATTENTE_GPS ;

#ifndef TERMIC_DEBUG
// boucle d'attente premier GGA et vitesse minimale
int iboucle = 0 ;
int ivitesse = 0 ;
while ( g_GlobalVar.m_TaskArr[TEMPS_NUM_TASK].m_Run )
    {
    // toutes les 1 secondes a 1hz
    delay( 1000 ) ;
    iboucle++ ;

    // toutes les 7s beep d'attente
    bool beep = !(iboucle%7) ;

    // beep attente gps 'G'
    if ( beep && g_GlobalVar.m_BeepAttenteGVZone )
        CGlobalVar::beeper( 1500 , 100 ) ;

    #ifdef REBOOT_DEBUG
     break ;
    #endif

    // si pas de GGA attente
    if ( m_MillisPremierGGA == 0 )
        {
        ivitesse = 0 ;
        continue ;
        }

    // recalage alti pression 1 fois par secondes
    g_GlobalVar.m_MutexVariable.PrendreMutex() ;
     g_GlobalVar.m_MS5611.SetAltiSolMetres( g_GlobalVar.m_AltiSolHgt ) ;
    g_GlobalVar.m_MutexVariable.RelacherMutex() ;

    #ifndef TMA_DEBUG
    // temps pour zones periode aeriennes
     g_GlobalVar.m_ZonesAerAll.SetDatePeriode() ;
    #endif // TMA_DEBUG

    // declenchement du vol par bouton droit si ecran 0
    if ( g_GlobalVar.BoutonDroit() && g_GlobalVar.GetEtatAuto() == CGestEcrans::ECRAN_0_Vz )
        break ;

    // si le gps nest pas stable au moins une fois (40 secondes)
    #ifndef SIMU_VOL
     g_GlobalVar.PushGpPos() ;
     if ( g_GlobalVar.m_DureeVolMin == ATTENTE_GPS && (! g_GlobalVar.IsGpsStable()) )
         {
         ivitesse = 0 ;
         continue ;
         }
    #endif

    // beep attente vitesse
    if ( beep && g_GlobalVar.m_BeepAttenteGVZone )
        {
        delay( 200 ) ;
        CGlobalVar::beeper( 2000 , 100 ) ;
        }

    // affichage gps pret
    g_GlobalVar.m_DureeVolMin = ATTENTE_VITESSE ;

    // si vitesse superieur a 16 kmh
    if ( g_GlobalVar.m_VitesseKmh >= g_GlobalVar.m_Config.m_vitesse_igc_kmh )
        ivitesse++;
    else
        ivitesse = 0 ;

    // au bout de 5 vitesses/5 secondes depassée
    if ( ivitesse >= 5 )
        break ;

    #ifdef SOUND_DEBUG
     break ;
    #endif
    }
#endif

// derniere configuration des zones
#ifndef TMA_DEBUG
 g_GlobalVar.m_ZonesAerAll.SetDatePeriode() ;
#endif

// mesure de temps de debut du vol
g_GlobalVar.m_MillisDebutVol = millis() ;

// point de vol de decollage
g_GlobalVar.m_HistoVol.m_ZDeco =
g_GlobalVar.m_HistoVol.m_ZMax = g_GlobalVar.m_TerrainPosCur.m_AltiBaro ;
g_GlobalVar.m_TerrainPosDeco = g_GlobalVar.m_TerrainPosCur ;
g_GlobalVar.m_TerrainPosDeco.m_Nom = "decollage" ;

// pour calcul de distance et init histo vol
g_GlobalVar.m_HistoVol.m_LastLat = g_GlobalVar.m_TerrainPosDeco.m_Lat ;
g_GlobalVar.m_HistoVol.m_LastLon = g_GlobalVar.m_TerrainPosDeco.m_Lon ;
g_GlobalVar.m_HistoVol.m_DistanceKm = 0. ;
g_GlobalVar.m_HistoVol.m_VsMax = 0. ;
g_GlobalVar.m_HistoVol.m_VzMax = -10 ;
g_GlobalVar.m_HistoVol.m_VzMin =  10 ;

// lancement tache fichier igc
xTaskCreatePinnedToCore(TacheGpsIgc, "IgcTask", IGC_STACK_SIZE , & g_GlobalVar , IGC_PRIORITY , NULL, IGC_CORE);

// bip debut enregistrement
CGlobalVar::BeepOk() ;
delay(200);
CGlobalVar::BeepOk() ;
delay(200);
CGlobalVar::BeepOk() ;

// pos et temps debut de stationnarite
g_GlobalVar.InitFauxDepart( g_GlobalVar.m_TerrainPosDeco.m_Lat , g_GlobalVar.m_TerrainPosDeco.m_Lon , millis() ) ;

// mise a jour du temps de vol toutes les secondes
int iboucleHistoVol = 0 ;
while (g_GlobalVar.m_TaskArr[TEMPS_NUM_TASK].m_Run)
    {
    // a 1 hz
    delay( 1000 ) ;

    // toutes les secondes

    // temps de vol en minutes
    unsigned long TempsCourant = millis() ;
    TempsCourant -= g_GlobalVar.m_MillisDebutVol ;

    TempsCourant /= 1000 * 60 ;
    g_GlobalVar.m_DureeVolMin = TempsCourant ;

    // histo vol
    float Distance = g_GlobalVar.m_HistoVol.PushHistoVol() ;

    // pour la finesse sol
    g_GlobalVar.PushDistAlti( Distance , g_GlobalVar.m_TerrainPosCur.m_AltiBaro ) ;

    // si faux depart de vol
    #ifndef SOUND_DEBUG
    if ( g_GlobalVar.IsBadFlightBegin() )
        {
        CGlobalVar::RelancerEnregistrementFichier() ;
        break ;
        }
    #endif

    // historique du vol toutes les 5 sec
    if ( !((iboucleHistoVol++)%5) )
        g_GlobalVar.m_HistoVol.EcritureFichier() ;
    }

g_GlobalVar.m_TaskArr[TEMPS_NUM_TASK].m_Stopped = true ;
while( true )
    vTaskDelete(NULL) ;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Fonction static qui enregistre le fichier IGC, l'altitude max , la Vs
/// max et la Vz Min/Max.
/// toutes les secondes
void CGps::TacheGpsIgc(void *param)
{
#ifdef _LG_DEBUG_
 Serial.println("tache igc lancee");
#endif

// init fichier IGC
g_GlobalVar.InitCurentIgc() ;

while (g_GlobalVar.m_TaskArr[IGC_NUM_TASK].m_Run)
    {
    // enregistrement position dans igc
    g_GlobalVar.PushLoc2Igc() ;

    // toutes les secondes
    delay( 1000 ) ;
    }

g_GlobalVar.m_TaskArr[IGC_NUM_TASK].m_Stopped = true ;
while( true )
    vTaskDelete(NULL) ;
}
