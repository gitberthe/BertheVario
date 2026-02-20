////////////////////////////////////////////////////////////////////////////////
/// \file CQMC5883Mag.cpp
///
/// \brief Capteur magnetique.
///
/// \date creation     : 04/10/2024
/// \date modification : 20/03/2025
///

#include "../BertheVario213.h"

DFRobot_QMC5883 compass(&Wire, /*I2C addr*/QMC5883_ADDRESS);

////////////////////////////////////////////////////////////////////////////////
/// \brief Scan des adresse I2C
void CQMC5883Mag::ScanI2C()
{
byte error, address;
int nDevices;

Serial.println("Scanning...");

nDevices = 0;
for(address = 1; address < 127; address++ )
    {
    // The i2c_scanner uses the return value of
    // the Write.endTransmisstion to see if
    // a device did acknowledge to the address.
    Wire.beginTransmission(address);
    error = Wire.endTransmission();

    if (error == 0)
        {
        Serial.print("I2C device found at address 0x");
        if (address<16)
            Serial.print("0");
        Serial.print(address,HEX);
        Serial.println("  !");

        nDevices++;
        }
    else if (error==4)
        {
        Serial.print("Unknown error at address 0x");
        if (address<16)
            Serial.print("0");
        Serial.println(address,HEX);
        }
    }
if (nDevices == 0)
    Serial.println("No I2C devices found\n");
else
    Serial.println("done\n");
}

////////////////////////////////////////////////////////////////////////////////
/// \brief initialisation du capteur magnetique, lecture fichier calibration.
void CQMC5883Mag::InitMagnetique()
{
if ( compass.begin() )
    {
    m_CapMagnetique = 0.0 ;
    compass.setDataRate( QMC5883_DATARATE_10HZ ) ;
    //compass.setRange( QMC5883_RANGE_8GA ) ;
    compass.setRange( QMC5883_RANGE_2GA ) ;
    compass.setSamples(QMC5883_SAMPLES_8);
    }
else
    m_CapMagnetique = 360. ;

LectureFichierCalibration() ;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Lecture des vecteurs magnetique et etalonnage pour cap.
void CQMC5883Mag::LectureCap()
{
sVector_t mag = compass.readRaw();
//compass.getHeadingDegrees();

float x = (((float)mag.XAxis)-m_ZeroX) * m_GainX ;
float y = (((float)mag.YAxis)-m_ZeroY) * m_GainY ;

float Cap = -atan2f(x,y) * 180. / T_PI ;

if ( Cap < 0. )
    Cap += 360. ;

if ( Cap > 360. )
    Cap -= 360. ;

#ifdef MAG_DEBUG
 Serial.println( "---" ) ;
 Serial.print( "x:" ) ; Serial.println( x ) ;
 Serial.print( "y:" ) ; Serial.println( y ) ;
 Serial.print( "0x:" ) ; Serial.println( m_ZeroX ) ;
 Serial.print( "0y:" ) ; Serial.println( m_ZeroY ) ;
 Serial.print( "dx:" ) ; Serial.println( m_GainX ) ;
 Serial.print( "dy:" ) ; Serial.println( m_GainY ) ;
 //Serial.print( "z:" ) ; Serial.println( z ) ;
#endif

//if ( cap < 0. )
//    cap = 360 - cap ;

/*if ( cap > 360. )
    cap -= 360. ;*/

m_CapMagnetique = Cap ;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Calibration des vecteurs magnetiques
void CQMC5883Mag::CalibrationMagnetique()
{
unsigned long TempsDebut = millis() ;

float MinX = __FLT_MAX__ ;
float MinY = __FLT_MAX__ ;
float MaxX = -__FLT_MAX__ ;
float MaxY = -__FLT_MAX__ ;

// pendant 40 s
while ( (((long)(millis()-TempsDebut))/((long)1000)) < 40 )
    {
    // 4 hz
    delay( 250 ) ;
    // mesure capteur
    sVector_t mag = compass.readRaw();
    // min max
    MinX = std::min( MinX , ((float)mag.XAxis) ) ;
    MinY = std::min( MinY , ((float)mag.YAxis) ) ;
    MaxX = std::max( MaxX , ((float)mag.XAxis) ) ;
    MaxY = std::max( MaxY , ((float)mag.YAxis) ) ;
    }

m_ZeroX = (MaxX+MinX) / 2. ;
m_ZeroY = (MaxY+MinY) / 2. ;
m_GainX = 1./(MaxX-MinX) ;
m_GainY = 1./(MaxY-MinY) ;

EcritureFichierCalibration() ;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Lecture des parametres de calibration X/Y
void CQMC5883Mag::LectureFichierCalibration()
{
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
    int ipar = 0 ;
    char * pChar = strtok( TmpChar , "\n" ) ;
    while ( pChar != NULL )
        {
        if ( ipar == 0 )
            m_ZeroX = atof( pChar ) ;
        else if ( ipar == 1 )
            m_ZeroY = atof( pChar ) ;
        else if ( ipar == 2 )
            m_GainX = atof( pChar )/1000. ;
        else if ( ipar == 3 )
            m_GainY = atof( pChar )/1000. ;
        else
            Serial.println("erreur parametre cap magnetique" ) ;
        ipar++ ;
        pChar = strtok( NULL , "\n" ) ;
        }

    myFile.close() ;
    }
else
    Serial.println("Erreur ouverture fichier de calibration magnetique.") ;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Ecriture des parametres de calibration X/Y
void CQMC5883Mag::EcritureFichierCalibration()
{
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
myFile.println( m_ZeroX ) ;
myFile.println( m_ZeroY ) ;
myFile.println( m_GainX*1000. ) ;
myFile.println( m_GainY*1000. ) ;

myFile.close() ;
}

