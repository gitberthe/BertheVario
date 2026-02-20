////////////////////////////////////////////////////////////////////////////////
/// \file CAutoPage.cpp
///
/// \brief Automate de sequencement des pages ecran
///
/// \date creation     : 21/09/2024
/// \date modification : 22/01/2025
///

#include "../BertheVarioTac.h"

////////////////////////////////////////////////////////////////////////////////
/// \brief Initialisation de l'automate.
CAutoPages::CAutoPages()
{
// fonction erreur par defaut
for ( int ie = 0 ; ie < FIN ; ie++ )
    m_Automate[ie].m_pFunction = & CAutoPages::Erreur ;

m_Automate[ECRAN_0_Vz].m_pFunction       = & CAutoPages::EcranVz ;
m_Automate[ECRAN_1_Histo].m_pFunction    = & CAutoPages::EcranHisto ;
m_Automate[ECRAN_2a_ListeIgc].m_pFunction= & CAutoPages::EcranListeIgcFch ;
m_Automate[ECRAN_2b_ConfirmArchIgc].m_pFunction= & CAutoPages::EcranConfimeArchIgcFch ;
m_Automate[ECRAN_3a_TmaAll].m_pFunction  = & CAutoPages::EcranTmaAll ;
m_Automate[ECRAN_3b_TmaMod].m_pFunction  = & CAutoPages::EcranTmaMod ;
m_Automate[ECRAN_4_CfgFch].m_pFunction   = & CAutoPages::EcranCfgFch ;
m_Automate[ECRAN_5_TmaDessous].m_pFunction= & CAutoPages::EcranTmaDessous ;
m_Automate[ECRAN_6_Sys].m_pFunction      = & CAutoPages::EcranSys ;
m_Automate[ECRAN_7_WifiFileMgr].m_pFunction= & CAutoPages::EcranWifiFileMgr ;
m_Automate[ECRAN_8_Menu].m_pFunction     = & CAutoPages::EcranMenu ;
m_Automate[ECRAN_6b_CalMag].m_pFunction  = & CAutoPages::EcranCalibreMagnetique ;
m_Automate[ECRAN_6c_TelechFirm].m_pFunction= & CAutoPages::EcranTelechargementFirmware ;
m_Automate[ECRAN_9a_RandoVolMenu].m_pFunction  = &CAutoPages::EcranRandoVolMenu ;
m_Automate[ECRAN_9b_RandoVolCarte].m_pFunction  = & CAutoPages::EcranRandoVolCarte ;
m_Automate[ECRAN_9c_RandoVolInfo].m_pFunction= & CAutoPages::EcranRandoVolInfo ;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief pour le retour en arriere a l'ecran Vz
void CAutoPages::SetVzEtatAuto()
{
m_PageChanged = true ;

// ecran de configuration on sauvegarde
if ( m_EtatAuto == ECRAN_4_CfgFch )
    g_GlobalVar.m_Config.EcritureFichier() ;

// ecran de modification Tma on sauvegarde
if ( m_EtatAuto == ECRAN_3b_TmaMod )
    g_GlobalVar.m_ZonesAerAll.EcritureFichierZonesActive() ;

m_EtatAuto = ECRAN_0_Vz ;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Affichage des differants ecrans etat de l'automate.
void CAutoPages::SequencementPages()
{
// appel de la fonction de l'automate vers l'etat suivant
CAutoPages::EtatsAuto (CAutoPages::*pFunction)() = m_Automate[m_EtatAuto].m_pFunction ;
EtatsAuto NextStep = (this->*pFunction)() ;

//ScreenOff() ;
// si centre de l'ecran presse
if ( g_GlobalVar.m_Screen.IsCenterPressed() )
    {
    #ifdef DEBUG_RANDO_VOL
     SetVzEtatAuto() ;
    #else
     if ( m_EtatAuto != ECRAN_9a_RandoVolMenu &&
             m_EtatAuto != ECRAN_9b_RandoVolCarte &&
             m_EtatAuto != ECRAN_9c_RandoVolInfo )
        SetVzEtatAuto() ;
    #endif
    return ;
    }

// si changement de page
if ( m_EtatAuto != NextStep )
    {
    // pour le retour en arriere
    //m_LastEtatAuto = m_EtatAuto ;

    // pour retour automatique vers Vz_0
    m_MillisEcran0 = millis() ;

    // changement page
    m_PageChanged = true ;
    g_GlobalVar.GainMemoire() ;

    /*// anti rebond
    unsigned long time = millis() ;
    while( (millis()-time) < 200 )
        {
        ScreenRazButtons() ;
        delay(1) ;
        }*/
    }
// si meme page alors retour automatique ecran Vz si time out
else
    {
    // pas de changement de page
    m_PageChanged = false ;

    // arret tache % cpu
    if ( m_EtatAuto != ECRAN_6_Sys )
        g_GlobalVar.m_TaskArr[PERF_MON_NUM_TASK].m_Run = false ;

    // pas de retour Vz si modification tma ou wifi ou calibration
    if ( (m_CfgFileiChamps != -1 && m_EtatAuto == ECRAN_4_CfgFch ) ||
          m_EtatAuto == ECRAN_3b_TmaMod ||
          (m_EtatAuto == ECRAN_6b_CalMag && m_CalibrationEnCours) )
        m_MillisEcran0 = millis() ;

    // si pas page Vz ni rando
    if ( m_EtatAuto != ECRAN_0_Vz &&
         m_EtatAuto != ECRAN_9a_RandoVolMenu &&
         m_EtatAuto != ECRAN_9b_RandoVolCarte &&
         m_EtatAuto != ECRAN_9c_RandoVolInfo )
        {
        // si time out
        if( (millis()-m_MillisEcran0)/1000 > m_SecondesRetourEcran0 )
            {
            ScreenRaz() ;
            m_PageChanged = true ;
            g_GlobalVar.GainMemoire() ;
            m_EtatAuto = ECRAN_0_Vz ;
            return ;
            }
        }
    }

// prochain etat
m_EtatAuto = NextStep ;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Fonction erreur de l'automate.
CAutoPages::EtatsAuto CAutoPages::Erreur()
{
Serial.println("Erreur automate") ;
delay(200) ;
CGlobalVar::BeepError() ;

return ERREUR ;
}
