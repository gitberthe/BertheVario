////////////////////////////////////////////////////////////////////////////////
/// \file CSoundSvr.h
///
/// \brief
///
/// \date creation     : 03/10/2024
/// \date modification : 13/01/2025
///

#include "../BertheVario.h"

////////////////////////////////////////////////////////////////////////////////
/// \brief Creation de la pile de demande
CSoundSvr::CSoundSvr()
{
// taille de 8
m_queue = xQueueCreate(SOUND_BUFFER_SIZE, sizeof(StSoundRequest));
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Met un son dans la file d'attente.
void CSoundSvr::PostSoundRequest( const StSoundRequest * pReq )
{
xQueueSend(m_queue, (void*)pReq, (TickType_t)0);
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Lance la tache de son
void CSoundSvr::LanceTacheSound()
{
g_GlobalVar.m_TaskArr[SOUNDSVR_NUM_TASK].m_Run = true ;
g_GlobalVar.m_TaskArr[SOUNDSVR_NUM_TASK].m_Stopped = false ;
xTaskCreatePinnedToCore(TacheSoundSvr, "SoundSvr", SOUNDSVR_STACK_SIZE , this, SOUNDSVR_PRIORITY, NULL, SOUNDSVR_CORE);
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Tache pour jouer les sons qui sont dans la file d'attente.
void CSoundSvr::TacheSoundSvr(void* param)
{
g_GlobalVar.m_TaskArr[SOUNDSVR_NUM_TASK].m_Stopped = false ;

// utilisation du convertisseur DA
/*dac_output_enable(DAC_CHANNEL_1);
dac_cw_config_t config ;
config.en_ch = DAC_CHANNEL_1 ; // DAC_GPIO25_CHANNEL ;
//config.scale = DAC_CW_SCALE_8 ;   // le moins fort
config.scale = DAC_CW_SCALE_1 ; // le plus fort */

const int channel = 0 ;
const int resolution = 8; // Résolution de 8 bits, 256 valeurs possibles

// boucle du serveur
StSoundRequest SoundRequest ;
while (g_GlobalVar.m_TaskArr[SOUNDSVR_NUM_TASK].m_Run)
    {
    // attente 10 millisecondes
    if( !xQueueReceive(g_GlobalVar.m_queue, &(SoundRequest), (TickType_t)0))
        {
        delay( 10 ) ;
        continue ;
        }

    // traitement de la demande
    if ( SoundRequest.m_Frequence > 0 )
        {
        // configuration led
        g_GlobalVar.m_MutexI2c.PrendreMutex() ;
         ledcSetup( channel , SoundRequest.m_Frequence , resolution ) ;
         ledcAttachPin(SPEAKER_PIN, channel);
         //ledcWrite(channel,127);    // son maximum 255
         ledcWrite(channel, SoundRequest.m_Cycle );    // son maximum 255
        g_GlobalVar.m_MutexI2c.RelacherMutex() ;

        // attente
        delay( SoundRequest.m_DelayMs ) ;

        // raz led
        g_GlobalVar.m_MutexI2c.PrendreMutex() ;
         ledcDetachPin(SPEAKER_PIN) ;
        g_GlobalVar.m_MutexI2c.RelacherMutex() ;

        /*g_GlobalVar.m_MutexI2c.PrendreMutex() ;
         tone(SPEAKER_PIN, SoundRequest.m_Frequence, SoundRequest.m_DelayMs ) ;
        g_GlobalVar.m_MutexI2c.RelacherMutex() ; // */
        /*config.freq = SoundRequest.m_Frequence ;
        config.scale= SoundRequest.m_Volume ;
        dac_cw_generator_config( & config ) ;
        dac_cw_generator_enable() ;
        delay( SoundRequest.m_DelayMs ) ;
        // desactivation du son
        dac_cw_generator_disable() ; // */
        }
    else
        {
        // attente sans le son
        delay( SoundRequest.m_DelayMs ) ;
        }
    }

g_GlobalVar.m_TaskArr[SOUNDSVR_NUM_TASK].m_Stopped = true ;
while( true )
    vTaskDelete(NULL) ;
}

//fin :
//pinMode(26, OUTPUT);
/*int freq = 3000; // 3000 Hz
const int ledChannel = 1;
const int resolution = 8; // Résolution de 8 bits
const int ledPin = 26;
int dutyCycle = 255/2 ;

ledcWrite(ledChannel, dutyCycle);
ledcSetup(ledChannel, freq, resolution);
ledcAttachPin(ledPin, ledChannel);
//ledcWriteTone( ledChannel, freq ) ;
//pinMode(22, OUTPUT);

while( true )
    delay( 2000 ) ;

freq = 7000 ;
ledcWrite(ledChannel, dutyCycle);
ledcSetup(ledChannel, freq, resolution);
ledcAttachPin(ledPin, ledChannel);
//ledcWriteTone( ledChannel, freq ) ;
ledcDetachPin( ledPin ) ; //*/


