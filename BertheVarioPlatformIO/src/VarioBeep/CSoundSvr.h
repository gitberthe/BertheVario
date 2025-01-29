////////////////////////////////////////////////////////////////////////////////
/// \file CSoundSvr.h
///
/// \brief
///
/// \date creation     : 03/10/2024
/// \date modification : 28/01/2025
///

#ifndef _CSOUNDSVR_
#define _CSOUNDSVR_

#define SOUND_BUFFER_SIZE 10
#define SOUND_DELAY_ONLY  -1

#define VOLUME_MOYEN       127
#define VOLUME_MAX_CYCLE   255
#define VOLUME_BAS         50

////////////////////////////////////////////////////////////////////////////////
/// \brief Serveur de son
class CSoundSvr
{
public :
    CSoundSvr() ;

    void LanceTacheSound() ;

    /// \brief requette pour serveur de son
    struct StSoundRequest
        {
        int     m_Frequence = 2000 ;    ///< frequence, negative alors juste attente
        int     m_DelayMs  = 200 ;      ///< durée de la frequence
        int     m_Cycle = VOLUME_MOYEN ; ///< moyennement fort par defaut
        //dac_cw_scale_t  m_Volume = DAC_CW_SCALE_8 ; ///< tres fort par defaut
        } ;

protected :
    void PostSoundRequest( const StSoundRequest * pReq ) ;

private :
    static void TacheSoundSvr(void *param) ;

    QueueHandle_t   m_queue;
    StSoundRequest  m_SoundRequest ;
} ;

#endif
