////////////////////////////////////////////////////////////////////////////////
/// \file CVarioCapBeep.h
///
/// \brief
///
/// \date creation     : 03/10/2024
/// \date modification : 08/07/2025 suppression xc_track et limite zone xy alti
/// \date modification : 09/07/2025 modification beep vario
/// \date modification : 28/08/2025
/// \date 27/11/2025 : plus de beep zerotage
///

#include "../BertheVarioTac.h"

////////////////////////////////////////////////////////////////////////////////
/// \brief Lance la tache de mesure des capteurs et calcul de la VZ.
void CVarioCapBeep::LanceTacheVarioCapBeep()
{
g_GlobalVar.m_TaskArr[VARIOCAP_NUM_TASK].m_Run = true ;
xTaskCreatePinnedToCore(TacheVarioCapteur, "VarioCapteur", VARIOCAP_STACK_SIZE , this, VARIOCAP_PRIORITY,
    GetTaskHandle(VARIOCAP_NUM_TASK), VARIOCAP_CORE);

delay( 500 ) ;

g_GlobalVar.m_TaskArr[VARIOBEEP_NUM_TASK].m_Run = true ;
xTaskCreatePinnedToCore(TacheGenereSonVario, "VarioBeep", VARIOBEEP_STACK_SIZE , this, VARIOBEEP_PRIORITY,
    GetTaskHandle(VARIOBEEP_NUM_TASK), VARIOBEEP_CORE);
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Tache de mesure des capteurs et calcul de la VZ.
void CVarioCapBeep::TacheVarioCapteur(void* param)
{
// variables
const int DIV_SECONDES = 3 ;
float AltiPressForVzArr[DIV_SECONDES+1] ;

// premiere altitude
g_GlobalVar.m_pCapteurPression->MesureAltitudeCapteur() ;
float AltiBaroFiltree = g_GlobalVar.m_TerrainPosCur.m_AltiBaroRec = g_GlobalVar.m_pCapteurPression->GetAltiBaroPureMetres() ;

// pile des alti baro
for ( int i = 0 ; i <= DIV_SECONDES ; i++ )
    AltiPressForVzArr[i] = AltiBaroFiltree ;

// boucle Vz et cap magnetique
unsigned long time = millis() ;
int Delay3Hz = 1000/DIV_SECONDES ;
while (g_GlobalVar.m_TaskArr[VARIOCAP_NUM_TASK].m_Run)
    {
    // a 3 hz
    if ( (millis()-time) < Delay3Hz )
        {
        delay( 10 ) ;
        continue ;
        }
    time = millis() ;

    // lecture cap magnetique
    g_GlobalVar.m_QMC5883Mag.LectureCap() ;

    // mesure altitude capteur
    const float coef = g_GlobalVar.m_Config.m_coef_filtre_alti_baro ;
    g_GlobalVar.m_pCapteurPression->MesureAltitudeCapteur() ;

    // calcul altibaro pure filtree
    AltiBaroFiltree = (1.-coef) * g_GlobalVar.m_pCapteurPression->GetAltiBaroPureMetres() + coef * AltiBaroFiltree ;

    // mise a jour altitude recalee
    //g_GlobalVar.m_TerrainPosCur.m_AltiBaroRec = g_GlobalVar.m_pCapteurPression->GetAltiMetres() ;

    // decalage du tableau alti fifo par 0 sur 1 secondes
    for ( int i = DIV_SECONDES ; i>0 ; i-- )
        AltiPressForVzArr[ i ] = AltiPressForVzArr[ i - 1 ] ;
    AltiPressForVzArr[ 0 ] = AltiBaroFiltree ;

    // calcul difference alti baro pure sur 1 seconde pour Vz
    float DiffAlti = AltiPressForVzArr[ 0 ] - AltiPressForVzArr[ DIV_SECONDES ] ;

    // recopie de Vz
    g_GlobalVar.m_VitVertMS = DiffAlti ;
    }

DeleteTask( VARIOCAP_NUM_TASK ) ;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Fonction static core 0 qui gere les beep.
void CVarioCapBeep::TacheGenereSonVario(void *param)
{
const float LowFreq = 1000 ;
const float MinFreq = 1200 ;
const float MaxFreq = 8000 ;
const float SeuilVzMaxBeep    = g_GlobalVar.m_Config.m_vz_seuil_max ;
const float SeuilVzMinBeep    = g_GlobalVar.m_Config.m_vz_seuil_haut ;
const float SeuilVzDeguelante = g_GlobalVar.m_Config.m_vz_seuil_bas ;
while (g_GlobalVar.m_TaskArr[VARIOBEEP_NUM_TASK].m_Run)
    {
    // desactivation du son cause TMA
    //bool NotActive = g_GlobalVar.m_ZonesAerAll.m_DansDessousUneZone == ZONE_DEDANS ; //||
                     //g_GlobalVar.m_ZonesAerAll.m_LimiteZone == ZONE_LIMITE_ALTI ||
                     //g_GlobalVar.m_ZonesAerAll.m_LimiteZone == ZONE_LIMITE_FRONTIERE ;

    // si alarme zone desactivé => son vario active
    //if ( !g_GlobalVar.m_BeepAttenteGVZone )
    //    NotActive = false ;

    /*// si son vario desactive
    if ( !g_GlobalVar.m_BeepVario )
        {
        delay( 500 ) ;
        continue ;
        }*/

    float LocalVitVertMS = g_GlobalVar.m_VitVertMS ;
    //LocalVitVertMS = 0.5 ;
    //float LocalVitVertMS = g_GlobalVar.m_Config.m_vz_seuil_haut ;
    #ifdef SOUND_DEBUG
     LocalVitVertMS = 6 ;
    #endif

    // si degueulante
    if ( LocalVitVertMS <= SeuilVzDeguelante )
        {
        g_GlobalVar.beeper( LowFreq , 200 ) ;
        delay( 400 ) ;
        continue ;
        }
    // descente normale
    else if ( LocalVitVertMS > SeuilVzDeguelante && LocalVitVertMS < SeuilVzMinBeep )
        {
        delay( 500 ) ;
        continue ;
        }
    /*
    // zerotage en vol (pas au sol)
    else if ( LocalVitVertMS >= 0. && LocalVitVertMS < SeuilVzMin )
        {
        static int BeepZerotage = 0 ;
        BeepZerotage++ ;
        BeepZerotage = BeepZerotage%9 ;
        // si le vol a debuté
        if ( g_GlobalVar.m_DureeVolMin >= 0 )
            {
            int MidFreq = (LowFreq + MinFreq) / 2 ;
            if ( BeepZerotage == 0 || BeepZerotage == 3 )
                g_GlobalVar.beeper( MinFreq , 100 ) ;
            else if ( BeepZerotage == 1 || BeepZerotage == 4 )
                g_GlobalVar.beeper( MidFreq , 100 ) ;
            else if ( BeepZerotage == 2 || BeepZerotage == 5 )
                g_GlobalVar.beeper( LowFreq , 100 ) ;
            else
                delay( 500 ) ;
            }
        // si le vol n'a pas debute
        else
            delay( 100 ) ;
        continue ;
        }*/

    // coefficient de Vz
    float Coef01 = (LocalVitVertMS-SeuilVzMinBeep) / (SeuilVzMaxBeep-SeuilVzMinBeep) ;
    if ( Coef01 > 1 )
        Coef01 = 1. ;
    if ( Coef01 < 0 )
        Coef01 = 0. ;

    // calcul frequence son
    float Freq = MinFreq + Coef01 * ( MaxFreq - MinFreq ) ;

    // calcul de la recurrence
    float RecurrenceMs = 700 - Coef01 * 550 ;

    // calcul de la largeur du beep
    float LargeurBeepMs = 100 ;

    // attente
    delay(RecurrenceMs) ;

    // emmission son
    g_GlobalVar.beeper( Freq, LargeurBeepMs ) ;
    }

DeleteTask(VARIOBEEP_NUM_TASK) ;
}
