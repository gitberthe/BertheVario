////////////////////////////////////////////////////////////////////////////////
/// \file CNbSatDelay.h
///
/// \brief
///
/// \date creation     : 12/10/2024
/// \date modification : 12/10/2024
///

#ifndef _NBSATDELAY_
#define _NBSATDELAY_

////////////////////////////////////////////////////////////////////////////////
/// \brief Temps d'attente decroissant avec le nombre de satelittes en vue.
class CNbSatDelay
{
public :
    int GetNbSatDelay( int NbSat ) const ;
} ;

#endif
