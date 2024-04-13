////////////////////////////////////////////////////////////////////////////////
/// \file CGestEcran.cpp
///
/// \brief
///
/// \date creation     : 09/03/2024
/// \date modification : 01/04/2024
///

#include "../BertheVario.h"

////////////////////////////////////////////////////////////////////////////////
/// \brief Init de l'automate
CGestEcrans::CGestEcrans()
{
// fonction erreur par defaut
for ( int ie = 0 ; ie < FIN ; ie++ )
    m_Automate[ie].m_pFunction = & CGestEcrans::Erreur ;

m_Automate[ECRAN_0_Vz].m_pFunction      = & CGestEcrans::Ecran0Vz ;
m_Automate[ECRAN_1_Histo].m_pFunction   = & CGestEcrans::Ecran1Histo ;
m_Automate[ECRAN_2a_TmaAll].m_pFunction = & CGestEcrans::Ecran2aTmaAll ;
m_Automate[ECRAN_2b_TmaMod].m_pFunction = & CGestEcrans::Ecran2bTmaMod ;
m_Automate[ECRAN_3_Sys].m_pFunction     = & CGestEcrans::Ecran3Sys ;
m_Automate[ECRAN_4_CfgFch].m_pFunction  = & CGestEcrans::Ecran4CfgFch ;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Fonction erreur de l'automate.
CGestEcrans::EtatsAuto CGestEcrans::Erreur()
{
Serial.println("Erreur automate") ;
delay(2000) ;
CGlobalVar::BeepError() ;

return ERREUR ;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Affichage des differants ecrans etat de l'automate.
void CGestEcrans::AfficheAll()
{
// calcul Thermic
//g_Thermic.DoCapThermic() ;

// appel de la fonction de l'automate vers l'etat suivant
CGestEcrans::EtatsAuto (CGestEcrans::*pFunction)() = m_Automate[m_EtatAuto].m_pFunction ;
m_EtatAuto = (this->*pFunction)() ;
}
