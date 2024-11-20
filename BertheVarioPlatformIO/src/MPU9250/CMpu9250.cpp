////////////////////////////////////////////////////////////////////////////////
/// \file CMpu9250.cpp
///
/// \brief Fichier du capteur magnetique
///
/// \date creation     : 15/03/2024
/// \date modification : 20/11/2024
///

#include "../BertheVario.h"

MPU9250 g_mpu ;

////////////////////////////////////////////////////////////////////////////////
/// \brief Initialisation du capteur et lecture du fichier de calibration.
void CMpu9250::Init()
{
MPU9250Setting setting;
setting.mag_output_bits = MAG_OUTPUT_BITS::M16BITS;
//MPU9250::MAG_MODE = 0x02 ;

/*setting.accel_fs_sel = ACCEL_FS_SEL::A16G;
setting.gyro_fs_sel = GYRO_FS_SEL::G2000DPS;
setting.mag_output_bits = MAG_OUTPUT_BITS::M16BITS;
setting.fifo_sample_rate = FIFO_SAMPLE_RATE::SMPL_125HZ;
setting.gyro_fchoice = 0x03;
setting.gyro_dlpf_cfg = GYRO_DLPF_CFG::DLPF_5HZ;
setting.accel_fchoice = 0x01;
setting.accel_dlpf_cfg = ACCEL_DLPF_CFG::DLPF_5HZ;*/

if (!g_mpu.setup(MPU_STATIC_ADDRESS, setting))
    {
    Serial.println("MPU connection failed. Please check your connection with `connection_check` example.");
    CGlobalVar::BeepError() ;
    }

//    mpu.verbose(true) ;

// declinaison magnetique
g_mpu.setMagneticDeclination( 2. ) ;

// lecture coefficients de calibration
File myFile ;
int ic = 0 ;

// ouverture fichier
myFile = SD.open( NOM_FCH_CAL_MAG );
if ( myFile )
    {
    char TmpChar[500] ;

    // lecture fichier
    while(myFile.available())
        {
        TmpChar[ic++] = myFile.read();
        }
    TmpChar[ic++] = 0 ;

    // decoupage en ligne
    float mbx=0., mby=0., mbz=0. ;
    float msx=0., msy=0., msz=0. ;
    int ipar = 0 ;
    char * pChar = strtok( TmpChar , "\n" ) ;
    while ( pChar != NULL )
        {
        if ( ipar == 0 )
            mbx = atof( pChar ) ;
        else if ( ipar == 1 )
            mby = atof( pChar ) ;
        else if ( ipar == 2 )
            mbz = atof( pChar ) ;
        else if ( ipar == 3 )
            msx = atof( pChar ) ;
        else if ( ipar == 4 )
            msy = atof( pChar ) ;
        else if ( ipar == 5 )
            msz = atof( pChar ) ;
        else
            Serial.println("erreur parametre cap magnetique" ) ;
        ipar++ ;
        pChar = strtok( NULL , "\n" ) ;
        }

    g_mpu.setMagBias(mbx, mby, mbz ) ;
    g_mpu.setMagScale(msx,msy,msz) ;

    myFile.close() ;
    }
else
    Serial.println("Erreur ouverture fichier de calibration magnetique.") ;

}

////////////////////////////////////////////////////////////////////////////////
/// \brief Calcul le cap magnetique sur le plan horizontal, ou vertical suivant
/// VARIO_CAP_MAG_A_PLAT.
void CMpu9250::Update()
{
if ( g_mpu.update() )
    {
    m_x = g_mpu.getMagX() ;
    m_y = g_mpu.getMagY() ;
    m_z = g_mpu.getMagZ() ;

    #ifdef MPU9250_DEBUG
    Serial.print("cm:");
    Serial.print( m_x ) ;
    Serial.print( "," ) ;
    Serial.print( m_y ) ;
    Serial.print( "," ) ;
    Serial.println( m_z ) ;
    #endif // MPU9250_DEBUG

    float Angle = 0 ;
    #if VARIO_CAP_MAG_A_PLAT == 1 // a plat devant
     Angle = +270 - 180. / T_PI * atan2f ( m_y ,  m_x ) ;
    #endif
    #if VARIO_CAP_MAG_A_PLAT == 2 // sur suspente droite
     Angle = +270 - 180. / T_PI * atan2f ( m_x ,  m_z ) ;
    #endif
    #if VARIO_CAP_MAG_A_PLAT == 3 // sur suspente gauche
     Angle = +90 - 180. / T_PI * atan2f ( m_x ,  m_z ) ;
    #endif

    if ( Angle >= 360. )
        Angle -= 360. ;
    if ( Angle < 0 )
        Angle += 360. ;
    m_CapMagnetique = Angle ;
    }
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Ecrit le fichier de calibration
void CMpu9250::Calibration()
{
g_mpu.calibrateMag() ;

File myFile ;

// ouverture fichier
myFile = SD.open( NOM_FCH_CAL_MAG, FILE_WRITE , true );
if (!myFile)
    {
    Serial.print( "erreur creation fichier " ) ;
    Serial.println( NOM_FCH_CAL_MAG ) ;
    return ;
    }

// debut de fichier
myFile.seek(0) ;
myFile.println( g_mpu.getMagBias(0) ) ;
myFile.println( g_mpu.getMagBias(1) ) ;
myFile.println( g_mpu.getMagBias(2) ) ;
myFile.println( g_mpu.getMagScale(0) ) ;
myFile.println( g_mpu.getMagScale(1) ) ;
myFile.println( g_mpu.getMagScale(2) ) ;

myFile.close() ;
}

