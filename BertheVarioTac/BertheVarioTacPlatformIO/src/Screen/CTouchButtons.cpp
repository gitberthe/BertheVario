////////////////////////////////////////////////////////////////////////////////
/// \file CTouchButtons.cpp
///
/// \brief Boutons tactiles
///
/// \date creation     : 21/09/2024
/// \date modification : 01/11/2024
///

#include "../BertheVarioTac.h"

bool CTouchButtons::ms_Want2Reboot = false ;

////////////////////////////////////////////////////////////////////////////////
/// \brief
CTouchButtons::CTouchButtons()
{
for ( int i = 0 ; i < m_NbButtons ; i++ )
    {
    m_PressedArr[i] = false ;
    m_PressedArrLast[i] = true ;
    }
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Renvoi si le bouton a ete appuye et le RAZ .
bool CTouchButtons::IsButtonPressed( int ib )
{
bool ret = m_PressedArr[ib] ;
m_PressedArr[ib] = false ;
return ret ;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Pour le gel des boutons.
void CTouchButtons::SetFrozenDelay( int Ms )
{
m_FrozenMs = Ms ;
m_TimePressed = millis() ;
RazButtons() ;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Appellee par de driver CGLFX en asynchrone.
/// Indique si l'ecran a ete appuye et positionne la luminosite.
void CTouchButtons::SetPressed()
{
// gel des boutons
if ( (millis()-m_TimePressed) < m_FrozenMs )
    return ;
else
    m_FrozenMs = 0 ;

// anti-rebond
if ( (millis()-m_TimePressed) < ANTI_REBONDS_MS )
    return ;

// variables pur le reste du programme
m_ScreenPressed = true ;
m_TimePressed = millis() ;

// luminosite
SetBrightness( & m_ScreenPressed ) ;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Renvoi si le centre de l'ecran est appuyé
bool CTouchButtons::IsCenterPressed()
{
bool ret = m_CenterPressed ;
m_CenterPressed = false ;
return ret ;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Lit la position du touch pad et active les boutons en consequence.
/// Si on est en vol, la page menu par le centre de l'ecran n'est plus accessible.
/// Appele regulierement par loop().
/// \return true si un bouton pressé.
bool CTouchButtons::HandleButtons()
{
// luminosite pour dans le noir
SetBrightness( NULL ) ;

// pour tous les boutons
int ib = 0 ;
for ( int x = 0 ; x < g_GlobalVar.m_Screen.m_Largeur ; x += g_GlobalVar.m_Screen.m_Largeur/m_NbButtons , ib++ )
    {
    // verification zone clic
    if ( g_GlobalVar.m_Screen.m_XTouch > x && g_GlobalVar.m_Screen.m_XTouch < (x + g_GlobalVar.m_Screen.m_Largeur/m_NbButtons) &&
         g_GlobalVar.m_Screen.m_YTouch > (g_GlobalVar.m_Screen.m_Hauteur - m_HauteurBoutons) &&
         g_GlobalVar.m_Screen.m_ScreenPressed )
        {
        // bouton gauche centre ou droit
        m_PressedArr[ib] = true ;
        // desactivation g_tft
        m_ScreenPressed = false ;
        // desactivation centre écran
        m_CenterPressed = false ;
        // return action
        return true ;
        }
    }

// si pas en vol ou modes wifi, action centre ecran possible
if ( !g_GlobalVar.m_FinDeVol.IsInFlight() || g_GlobalVar.m_ModeHttp || g_GlobalVar.m_ModeHttpOta )
    {
    // si toujours presse alors centre de l'ecran
    if ( m_ScreenPressed )
        {
        // centre pressé
        m_CenterPressed = true ;
        // desactivation g_tft
        m_ScreenPressed = false ;
        // return action
        return true ;
        }
    }
else
    {
    // centre pressé
    m_CenterPressed = false ;
    // desactivation g_tft
    m_ScreenPressed = false ;
    }

return false ;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Affiche la position du touch pad et active les boutons en consequence
void CTouchButtons::AfficheButtons()
{
const uint16_t ColorFond  = TFT_BLACK ;
const uint16_t ColorTexte = TFT_WHITE ;

// pour tous les boutons
int ib = 0 ;
for ( int x = 0 ; x < g_GlobalVar.m_Screen.m_Largeur ; x += g_GlobalVar.m_Screen.m_Largeur/m_NbButtons , ib++ )
    {
    // bord du boutons
    int ColorDouble = ColorFond ;

    // pout affichage rapide non gourmand en CPU
    if ( m_PressedArrLast[ib] == m_PressedArr[ib] )
        continue ;
    m_PressedArrLast[ib] = m_PressedArr[ib] ;

    // cerclage boutons
    if ( m_PressedArr[ib] )
        ColorDouble = ColorTexte ;

    g_tft.fillRect( x + 10 , g_GlobalVar.m_Screen.m_Hauteur - m_HauteurBoutons + 10 , g_GlobalVar.m_Screen.m_Largeur/m_NbButtons -20 , m_HauteurBoutons -20 , ColorFond ) ;
    g_tft.fillRect( x , g_GlobalVar.m_Screen.m_Hauteur - m_HauteurBoutons ,
                        g_GlobalVar.m_Screen.m_Largeur/m_NbButtons ,m_Marge , ColorDouble ) ;
    g_tft.fillRect( x , g_GlobalVar.m_Screen.m_Hauteur - m_HauteurBoutons ,
                        m_Marge , m_HauteurBoutons , ColorDouble ) ;
    g_tft.fillRect( x + g_GlobalVar.m_Screen.m_Largeur/m_NbButtons - m_Marge , g_GlobalVar.m_Screen.m_Hauteur - m_HauteurBoutons ,
                        m_Marge , m_HauteurBoutons , ColorDouble ) ;
    g_tft.fillRect( x , g_GlobalVar.m_Screen.m_Hauteur - m_Marge ,
                        g_GlobalVar.m_Screen.m_Largeur/m_NbButtons , m_Marge , ColorDouble ) ;
    g_tft.drawRect( x , g_GlobalVar.m_Screen.m_Hauteur - m_HauteurBoutons , g_GlobalVar.m_Screen.m_Largeur/m_NbButtons , m_HauteurBoutons , ColorTexte ) ;

    // position du bouton
    g_tft.setCursor( g_GlobalVar.m_Screen.m_Largeur / m_NbButtons * ib + 23  , g_GlobalVar.m_Screen.m_Hauteur - m_HauteurBoutons / 2 - 5 ) ;
    g_tft.setTextColor(ColorTexte) ;

    // texte du boutons
    g_tft.setTextSize(2) ;
    g_tft.print(m_Intitule[ib]);
    }
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Remet les boutons à zero
/// \param button == -1 : raz tpus boutons.
void CTouchButtons::RazButtons( int button )
{
if ( button == -1 )
    {
    m_CenterPressed = false ;
    for ( int ib = 0 ; ib < m_NbButtons ; ib++ )
        m_PressedArr[ib] = false ;
    }
else
    m_PressedArr[button] = false ;
m_ScreenPressed = false ;
//delay( 100 ) ;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief positionne le texte des boutons
void CTouchButtons::SetText( const char * Txt3 , int ib )
{
// taille du nouveau texte
int len = strlen(Txt3) ;

// si texte different pour forcer un reaffichage du text
if ( len != strlen( m_Intitule[ib] ) )
    m_PressedArrLast[ib] = !m_PressedArr[ib] ;
else
    {
    // comparaison de chaine
    for ( int ic = 0 ; ic <= len && ic <= 2 ; ic++ )
        {
        // si le texte a changé
        if ( Txt3[ic] != m_Intitule[ib][ic] )
            {
            m_PressedArrLast[ib] = !m_PressedArr[ib] ;
            break ;
            }
        }
    }

// positionnement texte bouton
for ( int ic = 0 ; ic < 4 ; ic++ )
    {
    if ( ic < len )
        m_Intitule[ib][ic] = Txt3[ic] ;
    else
        m_Intitule[ib][ic] = 0 ;
    }
}
