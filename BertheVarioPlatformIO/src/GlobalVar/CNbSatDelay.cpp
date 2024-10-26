////////////////////////////////////////////////////////////////////////////////
/// \file CNbSatDelay.cpp
///
/// \brief
///
/// \date creation     : 12/10/2024
/// \date modification : 20/10/2024
///

#include "../BertheVario.h"

////////////////////////////////////////////////////////////////////////////////
/// \brief Renvoi le temps en secondes d'attente apres un changement de nombre
/// de satellites en vue.
/// \return m_sat_sec : si 4 satellites.
/// \return 0 : si 30 satellites.
int CNbSatDelay::GetNbSatDelay(int NbSat) const
{
float SecDelay = ((float)(g_GlobalVar.m_Config.m_sat_sec))*((float)(30-NbSat))/26. ;
return (int)SecDelay ;
}
