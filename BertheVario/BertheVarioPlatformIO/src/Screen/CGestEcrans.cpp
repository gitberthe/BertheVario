////////////////////////////////////////////////////////////////////////////////
/// \file CGestEcran.cpp
///
/// \brief
///
/// \date creation     : 09/03/2024
/// \date modification : 08/07/2025 suppression xc_track et limite zone xy alti
/// \date 24/10/2025 : frequence de tache calcul
/// \date 30/12/2025 : TacheGpsIgcTempsHistoCalcul
///

#include "../BertheVario.h"

////////////////////////////////////////////////////////////////////////////////
/// \brief Init de l'automate
CGestEcrans::CGestEcrans()
{
// fonction erreur par defaut
for ( int ie = 0 ; ie < FIN ; ie++ )
    m_Automate[ie].m_pFunction = & CGestEcrans::Erreur ;

m_Automate[ECRAN_Debut].m_pFunction     = & CGestEcrans::EcranVz ;
m_Automate[ECRAN_0_Vz].m_pFunction      = & CGestEcrans::EcranVz ;
m_Automate[ECRAN_1_Histo].m_pFunction   = & CGestEcrans::EcranHisto ;
m_Automate[ECRAN_2a_ListeIgc].m_pFunction= & CGestEcrans::EcranListeIgcFch ;
m_Automate[ECRAN_2b_ConfirmArchIgc].m_pFunction= & CGestEcrans::EcranConfimeArchIgcFch ;
m_Automate[ECRAN_3a_TmaAll].m_pFunction = & CGestEcrans::EcranTmaAll ;
m_Automate[ECRAN_3b_TmaMod].m_pFunction = & CGestEcrans::EcranTmaMod ;
m_Automate[ECRAN_4_CfgFch].m_pFunction  = & CGestEcrans::EcranCfgFch ;
m_Automate[ECRAN_5_TmaDessous].m_pFunction= & CGestEcrans::EcranTmaDessous ;
m_Automate[ECRAN_6_Sys].m_pFunction     = & CGestEcrans::EcranSys ;
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
/// \brief Renvoi le nom du cap en 2 lettres
void CGestEcrans::GetCapChar( int CapDeg , char * NomCap )
{
int CapMarge = 45/2 + 1 ;
if ( CapDeg < 0 )
    strcpy( NomCap, "--" ) ;
else if ( CapDeg < CapMarge || CapDeg > (360-CapMarge) )
    strcpy( NomCap, "N " ) ;
else if ( abs(CapDeg-45) < CapMarge )
    strcpy( NomCap, "NE" ) ;
else if ( abs(CapDeg-90) < CapMarge )
    strcpy( NomCap, "E " ) ;
else if ( abs(CapDeg-135) < CapMarge )
    strcpy( NomCap, "SE" ) ;
else if ( abs(CapDeg-180) < CapMarge )
    strcpy( NomCap, "S " ) ;
else if ( abs(CapDeg-225) < CapMarge )
    strcpy( NomCap, "SW" ) ;
else if ( abs(CapDeg-270) < CapMarge )
    strcpy( NomCap, "W " ) ;
else if ( abs(CapDeg-315) < CapMarge )
    strcpy( NomCap, "NW" ) ;
else
    strcpy( NomCap, "--" ) ;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Affichage des differants ecrans etat de l'automate.
void CGestEcrans::AfficheAll()
{
// calcul Thermic
//g_Thermic.DoCapThermic() ;

// appel de la fonction de l'automate vers l'etat suivant
CGestEcrans::EtatsAuto (CGestEcrans::*pFunction)() = m_Automate[m_EtatAuto].m_pFunction ;
EtatsAuto NextStep = (this->*pFunction)() ;

if ( m_EtatAuto != NextStep )
    {
    // changement de page
    m_PageChanged = true ;

    // pour retour automatique vers Vz_0
    m_MillisEcran0 = millis() ;

    // raz screen si changement ecran (doublon)
    //if ( NextStep == ECRAN_0_Vz )
    //    ScreenRaz() ;

    // pour un gain memoire
    g_GlobalVar.GainMemoire() ;
    }
else
    {
    // page changee
    m_PageChanged = false ;
    // arret tache % cpu
    if ( m_EtatAuto != ECRAN_6_Sys )
        g_GlobalVar.m_TaskArr[PERF_MON_NUM_TASK].m_Run = false ;
    }

m_EtatAuto = NextStep ;
}
