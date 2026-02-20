////////////////////////////////////////////////////////////////////////////////
/// \file CSoundSvr.h
///
/// \brief
///
/// \date creation   : 03/10/2024
/// \date 26/10/2025 : MakeEmptySound()
///

#ifndef _CSOUNDSVR_
#define _CSOUNDSVR_

#define SOUND_BUFFER_SIZE   10
#define SOUND_DELAY_ONLY    -1
#define SOUND_VOLUME_DEF    127
#define DELAY_ATTENTE_IDLE  50

////////////////////////////////////////////////////////////////////////////////
/// \brief Serveur de son
class CSoundSvr
{
public :
    CSoundSvr() ;

    /// \brief requette pour serveur de son
    struct StSoundRequest
        {
        int             m_Frequence = 2000 ;    ///< frequence, negative alors juste attente
        int             m_DelayMs  = 200 ;      ///< durée de la frequence
        //dac_cw_scale_t  m_Volume = DAC_CW_SCALE_1 ; ///< tres fort par defaut
        int             m_Cycle = SOUND_VOLUME_DEF ; ///< tres fort par defaut
        } ;

    void LanceTacheSound() ;
    void MakeEmptySound() ;

protected :
    void PostSoundRequest( const StSoundRequest * pReq ) ;

private :
    static void TacheSoundSvr(void *param) ;

    QueueHandle_t   m_queue;
    StSoundRequest  m_SoundRequest ;
} ;

#endif
