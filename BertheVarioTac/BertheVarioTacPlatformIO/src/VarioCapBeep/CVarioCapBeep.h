////////////////////////////////////////////////////////////////////////////////
/// \file CVarioCapBeep.h
///
/// \brief
///
/// \date creation     : 03/10/2024
/// \date modification : 04/10/2024
///

#ifndef _CVARIOCAPBEEP_
#define _CVARIOCAPBEEP_

////////////////////////////////////////////////////////////////////////////////
/// \brief Lectture capteur pression/cap et generation de la Vz
class CVarioCapBeep
{
public :
    void LanceTacheVarioCapBeep() ;

private :
    static void TacheGenereSonVario(void *param) ;
    static void TacheVarioCapteur(void *param) ;
} ;

#endif
