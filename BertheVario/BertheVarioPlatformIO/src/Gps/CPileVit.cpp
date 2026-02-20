////////////////////////////////////////////////////////////////////////////////
/// \file CPileVit.cpp
///
/// \brief
///
/// \date creation     : 02/09/2024
/// \date modification : 17/10/2024
///

#ifdef _BERTHE_VARIO_
 #include "../BertheVario.h"
#endif
#ifdef _BERTHE_VARIO_213_
 #include "../BertheVario213.h"
#endif
#ifdef _BERTHE_VARIO_TAC_
 #include "../BertheVarioTac.h"
#endif

////////////////////////////////////////////////////////////////////////////////
/// \brief
CPileVit::CPileVit()
{
InitPile() ;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief
CPileVit::~CPileVit()
{
}

////////////////////////////////////////////////////////////////////////////////
/// \brief On empile une vitesse Gps. Sauf si changement de satellites alors
/// tempo de 10.
void CPileVit::PushData()
{
// empilement position
m_PosArr[ m_ipile ].m_VitHorizontale = g_GlobalVar.m_VitesseKmh ;
m_PosArr[ m_ipile ].m_VitVerticale   = g_GlobalVar.m_VitVertMS ;
m_PosArr[ m_ipile ].m_HDop           = CTrame::GetHDop() ;
m_ipile++ ;

#ifdef DEBUG_HDOP
 Serial.print( "hdop : " ) ;
 Serial.println( CTrame::GetHDop() , 2 ) ;
#endif

// rebouclage position cpurante
if ( m_ipile >= TAILLE_PILE_VITESSE )
    {
    m_ipile = 0 ;
    m_pile_full = true ;
    }

// si pile pas pleine
if ( ! m_pile_full )
    {
    m_MoyenneVitHor = 0. ;
    m_MoyenneVitVert = 0. ;
    return ;
    }

// calcul des moyennes
bool l_HDop = true ;
float moyenne_vit_hor  = 0. ;
float moyenne_vit_vert = 0. ;
for ( int ip = 0 ; ip < TAILLE_PILE_VITESSE ; ip++ )
    {
    // comparaisons des HDop
    if ( ip > 0 && m_PosArr[ip-1].m_HDop != m_PosArr[ip].m_HDop )
        l_HDop = false ;
    if ( m_PosArr[ip].m_HDop >= HDOP_BON )
        l_HDop = false ;
    // vitesses H/V
    moyenne_vit_hor  += m_PosArr[ip].m_VitHorizontale ;
    moyenne_vit_vert += m_PosArr[ip].m_VitVerticale ;
    }
// recopie dans variables asynchrone
m_HDopOk = l_HDop ;
m_MoyenneVitHor   = moyenne_vit_hor  / TAILLE_PILE_VITESSE ;
m_MoyenneVitVert  = moyenne_vit_vert / TAILLE_PILE_VITESSE ;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Initialise le debut de vol et le nombre de fix alti baro
void CPileVit::InitPile()
{
m_TypeDebutVol = DEBUT_VOL_INCONNU ;
m_CountFixAltiBaro = 0 ;
m_LastFixAltiBaro = 0 ;
m_HDopOk = false ;
m_ipile = 0 ;
m_pile_full = false ;

m_MoyenneVitHor  = 0. ;       ///< pour le debut de vol
m_MoyenneVitVert = 0. ;       ///< pour le debut de vol
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Informe si l'on doit attendre pour stabilisation Gps
bool CPileVit::IsWaitForStab() const
{
if ( ! m_pile_full )
    return true ;

if ( !m_HDopOk )
    return true ;

return false ;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Informe si l'on peut recaller l'altitude baro avec le sol.
/// Un recallage toutes les 30 sec min minimum
bool CPileVit::CanFixAltiBaro()
{
// toutes les 30 sec au plus
unsigned long DeltaTMilli = millis() - m_LastFixAltiBaro ;
if ( (DeltaTMilli/1000) < (m_CountFixAltiBaro*LAST_FIX_ALTIBARO_SEC) )
    return false ;

// si pile par pleine (mise sous tension)
if ( ! m_pile_full )
    return false ;

// si forte vitesse XY pas de fix
if ( g_GlobalVar.m_VitesseKmh > VITESSE_FAIBLE_HOR_KMH )
    return false ;

// si forte vitesse Vz pas de fix
if ( fabsf( g_GlobalVar.m_VitVertMS ) > g_GlobalVar.m_Config.m_vz_igc_ms )
    return false ;

// memorisation du temps
m_LastFixAltiBaro = millis() ;

// recallage autorise 10s * x
m_CountFixAltiBaro++ ;
if ( m_CountFixAltiBaro > 60 )
    m_CountFixAltiBaro > 60 ; // 10 min max

return true ;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Indique si on est en vol pour declenchement debut de vol vitesse.
/// Si la moyenne des vitesses est assez haute.
bool CPileVit::IsStartFlight()
{
// si pile pas pleine
if ( !m_pile_full )
    return false ;

// vitesse verticale depassee
float VzMin = g_GlobalVar.m_Config.m_vz_igc_ms ;
#ifdef LOW_VZ_START
 VzMin = 0.1 ;
#endif
if ( fabsf( m_MoyenneVitVert ) >= VzMin )
    {
    m_TypeDebutVol = DEBUT_VOL_VIT_VERT ;
    return true ;
    }

// vitesse horizontale depassee si HDop ok (sans changement de precision)
float VhorMin = g_GlobalVar.m_Config.m_vitesse_igc_kmh ;
#ifdef LOW_VZ_START
 VhorMin = 3.5 ;
#endif
if ( m_HDopOk && (m_MoyenneVitHor >= VhorMin) )
    {
    m_TypeDebutVol = DEBUT_VOL_VIT_HOR ;
    return true ;
    }

// hauteur sol > 10m
if ( m_CountFixAltiBaro > 1 && g_GlobalVar.GetHauteurSol() >= HAUTEUR_LOIN_SOL_METRE_DECO )
    {
    m_TypeDebutVol = DEBUT_VOL_HAUTEUR_SOL ;
    return true ;
    }

return false ;
}
