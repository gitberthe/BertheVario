////////////////////////////////////////////////////////////////////////////////
/// \file CVarioBeep.cpp
///
/// \brief
///
/// \date creation     : 04/03/2024
/// \date modification : 14/07/2024
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
//delay(500) ;
#ifdef _LG_DEBUG_
 Serial.println("lancement tache vario beep");
#endif
// tache de port serie
xTaskCreatePinnedToCore(TacheVarioBeep, "TaskBeep", VARIOBEEP_STACK_SIZE, this, VARIOBEEP_PRIORITY,NULL, VARIOBEEP_CORE);
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Fonction static core 0 qui calcul la Vz et gere les beep.
void CVarioBeep::TacheVarioBeep(void *param)
{
delay(5000) ;
const float MinFreq = 1400 ;
const float MaxFreq = 8000 ;
const float SeuilVzMin = g_GlobalVar.m_Config.m_vz_seuil_haut ;
while (g_GlobalVar.m_TaskArr[VARIOBEEP_NUM_TASK].m_Run)
    {
    // desactivation du son
    bool NotActive = g_GlobalVar.m_ZonesAerAll.m_DansDessousUneZone == ZONE_DEDANS ||
                     g_GlobalVar.m_ZonesAerAll.m_DansDessousUneZone == ZONE_LIMITE_ALTI ||
                     g_GlobalVar.m_ZonesAerAll.m_LimiteZone != ZONE_EN_DEHORS ;

    if ( !g_GlobalVar.m_BeepAttenteGVZone )
        NotActive = false ;

    float LocalVitVertMS = g_GlobalVar.m_VitVertMS ;
    //float LocalVitVertMS = g_GlobalVar.m_Config.m_vz_seuil_max ;
    //float LocalVitVertMS = g_GlobalVar.m_Config.m_vz_seuil_haut ;
    #ifdef SOUND_DEBUG
     LocalVitVertMS = 6 ;
    #endif

    // coefficient de Vz
    float Coef01 = (LocalVitVertMS-SeuilVzMin) / g_GlobalVar.m_Config.m_vz_seuil_max ;
    if ( Coef01 > 1 )
        Coef01 = 1. ;
    if ( Coef01 < 0 )
        Coef01 = 0. ;

    // si pas de taux de montee suffisante
    if ( LocalVitVertMS < SeuilVzMin )
        {
        // si degueulante
        if ( LocalVitVertMS < g_GlobalVar.m_Config.m_vz_seuil_bas && !NotActive )
            {
            g_GlobalVar.beeper( MinFreq - 700 , 300 ) ;
            delay( 300 ) ;
            }
        else
            delay( 500 ) ;
        continue ;
        }

    // calcul frequence son
    float Freq = MinFreq + Coef01 * ( MaxFreq - MinFreq ) ;

    // calcul de la recurrence
    float RecurrenceMs = 500 - Coef01 * 400 ;

    // calcul de la largeur du beep
    float LargeurBeepMs = 100 ;

    // attente
    delay(RecurrenceMs) ;

    // si alarme zone
    if ( NotActive )
        continue ;

    // emmission son
    g_GlobalVar.beeper( Freq, LargeurBeepMs ) ;
    }

g_GlobalVar.m_TaskArr[VARIOBEEP_NUM_TASK].m_Stopped = true ;
}
