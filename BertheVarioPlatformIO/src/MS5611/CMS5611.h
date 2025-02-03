////////////////////////////////////////////////////////////////////////////////
/// \file CMS5611.h
///
/// \brief Fichier du capteur de pression
///
/// \date creation     : 07/03/2024
/// \date modification : 03/02/2025
///

#ifndef _CMS5611_
#define _CMS5611_

class CTrame ;
////////////////////////////////////////////////////////////////////////////////
/// \brief Capteur de pression et de temperature. Tache de calcul de l'altitude
/// pression filtree et de la Vz.
/// Precision du capteur 10cm.
/// Attention à la frequence du bus I2C CGlobalVar::InitI2C().
class CMS5611
{
public :

    void InitMs5611() ;

    void  LancerTacheCalculVzCapMag() ;
    void  SetAltiSolMetres( float AltiSol ) ;
    void  SetAltiSolUndef()                 ///< pour demmarrage en vol alti = alti baro pure
            { m_DiffAltiFchAgl = 0. ; } ;

    friend CTrame ;

    float GetAltiMetres() ;             ///< alti pression filtree recalee alti gps en debut de vol
    float GetTemperatureDegres() ;
    float GetPressureMb() ;

private :
    void  Read() ;
    float GetAltiPressionCapteurMetres() ;

    float m_AltiPressionFiltree ;       ///< altitude pression filtree
    float m_DiffAltiFchAgl = 0 ;        ///< difference avec alti fichier agl

    float CalcAltitude(float pressure_mb_x100 , float seaLevelPressure = 101325 ) ;

    static void TacheVzCapMag(void *param) ;
} ;

#endif
