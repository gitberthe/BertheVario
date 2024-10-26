////////////////////////////////////////////////////////////////////////////////
/// \file CSimuVol.h
///
/// \brief
///
/// \date creation     : 30/03/2024
/// \date modification : 30/03/2024
///

#ifdef SIMU_VOL

#ifndef _SIMUVOL_
#define _SIMUVOL_

////////////////////////////////////////////////////////////////////////////////
/// \brief Classe de simulation de vol pour mise au point GnuVario avec
/// #define SIMU_VOL dans
class CSimuVol
{
public :
    void LancerTacheSimuVol() ;

private :
    static void TacheSimuVol( void * param ) ;
} ;

extern CSimuVol g_SimuVol ;

#endif

#endif
