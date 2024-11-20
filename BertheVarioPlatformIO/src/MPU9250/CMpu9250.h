////////////////////////////////////////////////////////////////////////////////
/// \file CMpu9250.h
///
/// \brief Fichier du capteur magnetique
///
/// \date creation     : 15/03/2024
/// \date modification : 20/11/2024
///

#ifndef _CMYMPU9250_
#define _CMYMPU9250_

#define NOM_FCH_CAL_MAG     "/config/calmag.txt"

////////////////////////////////////////////////////////////////////////////////
/// \brief Capteur Accelerometrique et de cap magnetique.
/// Modifier le fichier ./Projects/BertheVario/.pio/libdeps/esp32dev/MPU9250/MPU9250.h,
/// ligne 85, static constexpr uint8_t MAG_MODE {0x02};
/// On peut ainsi baisser la frequence de lecture du capteur magnetique a 5hz sinon ça bug.
class CMpu9250
{
public :
    void Init() ;
    void Update() ;
    void Calibration() ;

    float m_CapMagnetique = 0. ; ///< cap magnetique horizontal, lecture 5hz apres modif de .pio/libdeps/esp32dev/MPU9250/MPU9250.h

private :
    float m_x , m_y , m_z ; ///< composantes du champs magnetique terrestre
} ;

#endif
