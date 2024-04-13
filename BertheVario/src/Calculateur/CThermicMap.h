////////////////////////////////////////////////////////////////////////////////
/// \file CThermicMap.h
///
/// \brief
///
/// \date creation     : 23/03/2024
/// \date modification : 03/04/2024
///

#ifndef _THERMICMAP_
#define _THERMICMAP_

#define UnMileEnMetres         (1852.)
#define T_PI                   (3.1415926535)

#define ResolutionEnMetreMapXY   (25.)
#define ResolutionEnMetreMapZ    (10.)

#define VzMin4Clear              (-0.5)

////////////////////////////////////////////////////////////////////////////////
/// \brief Classe de 64bits, qui est la clee en x/y/z de la map des CVzSum.
/// pour une position de ResolutionEnMetreMapXY et Z, on obtient la CVzSum depuis
/// le debut du vol. 2^15-1 * 25m => +- 819,175km
/// Chaque case est une case 3D de x25*y25*z10m.
union st_key
{
    long long m_l ;     ///< donnees en 64 bits pour la map (???)
    struct
        {
        int m_x : 16 ;  ///< index x de resolution ResolutionEnMetreMapXY metres de dimension
        int m_y : 16 ;  ///< index y de resolution ResolutionEnMetreMapXY metres de dimension
        int m_z : 16 ;  ///< index z de resolution ResolutionEnMetreMapZ metres de haut
        } ;

    bool operator < (const st_key & key) const
        { return m_l < key.m_l ; } ;
    bool operator > (const st_key & key) const
        { return m_l > key.m_l ; } ;
    bool operator == (const st_key & key) const
        { return m_l == key.m_l ; } ;

    void SetXYZRec( float LonDeg , float LatDeg, int Alti ) ;
} ;

////////////////////////////////////////////////////////////////////////////////
/// \brief Classe qui memorise la Vz d'une case et sa position 3D, au fil du vol
/// pour tout les passages.
class CVzSum
{
public :
    void AddVz( float Vz , st_key key )
        { m_VzSum += Vz ; m_VzNb++ ; m_key = key ; } ;
    float GetAverage() const
        { return m_VzSum / m_VzNb ; } ;

    bool operator < ( const CVzSum & Vz ) const ;
    bool operator > ( const CVzSum & Vz ) const ;

    static void SetPosCourante() ;
    static void SetTriDistance( bool tri )
                    { ms_TriDistance = tri ; } ;

    short   m_Voisinage = 0 ;///< nombre de case en voisinnage avec Vz superieur a seuil
    st_key  m_key ; ///< clee/position pour suppression et tri de proximite

private :
    float GetDistPosCurSquare() const ;

    float m_VzSum = 0. ; ///< sum des Vz
    int   m_VzNb  = 0 ;  ///< nombre de vz enregistres

    static int ms_x_cur ;    ///< index x position courante pour calcul et tri proximite
    static int ms_y_cur ;    ///< index y position courante pour calcul et tri proximite
    static int ms_z_cur ;    ///< index z position courante pour calcul et tri proximite
    static bool ms_TriDistance ; ///< tri Distance ou Vz

} ;

////////////////////////////////////////////////////////////////////////////////
/// \brief Classe historique du vol sous forme de map. Enregistre les Vz au cours
/// du vol. Ce destine a donner, pour la position courante, le thermique le plus proche.
/// Composee de case 3d de ResolutionEnMetreMapXY et Z ayant comme origine le point de decollage.
/// Nettoyage periodique des case sous VzMin4Clear.
class CThermicMap : public std::map<st_key, CVzSum>
{
public :
    void LancerTacheTermic() ;

    float     m_GisementDeg = -1 ;
    float     m_DistanceMetres = -1 ;
    CMutex    m_MutexMap ;    ///< comme mutitache il faut un mutex map

    void CalcTermicProche() ;

private :
    const int   m_NombreVoisinMin = 1 ; ///< pour la supression des cases isolee
    const int   m_VolumeVoisinage = 3 ; ///< +- volume de voisinage
    std::vector<CVzSum> m_VecVzAutour ; ///< pour clean map

    void PushVz( float Vz , float LatDeg , float LonDeg , int Alti ) ;
    void CleanMap( bool WithNeighbour = false ) ;
    void CalcVoisinage() ;

    static void TacheTermic(void * param) ;
} ;

extern CThermicMap g_TermicMap ;

#endif
