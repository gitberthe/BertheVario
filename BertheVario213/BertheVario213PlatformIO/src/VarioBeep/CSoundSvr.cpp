////////////////////////////////////////////////////////////////////////////////
/// \file CSoundSvr.h
///
/// \brief
///
/// \date creation   : 03/10/2024
/// \date 17/10/2025 : modification mutex
/// \date 26/10/2025 : MakeEmptySound() .
///

#include "../BertheVario213.h"

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
xTaskCreatePinnedToCore(TacheSoundSvr, "SoundSvr", SOUNDSVR_STACK_SIZE , this, SOUNDSVR_PRIORITY,
    GetTaskHandle(SOUNDSVR_NUM_TASK), SOUNDSVR_CORE);
}


////////////////////////////////////////////////////////////////////////////////
/// \brief pour vider la file.
void CSoundSvr::MakeEmptySound()
{
StSoundRequest SoundRequest ;
unsigned int ret ;
do {
    g_GlobalVar.m_MutexQueue.PrendreMutex() ;
     ret = xQueueReceive(g_GlobalVar.m_queue, & SoundRequest , (TickType_t)0) ;
    g_GlobalVar.m_MutexQueue.RelacherMutex() ;
    }
while ( ret ) ;
}

// #define TASMOTA

////////////////////////////////////////////////////////////////////////////////
/// \brief Tache pour jouer les sons qui sont dans la file d'attente.
void CSoundSvr::TacheSoundSvr(void* param)
{
const int channel = 0 ;
const int resolution = 8; // Résolution de 8 bits, 256 valeurs possibles

// boucle du serveur
StSoundRequest SoundRequest ;
#ifdef TASMOTA
// init par defaut sinon bug
g_GlobalVar.m_MutexI2c.PrendreMutex() ;
 ledcAttachChannel(SPEAKER_PIN, SoundRequest.m_Frequence , resolution , channel );
 ledcDetach(SPEAKER_PIN) ;
g_GlobalVar.m_MutexI2c.RelacherMutex() ;
#endif
while (g_GlobalVar.m_TaskArr[SOUNDSVR_NUM_TASK].m_Run || true )
    {
    // attente 50 millisecondes
    g_GlobalVar.m_MutexQueue.PrendreMutex() ;
     unsigned int ret = xQueueReceive(g_GlobalVar.m_queue, & SoundRequest , (TickType_t)0) ;
    g_GlobalVar.m_MutexQueue.RelacherMutex() ;
    if( !ret )
        {
        delay( 50 ) ;
        continue ;
        }

    // si pile a moitier pleine on passe les attentes
    g_GlobalVar.m_MutexQueue.PrendreMutex() ;
     ret = uxQueueSpacesAvailable( g_GlobalVar.m_queue ) ;
    g_GlobalVar.m_MutexQueue.RelacherMutex() ;
    if ( ret < (SOUND_BUFFER_SIZE/2) )
        {
        if ( SoundRequest.m_Frequence == SOUND_DELAY_ONLY )
            continue ;
        }

    // traitement de la demande
    if ( SoundRequest.m_Frequence > 0 )
        {
        // configuration led
        g_GlobalVar.m_MutexI2c.PrendreMutex() ;
        #ifdef TASMOTA
         ledcAttachChannel(SPEAKER_PIN, SoundRequest.m_Frequence , resolution , channel );
         ledcWrite(SPEAKER_PIN, SoundRequest.m_Cycle );
        #else
         ledcSetup( channel , SoundRequest.m_Frequence , resolution ) ;
         ledcAttachPin(SPEAKER_PIN, channel );
         ledcWrite(channel, SoundRequest.m_Cycle );
        #endif
        g_GlobalVar.m_MutexI2c.RelacherMutex() ;

        // attente
        delay( SoundRequest.m_DelayMs ) ;

        // raz led
        g_GlobalVar.m_MutexI2c.PrendreMutex() ;
        #ifdef TASMOTA
         ledcDetach(SPEAKER_PIN) ;
        #else
         ledcDetachPin(SPEAKER_PIN) ;
        #endif
        g_GlobalVar.m_MutexI2c.RelacherMutex() ;
        }
    else
        {
        // attente sans le son
        delay( SoundRequest.m_DelayMs ) ;
        }
    }

// fin de tache
DeleteTask( SOUNDSVR_NUM_TASK ) ;
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


