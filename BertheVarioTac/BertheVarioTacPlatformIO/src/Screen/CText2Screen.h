////////////////////////////////////////////////////////////////////////////////
/// \file CText2Screen.h
///
/// \brief Texte a afficher
///
/// \date creation     : 21/09/2024
/// \date modification : 19/10/2025
///

#ifndef _TXT2SCREEN_
#define _TXT2SCREEN_

////////////////////////////////////////////////////////////////////////////////
/// \brief Du tete a afficher qui ne flick pas.
class CText2Screen
{
public :
    CText2Screen() ;
    ~CText2Screen() ;

    void SetPos( uint16_t x , uint16_t y , uint16_t taille , char Unite = ' ' ) ;
    void Affiche( const char * pChar , int color = TFT_WHITE ) ;
    void ChangeUnit( char c ) ;
    void ForceReaffichage()
        { m_ForceAffichage = true ; } ;

private :
    //std::string m_TextePrincipal ;  ///< texte courant principal
    CLGString   m_TextePrincipal ;  ///< texte courant principal
    bool        m_ForceAffichage = false ; ///< force un reaffichage
    char        m_Unite = ' ' ;
    uint16_t    m_x = 0 ;
    uint16_t    m_y = 0 ;
    uint16_t    m_Taille = 3 ;
    int16_t     m_XUnit ;
    int16_t     m_YUnit ;
} ;

#endif
