////////////////////////////////////////////////////////////////////////////////
/// \file CTrame.h
///
/// \brief
///
/// \date creation     : 03/03/2024
/// \date 19/10/2025 : introduction du HDop
/// \date 21/10/2025 : nombre de fix altibaro, debut de vol
/// \date 25/10/2025 : '*' dans trame debut/fin message et GSA
/// \date 04/01/2026 : refonte TraiteBuffer()
///

#ifndef _TRAME_
#define _TRAME_

#define HDOP_EXCELLENT (1.4)
#define HDOP_BON       (3.)

#define TAILLE_BUFFER_TRAME (500)

////////////////////////////////////////////////////////////////////////////////
/// \brief Gestion trame Gps et rempissage variables CGlobalVar g_GlobalVar.
class CTrame
{
public :
    CTrame() ;

    void Push(char c) ;
    bool IsGpsOk() const
        { return m_MillisPremierGGA != 0 ; } ;
    void ForceGpsOk()
        { m_MillisPremierGGA = 1 ; } ;
    static unsigned long GetMillisPremierGGA()
        { return m_MillisPremierGGA ; } ;

    static int GetNbSat()   ///< renvoi le nombre de satellites en vue
        { return m_NombreSatellite ; } ;
    static float GetHDop()
        { return m_HDop ; } ;


protected :
    static unsigned long m_MillisPremierGGA ;   ///< temps du premier GGA pour stabilisation donnees gps
    static int           m_NombreSatellite ;    ///< nombre de satellites pour le GGA
    static float         m_HDop ;               ///< precision horizontale. < 3 excelent
    static float         m_PDopGSA ;            ///< precision globale
    static float         m_HDopGSA ;            ///< precision horizontale
    static float         m_VDopGSA ;            ///< precision verticale

private :
    //float m_ChecksumGsa ;
    void TraiteBuffer() ;
    bool StrCmp( const char * pa , const char * pb ) ;

    //float m_AltiGpsGeoide = 0 ;     ///< altitude du gps corrige du geoide

    CLGArray<char *> m_TabParam ;          ///< tableau des champs des messages
    char *           m_pCharBuff ;         ///< pointeur courant du buffer
    char *           m_pCharBuffOverflow ; ///< pointeur de la fin du buffer
    char             m_BufRecep[TAILLE_BUFFER_TRAME] = {0} ;    ///< buffer de reception de la trame
} ;

#endif
