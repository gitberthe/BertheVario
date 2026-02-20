////////////////////////////////////////////////////////////////////////////////
/// \file CBrightness.h
///
/// \brief
///
/// \date creation     : 25/01/2025
/// \date modification : 25/01/2025
///

#pragma once

////////////////////////////////////////////////////////////////////////////////
/// \brief Gestion de la luminosite de l'ecran
class CBrightness
{
public :
    void SetBrightness( bool * ToucheScreen ) ;

private :
    CMutex        m_Mutexluminosite ;       ///< mutex de lavariable statique
    unsigned long m_LastScreenTouch = 0 ;   ///< temps du dernier touch
} ;
