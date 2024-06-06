////////////////////////////////////////////////////////////////////////////////
/// \file CConfigFile.h
///
/// \brief
///
/// \date creation     : 08/03/2024
/// \date modification : 06/06/2024
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
        std::string m_NomVar ;  ///< nom de la variable dans fichier de configuration
        short       m_Type ;    ///< type de la variable
        void *      m_pVar ;    ///< pointeur sur la variable de la classe
        } ;

    void GetChar( int iVec , std::string & Name , std::string & Value ) const ;

    float m_coef_filtre_alti_baro = 0.65 ;   ///< coefficient de filtrage [0,1[ , 0.99 tres fort filtrage
    int   m_periode_integration_sec = 3 ;   ///< pour le calcul de la Vz
    int   m_vitesse_igc_kmh = 16 ;          ///< vitesse de declenchement enregistrement
    int   m_stab_gps = 30 ;                 ///< rayon dispersion gps pour stabilite
    float m_vz_seuil_haut = 0.5 ;           ///< seuils de beep
    float m_vz_seuil_bas = -2.4 ;           ///< seuils de beep
    int   m_dtu          = 1 ;              ///< diff heure TU
    int   m_AltiMargin   = 65 ;             ///< marge en altitude de zone TMA CTR en metres
    int   m_XYMargin     = 100 ;            ///< marge en xy de zone TMA CTR en metres
    bool  m_Termic       = true ;           ///< affichage termic ou terrain

    std::string m_Ssid ;     ///< identifiant wifi
    std::string m_Passwd ;   ///< mot de passe wifi
    std::string m_Pilote ;   ///< nom pilote fichier igc
    std::string m_Voile ;    ///< nom voile fichier igc

    void LectureFichier() ;
    void EcritureFichier() ;

    std::vector< st_line *>  m_LinesVect ; ///< vecteur des lignes

    static void ReplaceCharIn( std::string & str , char cfind , char creplace ) ;

private :
    void FreeVect() ;
} ;

#endif
