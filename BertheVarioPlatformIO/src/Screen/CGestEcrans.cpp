////////////////////////////////////////////////////////////////////////////////
/// \file CGestEcran.cpp
///
/// \brief
///
/// \date creation     : 09/03/2024
/// \date modification : 13/03/2025
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
    m_PageChanged = false ;

m_EtatAuto = NextStep ;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief Lance les taches de calcul arriere plan basse priorité
void CGestEcrans::LancerTacheCalcul()
{
xTaskCreatePinnedToCore( TacheScreenCalcul, "ScreenCalc", CALCUL_STACK_SIZE, this  , CALCUL_PRIORITY , NULL, CALCUL_CORE );
}

///////////////////////////////////////////////////////////////////////////////
/// \brief Fonction static de calcul d'arrier plan basse priorite 2hz.
void CGestEcrans::TacheScreenCalcul(void * param)
{
int iboucle10hz = 0 ;
while( g_GlobalVar.m_TaskArr[CALCUL_NUM_TASK].m_Run )
    {
    iboucle10hz++ ;

    #ifdef XC_TRACK
    // envoi bluetooth Xc-Track a 10hz
     if ( g_GlobalVar.m_BleXct.IsInitialised() )
         g_GlobalVar.m_BleXct.Send() ;
    #endif
    delay( 100 ) ;

    // si on n'est pas en ecran 0
    //if ( g_GlobalVar.m_EtatAuto != ECRAN_0_Vz )
    //    continue ;

    // calcul du termic
    //g_TermicMap.CalcTermicProche() ;

    // calcul du terrain le plus proche en finesses 1hz
    if ( !(iboucle10hz%5) )
        g_GlobalVar.m_TerrainArr.CalcTerrainPlusProche() ;
    // calcul de la zone aerienne courante / proche 1hz
    if ( !((iboucle10hz+2)%5) )
        g_GlobalVar.m_ZonesAerAll.CalcZone() ;
    }

g_GlobalVar.m_TaskArr[CALCUL_NUM_TASK].m_Stopped = true ;
while( true )
    vTaskDelete(NULL) ;
}
