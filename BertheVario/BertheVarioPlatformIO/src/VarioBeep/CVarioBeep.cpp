////////////////////////////////////////////////////////////////////////////////
/// \file CVarioBeep.cpp
///
/// \brief
///
/// \date creation     : 04/03/2024
/// \date modification : 08/07/2025 suppression xc_track et limite zone xy alti
/// \date modification : 09/07/2025 modification de beep vario sol
/// \date 27/11/2025 : plus de bips zerotage
///

#include "../BertheVario.h"

///////////////////////////////////////////////////////////////////////////////
/// \brief
void CVarioBeep::InitSpeaker()
{
//pinMode(PIN_AUDIO_AMP_ENA,OUTPUT);
//digitalWrite(PIN_AUDIO_AMP_ENA,HIGH);
//digitalWrite(PIN_AUDIO_AMP_ENA,LOW);

/*ESP32PWM::allocateTimer(0);
ESP32PWM::allocateTimer(1);
ESP32PWM::allocateTimer(2);
ESP32PWM::allocateTimer(3);*/
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Lance la tache de gestion du Gps
void CVarioBeep::LanceTacheVarioBeep()
{
#ifdef _LG_DEBUG_
 Serial.println("lancement tache vario beep");
#endif
// tache de port serie
xTaskCreatePinnedToCore(TacheVarioBeep, "TaskBeep", VARIOBEEP_STACK_SIZE, this, VARIOBEEP_PRIORITY,
    GetTaskHandle(VARIOBEEP_NUM_TASK), VARIOBEEP_CORE);
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Fonction static core 0 qui calcul la Vz et gere les beep.
void CVarioBeep::TacheVarioBeep(void *param)
{
#ifdef STACK_DEBUG
 int MinStack = 32000 ;
#endif

delay(3000) ;
const float LowFreq = 1100 ;
const float MinFreq = 1200 ;
const float MaxFreq = 8000 ;
const float SeuilVzMaxBeep    = g_GlobalVar.m_Config.m_vz_seuil_max ;
const float SeuilVzMinBeep    = g_GlobalVar.m_Config.m_vz_seuil_haut ;
const float SeuilVzDeguelante = g_GlobalVar.m_Config.m_vz_seuil_bas ;
while (g_GlobalVar.m_TaskArr[VARIOBEEP_NUM_TASK].m_Run)
    {
    // desactivation du son cause TMA
    //bool NotActive = g_GlobalVar.m_ZonesAerAll.m_DansDessousUneZone == ZONE_DEDANS ; // ) ; // ||
                     // g_GlobalVar.m_ZonesAerAll.m_LimiteZone == ZONE_LIMITE_ALTI ||
                     // g_GlobalVar.m_ZonesAerAll.m_LimiteZone == ZONE_LIMITE_FRONTIERE ;

    // si alarme zone desactivé => son vario active
    //if ( !g_GlobalVar.m_BeepAttenteGVZone )
    //    NotActive = false ;

    // si son vario desactive
    /* if ( ! g_GlobalVar.m_BeepVario )
        {
        delay( 500 ) ;
        continue ;
        } */

    float LocalVitVertMS = g_GlobalVar.m_VitVertMS ;
    //LocalVitVertMS = -3 ;
    //float LocalVitVertMS = g_GlobalVar.m_Config.m_vz_seuil_haut ;
    #ifdef SOUND_DEBUG
     LocalVitVertMS = 6 ;
    #endif

    #ifdef STACK_DEBUG
     int MesStack = uxTaskGetStackHighWaterMark(NULL) ;
     if ( MesStack < MinStack )
        MinStack = MesStack ;
     Serial.printf("Stack Min TacheVarioBeep : %d bytes\n", MinStack );
    #endif

    // si degueulante
    if ( LocalVitVertMS <= SeuilVzDeguelante )
        {
        g_GlobalVar.beeper( LowFreq , 200 ) ;
        delay( 400 ) ;
        continue ;
        }
    // descente normale ou peut de montee
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
        else
            delay( 100 ) ;
        continue ;
        }
    */

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
