////////////////////////////////////////////////////////////////////////////////
/// \file CVarioBeep.h
///
/// \brief
///
/// \date creation     : 04/03/2024
/// \date modification : 21/03/2024
///

#ifndef _VARIOBEEP_
#define _VARIOBEEP_

////////////////////////////////////////////////////////////////////////////////
/// \brief Beep fonction de la Vz.
class CVarioBeep
{
public :
    void InitSpeaker() ;
    void LanceTacheVarioBeep() ;

private :
    static void TacheVarioBeep(void *param) ;
} ;

#endif
