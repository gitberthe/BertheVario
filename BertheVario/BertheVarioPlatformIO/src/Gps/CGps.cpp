////////////////////////////////////////////////////////////////////////////////
/// \file CGps.cpp
///
/// \brief
///
/// \date creation   : 03/03/2024
/// \date 09/07/2025 : modification de beep vario sol
/// \date 14/10/2025 : modification StapGps
/// \date 21/10/2025 : nombre de fix altibaro, debut de vol
/// \date 22/10/2025 : Fichier HistoVol toutes les 60s
/// \date 23/10/2025 : suppression de CFinSol
/// \date 26/10/2025 : MakeEmptySound()
/// \date 27/11/2025 : toujours beep recalage altibaro mais plus de bip au demmarage.
/// \date 30/12/2025 : TacheGpsIgcTempsHistoCalcul
///

#ifdef _BERTHE_VARIO_
 #include "../BertheVario.h"
#endif
#ifdef _BERTHE_VARIO_213_
 #include "../BertheVario213.h"
#endif
#ifdef _BERTHE_VARIO_TAC_
 #include "../BertheVarioTac.h"
#endif

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
    xTaskCreatePinnedToCore(TacheGpsSerial, "GpsSerial", SERIAL_GPS_STACK_SIZE, NULL, SERIAL_GPS_PRIORITY,
        GetTaskHandle(SERIAL_GPS_NUM_TASK), SERIAL_GPS_CORE);

// tache de calcul du temps de vol
xTaskCreatePinnedToCore(TacheGpsAttenteVol, "GpsAttenteVol", ATTENTE_VOL_STACK_SIZE , NULL, ATTENTE_VOL_PRIORITY,
    GetTaskHandle(ATTENTE_VOL_NUM_TASK), ATTENTE_VOL_CORE);
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Fonction static qui lit les caracteres du port serie Gps et traite la trame.
void CGps::TacheGpsSerial(void *param)
{
#ifdef STACK_DEBUG
 int MinStack = 32000 ;
#endif

while (g_GlobalVar.m_TaskArr[SERIAL_GPS_NUM_TASK].m_Run)
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

    #ifdef STACK_DEBUG
     static int task = 0 ;
     if ( !(task++%200) )
        {
        int MesStack = uxTaskGetStackHighWaterMark(NULL) ;
        if ( MesStack < MinStack )
            MinStack = MesStack ;
        Serial.printf("Stack Min TacheGpsSerial : %d bytes\n", MinStack);
        }
    #endif
    }

