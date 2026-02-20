////////////////////////////////////////////////////////////////////////////////
/// \file CTouchButtons.h
///
/// \brief Boutons tactiles
///
/// \date creation   : 21/09/2024
/// \date 18/10/2025 : define boutons 0 1 2
///

#ifndef _TOUCHBUTTONS_
#define _TOUCHBUTTONS_

#define ANTI_REBONDS_MS 200

#define BOUTON_GAUCHE (0)
#define BOUTON_CENTRE (1)
#define BOUTON_DROIT  (2)

////////////////////////////////////////////////////////////////////////////////
/// \brief Pour les boutons tactiles en modifiables
class CTouchButtons : private CBrightness
{
public :
    CTouchButtons() ;

    void AfficheButtons() ;
    bool HandleButtons() ;
    bool IsButtonPressed( int ib ) ;
    void SetText( const char * Txt3 , int ib ) ;
    void RazButtons( int button = -1 ) ;

    void SetX( int x )
        { m_XTouch = x ; } ;
    void SetY( int y )
        { m_YTouch = y ; } ;
    int GetX() const
        { return m_XTouch ; } ;
    int GetY() const
        { return m_YTouch ; } ;
    void SetPressed() ;
    void SetFrozenDelay( int SecDelay ) ;
    bool IsCenterPressed() ;


    static void SetWant2Reboot( bool b = true ) ;
    static bool GetWant2Reboot() ;

    friend class CLGFX ;

    static bool ms_Want2Reboot ;

protected :
    int   m_XTouch = -1 ;       ///< coordonnees du touch
    int   m_YTouch = -1 ;       ///< coordonnees du touch
    bool  m_ScreenPressed = false ;   ///< si est appuyé sur l'ecran

private :
    unsigned long   m_TimePressed = 0 ;///< pour l' anti-rebond
    int             m_FrozenMs = 0 ;  ///< temps ou les boutons sont geles
    static const int m_NbButtons = 3 ;  ///< bombre de boutons a afficher
    bool            m_PressedArr[m_NbButtons] ;  ///< si boutons pressés
    bool            m_PressedArrLast[m_NbButtons] ;  ///< si boutons pressés pour affichage rapide
    char            m_Intitule[m_NbButtons][4] ; ///< texte de bouton
    bool            m_CenterPressed = false ; ///< si est appuyé en milieu d'ecran

    const int m_HauteurBoutons = 50 ;
    const int m_Marge = 10 ;

} ;

///////////////////////////////////////////////////////////////////////////////
/// \brief
inline void CTouchButtons::SetWant2Reboot( bool b )
{
ms_Want2Reboot = b ;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
inline bool CTouchButtons::GetWant2Reboot()
{
return ms_Want2Reboot ;
}

#endif
