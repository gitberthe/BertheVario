////////////////////////////////////////////////////////////////////////////////
/// \file CConfigFile.h
///
/// \brief
///
/// \date creation     : 08/03/2024
/// \date modification : 08/07/2025 suppression xc_track et limite zone xy alti
/// \date modification : 14/10/2025 m_sat_sec = 14 et plus de StabGps
/// \date 21/10/2024 : nombre de fix altibaro, debut de vol
/// \date 27/11/2025 : vz a 0.6 par defaut au decollage.
///

#ifndef _CCONFIGFILE_
#define _CCONFIGFILE_

#define CONFIG_FCH "/config/config.txt"

#define TYPE_VAR_STR   0
#define TYPE_VAR_FLOAT 1
#define TYPE_VAR_INT   2
#define TYPE_VAR_BOOL  3

////////////////////////////////////////////////////////////////////////////////
/// \brief Fichier de configuration de variables sur la carte SD.
class CConfigFile
{
public :

    CConfigFile() ;

    ///< une ligne du fichier
    struct st_line
        {
        CLGString  m_NomVar ;  ///< nom de la variable dans fichier de configuration
        short      m_Type ;    ///< type de la variable
        void *     m_pVar ;    ///< pointeur sur la variable de la classe
        } ;

    void GetChar( int iVec , CLGString & Name , CLGString & Value ) const ;

    float       m_coef_filtre_alti_baro = 0.85 ;  ///< coefficient de filtrage [0,1[ , 0.99 tres fort filtrage
    int         m_vitesse_igc_kmh = 12 ;          ///< vitesse de declenchement enregistrement
    float       m_vz_igc_ms = 0.6 ;               ///< vz de vitesse pour declenchement
    float       m_vz_seuil_max  = 4. ;            ///< seuils de beep
    float       m_vz_seuil_haut = 0.2 ;           ///< seuils de beep
    float       m_vz_seuil_bas  = -2.5 ;          ///< seuils de beep
    int         m_dtu           = 2 ;             ///< diff heure TU
    bool        m_alarme_reculade = false ;       ///< alarme sonore en cas de reculade
    #ifdef _BERTHE_VARIO_TAC_
     int   m_lum_seconde  = 20 ;             ///< valeur en seconde luminosite maximale sol
    #endif

    CLGString m_Ssid ;     ///< identifiant wifi
    CLGString m_Passwd ;   ///< mot de passe wifi
    CLGString m_Pilote ;   ///< nom pilote fichier igc
    CLGString m_Voile ;    ///< nom voile fichier igc

    void LectureFichier() ;
    void EcritureFichier() ;
    void ConstructVect() ;
    void FreeVect() ;

    CLGArrayPtr<st_line>  m_LinesVect ; ///< vecteur des lignes

    static void ReplaceCharIn( CLGString & str , char cfind , char creplace ) ;

protected :
} ;

#endif