DeleteTask( SERIAL_GPS_NUM_TASK ) ;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Fonction static qui calcul le temps de vol. Elle assure aussi une
/// surveillance de la position pour detection de faux vol.
/// Lancement tache enregistrement IGC.
void CGps::TacheGpsAttenteVol(void *param)
{
#ifdef _LG_DEBUG_
 Serial.println("tache temps de vol lancee");
#endif

#ifdef STACK_DEBUG
 int MinStack = 32000 ;
#endif

// raz temps premier GGA
CTrame::m_MillisPremierGGA = 0 ;
g_GlobalVar.m_VitesseKmh = 0 ;
g_GlobalVar.m_BeepVario = false ;

// affichage gps non pret
g_GlobalVar.m_DureeVolMin = ATTENTE_MESSAGE_GPS ;
g_GlobalVar.m_PileVit.InitPile() ;
g_GlobalVar.m_FinDeVol.InitFinDeVol() ;

// boucle d'attente premier GGA
int iboucle = 0 ;
while ( g_GlobalVar.m_TaskArr[ATTENTE_VOL_NUM_TASK].m_Run )
    {
    // toutes les 1 secondes a 1hz
    delay( 1000 ) ;
    iboucle++ ;

    #ifdef STACK_DEBUG
     int MesStack = uxTaskGetStackHighWaterMark(NULL) ;
     if ( MesStack < MinStack )
        MinStack = MesStack ;
     Serial.printf("Stack Min TacheAttenteVol : %d bytes\n", MinStack );
    #endif

    // temps pour zones periode aeriennes
    g_GlobalVar.m_ZonesAerAll.SetDatePeriode() ;

    // toutes les 7s beep d'attente
    bool beep = !(iboucle%m_BeepSecondes) ;

    // beep attente gps 'G'
    if ( beep && g_GlobalVar.m_BeepVario )
        CGlobalVar::beeper( 1100 , 200 ) ;

    // empilement Vz/Vs pile vitesse
    g_GlobalVar.m_PileVit.PushData() ;

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

// boucle d'attente vitesse minimale verticale ou horizontale
while ( g_GlobalVar.m_TaskArr[ATTENTE_VOL_NUM_TASK].m_Run )
    {
    // toutes les 1 secondes a 1hz
    delay( 1000 ) ;
    iboucle++ ;

    // toutes les 5s beep d'attente
    bool beep = !(iboucle%m_BeepSecondes) ;

    #ifndef TMA_DEBUG
    // temps pour zones periode aeriennes
     g_GlobalVar.m_ZonesAerAll.SetDatePeriode() ;
    #endif // TMA_DEBUG

    // declenchement du vol par bouton droit si ecran 0_Vz
    if ( (g_GlobalVar.GetEtatAuto() == CGestEcrans::ECRAN_0_Vz) && g_GlobalVar.BoutonDroitTousAppui() )
        {
        g_GlobalVar.m_PileVit.SetDebutDeVol( DEBUT_VOL_BOUTON ) ;
        // purge boutons pour eviter un arret vol dans la fouléé
        g_GlobalVar.PurgeBoutons( 3000 ) ;
        // on passe en vol
        break ;
        }

    #ifdef STACK_DEBUG
     int MesStack = uxTaskGetStackHighWaterMark(NULL) ;
     if ( MesStack < MinStack )
        MinStack = MesStack ;
     Serial.printf("Stack Min TacheAttenteVol : %d bytes\n", MinStack);
    #endif

    // empilement Vz/Vs pile vitesse
    g_GlobalVar.m_PileVit.PushData() ;

    // mise a jour des altitudes sol et baro recalee sans mutex carte SD
    g_GlobalVar.MiseAJourAltitudesSolEtBaroRec() ;

    // recalage altibaro autorise
    if ( g_GlobalVar.m_PileVit.CanFixAltiBaro() )
        {
        //if ( g_GlobalVar.m_BeepVario )
            CGlobalVar::beeper( 3000 , 100 ) ;

        // difference altitude sol / altibaro
        #ifdef _BERTHE_VARIO_TAC_
         g_GlobalVar.m_pCapteurPression->SetAltiSolMetres( g_GlobalVar.m_AltitudeSolHgt ) ;
        #else
         g_GlobalVar.m_MS5611Press.SetAltiSolMetres( g_GlobalVar.m_AltitudeSolHgt ) ;
        #endif

        // mise a jour des altitudes sol et baro recalee sans mutex carte SD
        g_GlobalVar.MiseAJourAltitudesSolEtBaroRec() ;

        // calcul du terrain le plus proche en finesses 1hz
        g_GlobalVar.m_TerrainArr.CalcTerrainPlusProche() ;

        // calcul de la zone aerienne
        g_GlobalVar.m_ZonesAerAll.CalcZone() ;
        }

    #ifndef SIMU_VOL
     // si la pile des vitesses n'est pas remplie (ou changement Nb Sat)
     if ( g_GlobalVar.m_PileVit.IsWaitForStab() )
         {
         g_GlobalVar.m_DureeVolMin = ATTENTE_STABILITE_GPS ;

         // beep attente gps 'S'
         if ( beep && g_GlobalVar.m_BeepVario )
             CGlobalVar::beeper( 1500 , 100 ) ;
         }
     else
        {
        // affichage gps pret 'V'
        g_GlobalVar.m_DureeVolMin = ATTENTE_VITESSE_VOL ;

        // beep attente vitesse
        if ( beep && g_GlobalVar.m_BeepVario )
            {
            CGlobalVar::beeper( 1500 , 200 ) ;
            CGlobalVar::beeper( SOUND_DELAY_ONLY , 200 ) ;
            CGlobalVar::beeper( 2000 , 200 ) ;
            }
        }
    #endif

    // si debut de vol cause vitesses verticale ou horizontale
    if ( g_GlobalVar.m_PileVit.IsStartFlight() )
        break ;
    }

// derniere configuration des zones
#ifndef TMA_DEBUG
 g_GlobalVar.m_ZonesAerAll.SetDatePeriode() ;
#endif

// mise a jour des altitudes sol et baro recalee sans mutex carte SD
g_GlobalVar.MiseAJourAltitudesSolEtBaroRec() ;

// mesure de temps de debut du vol
g_GlobalVar.m_MillisDebutVol = millis() ;

// init histo vol
g_GlobalVar.m_HistoVol.m_ZDeco =
g_GlobalVar.m_HistoVol.m_ZMax =
g_GlobalVar.m_HistoVol.m_ZMin = g_GlobalVar.m_TerrainPosCur.m_AltiBaroRec ;
g_GlobalVar.m_HistoVol.m_DebutDeVol = g_GlobalVar.m_PileVit.GetDebutDeVol() ;
// point de vol de decollage
g_GlobalVar.m_TerrainPosDeco = g_GlobalVar.m_TerrainPosCur ;
g_GlobalVar.m_TerrainPosDeco.m_Nom = "decollage" ;

// pour calcul de distance et init histo vol
g_GlobalVar.m_HistoVol.m_LastLat = g_GlobalVar.m_TerrainPosDeco.m_Lat ;
g_GlobalVar.m_HistoVol.m_LastLon = g_GlobalVar.m_TerrainPosDeco.m_Lon ;
g_GlobalVar.m_HistoVol.m_DistanceKm = 0. ;
g_GlobalVar.m_HistoVol.m_VsMax = 0. ;
g_GlobalVar.m_HistoVol.m_VzMax = -10 ;
g_GlobalVar.m_HistoVol.m_VzMin =  10 ;

// pos et temps debut de stationnarite
g_GlobalVar.m_FinDeVol.InitFinDeVol() ;

if ( g_GlobalVar.m_TaskArr[ATTENTE_VOL_NUM_TASK].m_Run )
    {
    // run igc a true
    g_GlobalVar.m_TaskArr[IGC_NUM_TASK].m_Run = true ;

    // bip debut enregistrement
    CGlobalVar::beeper( 4000 , 100 ) ;
    CGlobalVar::beeper( SOUND_DELAY_ONLY , 50 ) ;
    CGlobalVar::beeper( 5000 , 100 ) ;
    CGlobalVar::beeper( SOUND_DELAY_ONLY , 50 ) ;
    CGlobalVar::beeper( 6000 , 100 ) ;

    // lancement tache fichier igc
    xTaskCreatePinnedToCore(TacheGpsIgcTempsHistoCalcul, "IgcTempsHistoCalculTask", IGC_STACK_SIZE , NULL , IGC_PRIORITY ,
        GetTaskHandle(IGC_NUM_TASK) , IGC_CORE);
    }

// attente tache tuee
DeleteTask( ATTENTE_VOL_NUM_TASK ) ;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Fonction static qui enregistre le fichier IGC, l'altitude max , la Vs
/// max et la Vz Min/Max.
/// toutes les secondes
void CGps::TacheGpsIgcTempsHistoCalcul(void *param)
{
#ifdef _LG_DEBUG_
 Serial.println("tache igc lancee");
#endif

#ifdef STACK_DEBUG
 int MinStack = 32000 ;
#endif

// remise a true des beep zone aerienne si desactive lors attente gps
g_GlobalVar.m_BeepVario = true ;

// init fichier IGC
#ifdef _BERTHE_VARIO_TAC_
g_GlobalVar.m_Screen.m_MutexTft.PrendreMutex() ;
#endif
 g_GlobalVar.InitCurentIgc() ;
#ifdef _BERTHE_VARIO_TAC_
g_GlobalVar.m_Screen.m_MutexTft.RelacherMutex() ;
#endif

// positionnement du nom de fichier histo
g_GlobalVar.m_HistoVol.SetNomFichierHisto( g_GlobalVar.GetIgcFileName() ) ;

const int DelayInterTache = 10 ;
int iBoucleFichierHisto = 0 ;
unsigned int Temps1s = 0 ;
while (g_GlobalVar.m_TaskArr[IGC_NUM_TASK].m_Run)
    {
    // boucle temps 1 s
    while ( (millis() - Temps1s) < 1000 )
        delay( DelayInterTache ) ;   // attente 10 ms
    Temps1s = millis() ;

    #ifdef STACK_DEBUG
     int MesStack = uxTaskGetStackHighWaterMark(NULL) ;
     if ( MesStack < MinStack )
        MinStack = MesStack ;
     Serial.printf("Stack Min TacheGpsIgcTempsCalcul : %d bytes\n", MinStack );
    #endif

    // temps de vol en minutes
    unsigned long TempsCourant = millis() ;
    TempsCourant -= g_GlobalVar.m_MillisDebutVol ;
    float DureeVolMinutes = ((float)TempsCourant) / (1000 * 60) ;
    g_GlobalVar.m_DureeVolMin = DureeVolMinutes ;

    // mise a jour alti baro recalee sans mutex carte SD
    //g_GlobalVar.MiseAJourAltitudesSolEtBaroRec( false ) ;

    // enregistrement position dans igc
    #ifdef _BERTHE_VARIO_TAC_
    g_GlobalVar.m_Screen.m_MutexTft.PrendreMutex() ;
    #endif
     g_GlobalVar.PushLoc2Igc() ;
    #ifdef _BERTHE_VARIO_TAC_
    g_GlobalVar.m_Screen.m_MutexTft.RelacherMutex() ;
    #endif

    // enregistrement min/max vitesse ...
    g_GlobalVar.m_HistoVol.PushHistoVol() ;

    // prise en compte position pour fin de vol
    g_GlobalVar.m_FinDeVol.PushPos4FlihgtEnd() ;

    // calcul du terrain le plus proche en finesses 1hz
    g_GlobalVar.m_TerrainArr.CalcTerrainPlusProche() ;

    // calcul de la zone aerienne
    g_GlobalVar.m_ZonesAerAll.CalcZone() ;

    // mise a jour des altitudes sol et baro recalee sans mutex carte SD
    g_GlobalVar.MiseAJourAltitudesSolEtBaroRec() ;

    // fin des tache critiques
    delay( 10 ) ;

    // ecriture historique du vol toutes les 30sec
    #ifndef STACK_DEBUG
     if ( !(iBoucleFichierHisto++%30) )
    #endif
        {
        #ifdef _BERTHE_VARIO_TAC_
        g_GlobalVar.m_Screen.m_MutexTft.PrendreMutex() ;
        #endif
         g_GlobalVar.m_HistoVol.EcritureFichier() ;
        #ifdef _BERTHE_VARIO_TAC_
         g_GlobalVar.m_Screen.m_MutexTft.RelacherMutex() ;
        #endif
        }

    // arret du vol par bouton droit faibles vitesse XY et Z
    if ( g_GlobalVar.GetEtatAuto() == CGestEcrans::ECRAN_0_Vz &&
         g_GlobalVar.BoutonDroitTousAppui() &&
         g_GlobalVar.m_FinDeVol.IsAtGround() )
        {
        // purge boutons pour eviter un relance vol dans la fouléé
        g_GlobalVar.PurgeBoutons( 4000 ) ;
        break ;
        }

    // si fin de vol
    if ( g_GlobalVar.m_FinDeVol.IsFlightEnd() )
        {
        break ;
        }

    // mesure du temps
    #ifdef STACK_DEBUG
     unsigned long ecart = millis() - Temps1s ;
     Serial.print( "ecart < 1s :" ) ;
     Serial.println( ecart ) ;
    #endif

    }

// dernier enregistrement si pas wifi ou autre
#ifdef _BERTHE_VARIO_TAC_
 g_GlobalVar.m_Screen.m_MutexTft.PrendreMutex() ;
#endif
//if ( g_GlobalVar.m_TaskArr[IGC_NUM_TASK].m_Run )
    {
    // force ecriture igc
    g_GlobalVar.PushLoc2Igc(true) ;
    // historique du vol
    g_GlobalVar.m_HistoVol.EcritureFichier() ;
    }
#ifdef _BERTHE_VARIO_TAC_
 g_GlobalVar.m_Screen.m_MutexTft.RelacherMutex() ;
#endif

// beep fin de vol
CGlobalVar::beeper( 6000 , 100 ) ;
CGlobalVar::beeper( SOUND_DELAY_ONLY , 50 ) ;
CGlobalVar::beeper( 5000 , 100 ) ;
CGlobalVar::beeper( SOUND_DELAY_ONLY , 50 ) ;
CGlobalVar::beeper( 4000 , 100 ) ;

// relance attente vol
CGlobalVar::RelancerAttenteVol() ;

// fin de tache
DeleteTask( IGC_NUM_TASK ) ;
}
