////////////////////////////////////////////////////////////////////////////////
/// \file CConfigFile.h
///
/// \brief
///
/// \date creation     : 08/03/2024
/// \date modification : 03/02/2025
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
class CConfigFile : public CNbSatDelay
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

    float m_coef_filtre_alti_baro = 0.85 ;   ///< coefficient de filtrage [0,1[ , 0.99 tres fort filtrage
    //int   m_periode_integration_sec = 2 ;   ///< pour le calcul de la Vz
    int   m_vitesse_igc_kmh = 14 ;          ///< vitesse de declenchement enregistrement
    float m_vz_igc_ms = 0.5 ;               ///< vz de vitesse pour declenchement
    int   m_stab_gps_metre = 30 ;           ///< rayon dispersion gps pour stabilite
    int   m_stab_gps_sec = 10 ;             ///< taille pile en sec pour calcul rayon dispersion gps
    int   m_temps_igc_sec = 4 ;             ///< nombre de secondes de vitesse pour declenchement vol
    float m_vz_seuil_max  = 4. ;            ///< seuils de beep
    float m_vz_seuil_haut = 0.2 ;           ///< seuils de beep
    float m_vz_seuil_bas = -2.5 ;           ///< seuils de beep
    int   m_dtu          = 2 ;              ///< diff heure TU
    int   m_AltiMargin   = 40 ;             ///< marge en altitude de zone TMA CTR en metres
    int   m_XYMargin     = 10 ;             ///< marge en xy de zone TMA CTR en metres
    bool  m_alarme_reculade = false ;       ///< alarme sonore en cas de reculade
    #ifdef XC_TRACK
     bool  m_xc_track     = false ;          ///< bluetooth xc-track de la pression
    #endif

    std::string m_Ssid ;     ///< identifiant wifi
    std::string m_Passwd ;   ///< mot de passe wifi
    std::string m_Pilote ;   ///< nom pilote fichier igc
    std::string m_Voile ;    ///< nom voile fichier igc

    void LectureFichier() ;
    void EcritureFichier() ;
    void ConstructVect() ;
    void FreeVect() ;

    std::vector< st_line *>  m_LinesVect ; ///< vecteur des lignes

    static void ReplaceCharIn( std::string & str , char cfind , char creplace ) ;

    friend CNbSatDelay ;

protected :
    int   m_sat_sec = 12 ;                   ///< secondes d'interdiction debut vol vitesse cause changement nombre satellites (pour 4 satellites)
} ;

#endif
