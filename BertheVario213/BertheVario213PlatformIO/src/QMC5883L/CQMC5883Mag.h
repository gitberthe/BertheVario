////////////////////////////////////////////////////////////////////////////////
/// \file CQMC5883Mag.h
///
/// \brief Capteur magnetique.
///
/// \date creation     : 04/10/2024
/// \date modification : 05/10/2024
///

#ifndef _QMC5883MAG_
#define _QMC5883MAG_

#define NOM_FCH_CAL_MAG "/config/calmag.txt"

class CQMC5883Mag
{
public :
    void    InitMagnetique() ;
    void    LectureCap() ;
    void    CalibrationMagnetique() ;
    int     GetCapDegres() const
                { return m_CapMagnetique ; } ;

    void    ScanI2C() ;

private :
    void LectureFichierCalibration() ;
    void EcritureFichierCalibration() ;

    float   m_ZeroX = 0. ;
    float   m_ZeroY = 0. ;
    float   m_GainX = 1. ;
    float   m_GainY = 1. ;
    int     m_CapMagnetique = -1. ;   ///< cap magnetique en degres
} ;

#endif
