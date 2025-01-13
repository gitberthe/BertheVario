////////////////////////////////////////////////////////////////////////////////
/// \file CRandoVol.cpp
///
/// \brief
///
/// \date creation     : 13/01/2025
/// \date modification : 13/01/2025
///

#include "../BertheVario.h"

////////////////////////////////////////////////////////////////////////////////
/// \brief Initialisation rando-vol
void CRandoVol::InitRandoVol()
{
// supprimer les zones aeriennes

// tache gps
LanceTacheRandoVol() ;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Affichage rando-vol
void CRandoVol::AfficheRandoVol()
{
g_GlobalVar.EcranRandoVol() ;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Lance les tache de gestion du Gps : acquisition serie
void CRandoVol::LanceTacheRandoVol()
{
#ifdef _LG_DEBUG_
 Serial.println("lancement tache gps");
#endif

// tache de port serie
xTaskCreatePinnedToCore(CGps::TacheGpsSerial, "GpsSerial", SERIAL_GPS_STACK_SIZE, this, SERIAL_GPS_PRIORITY,NULL, SERIAL_GPS_CORE);

// tache de calcul du temps de vol
//xTaskCreatePinnedToCore(TacheGpsTempsVol, "GpsTempsVol", TEMPS_STACK_SIZE , this, TEMPS_PRIORITY, NULL, TEMPS_CORE);
}
