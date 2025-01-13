////////////////////////////////////////////////////////////////////////////////
/// \file CGps.cpp
///
/// \brief
///
/// \date creation     : 03/03/2024
/// \date modification : 13/10/2024
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
while( true )
    vTaskDelete(NULL) ;
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
g_GlobalVar.m_DureeVolMin = ATTENTE_MESSAGE_GPS ;

#ifndef TERMIC_DEBUG
// boucle d'attente premier GGA
int iboucle = 0 ;
while ( g_GlobalVar.m_TaskArr[TEMPS_NUM_TASK].m_Run )
    {
    // toutes les 1 secondes a 1hz
    delay( 1000 ) ;
    iboucle++ ;

    // temps pour zones periode aeriennes
    g_GlobalVar.m_ZonesAerAll.SetDatePeriode() ;

    // toutes les 7s beep d'attente
    bool beep = !(iboucle%m_BeepSecondes) ;

    // beep attente gps 'G'
    if ( beep && g_GlobalVar.m_BeepAttenteGVZone )
        CGlobalVar::beeper( 1100 , 200 ) ;

    #ifdef REBOOT_DEBUG
     break ;
    #endif

    // si pas de GGA attente
    if ( m_MillisPremierGGA == 0 )
        {
        // pas de memorisation du depart de vol par bouton droit
        if ( g_GlobalVar.GetEtatAuto() == CGestEcrans::ECRAN_0_Vz )
            g_GlobalVar.RazBoutonDroit() ;
        continue ;
        }
    else
        break ;
    }

// etat du GPS
g_GlobalVar.m_DureeVolMin = ATTENTE_STABILITE_GPS ;

// boucle d'attente vitesse minimale
int iVz = 0 ;
while ( g_GlobalVar.m_TaskArr[TEMPS_NUM_TASK].m_Run )
    {
    // toutes les 1 secondes a 1hz
    delay( 1000 ) ;
    iboucle++ ;

    // toutes les 7s beep d'attente
    bool beep = !(iboucle%m_BeepSecondes) ;

    #ifdef REBOOT_DEBUG
     break ;
    #endif

    #ifndef TMA_DEBUG
    // temps pour zones periode aeriennes
     g_GlobalVar.m_ZonesAerAll.SetDatePeriode() ;
    #endif // TMA_DEBUG

    // declenchement du vol par bouton droit si ecran 0_Vz
    if ( g_GlobalVar.GetEtatAuto() == CGestEcrans::ECRAN_0_Vz && g_GlobalVar.BoutonDroit() )
        {
        // raz difference altitude presion/wgs84 = altitude affichée est barometrique pure
        if ( ! g_GlobalVar.m_StabGps.IsGpsStable() )
            g_GlobalVar.m_MS5611.SetAltiSolUndef() ;

        // purge boutons pour eviter un arret vol dans la fouléé
        g_GlobalVar.PurgeBoutons( 6000 ) ;
        // on passe en vol
        break ;
        }

    // si le gps nest pas stable au moins une fois (10 secondes)
    // mais on ne le refait pas si second vol, uniquement 1 apres Mise Sous Tension
    #ifndef SIMU_VOL
     g_GlobalVar.m_StabGps.PushGpPos4Stab() ;
     // si pas attente vitesse
     if ( g_GlobalVar.m_DureeVolMin == ATTENTE_STABILITE_GPS && ! g_GlobalVar.m_StabGps.IsAlwaysStable() )
         {
         // beep attente gps 'S'
         if ( beep && g_GlobalVar.m_BeepAttenteGVZone )
             CGlobalVar::beeper( 1500 , 100 ) ;

         // recalage altibaro
         g_GlobalVar.m_MutexVariable.PrendreMutex() ;
          g_GlobalVar.m_MS5611.SetAltiSolMetres( g_GlobalVar.m_AltitudeSolHgt ) ;
         g_GlobalVar.m_MutexVariable.RelacherMutex() ;

         // si le gps n'est pas stable
         if ( ! g_GlobalVar.m_StabGps.IsGpsStable() )
            {
            g_GlobalVar.m_PileVit.ResetVit() ;
            // on reste en stabilisation gps
            continue ;
            }
         }
    #endif

    // affichage gps pret
    g_GlobalVar.m_DureeVolMin = ATTENTE_VITESSE_VOL ;

    // beep attente vitesse
    g_GlobalVar.m_PileVit.PusGpsVit() ;
    if ( beep && g_GlobalVar.m_BeepAttenteGVZone )
        {
        CGlobalVar::beeper( 1500 , 200 ) ;
        CGlobalVar::beeper( SOUND_DELAY_ONLY , 200 ) ;
        CGlobalVar::beeper( 2000 , 200 ) ;
        }

    // si debut de vol cause vitesses
    if ( g_GlobalVar.m_PileVit.IsStartFlight() )
        break ;

    // si vitesse verticale depassee pendant 4 secondes declenchement igc
    if ( fabs(g_GlobalVar.m_VitVertMS) >= g_GlobalVar.m_Config.m_vz_igc_ms )
        {
        iVz++ ;
        if ( iVz >= g_GlobalVar.m_Config.m_temps_igc_sec )
            break ;
        }
    else
        iVz = 0 ;

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
CGlobalVar::beeper( 7000 , 100 ) ;
CGlobalVar::beeper( SOUND_DELAY_ONLY , 200 );
CGlobalVar::beeper( 6800 , 100 ) ;
CGlobalVar::beeper( SOUND_DELAY_ONLY , 200) ;
CGlobalVar::beeper( 7000 , 100 ) ;

// pos et temps debut de stationnarite
g_GlobalVar.m_FinDeVol.InitFinDeVol() ;

// mise a jour du temps de vol toutes les secondes
// mise a jour histo toutes les 2 secondes
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

    // historique du vol toutes les 2 sec
    if ( !(iboucleHistoVol%2) )
        g_GlobalVar.m_HistoVol.EcritureFichier( g_GlobalVar.GetIgcFileName() ) ;
    iboucleHistoVol++ ;
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
    delay( 500 ) ;

    // prise en compte postion pour fin de vol
    g_GlobalVar.m_FinDeVol.PushPos4FlihgtEnd() ;
    delay( 500 ) ;

    // arret du vol par bouton droit
    if ( g_GlobalVar.GetEtatAuto() == CGestEcrans::ECRAN_0_Vz && g_GlobalVar.BoutonDroit()
         && g_GlobalVar.m_VitesseKmh < 5. && fabsf(g_GlobalVar.m_VitVertMS) < 0.4 )
        {
        // beep prise en compte relance
        CGlobalVar::beeper( 6000 , 300 ) ;
        // relance igc
        CGlobalVar::RelancerEnregistrementFichier() ;
        // purge boutons pour eviter un relance vol dans la fouléé
        g_GlobalVar.PurgeBoutons( 4000 ) ;
        break ;
        }

    // si fin de vol
    if ( g_GlobalVar.m_FinDeVol.IsFlightEnd() )
        {
        CGlobalVar::RelancerEnregistrementFichier() ;
        break ;
        }
    }

g_GlobalVar.m_TaskArr[IGC_NUM_TASK].m_Stopped = true ;
while( true )
    vTaskDelete(NULL) ;
}
