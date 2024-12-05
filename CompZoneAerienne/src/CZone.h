////////////////////////////////////////////////////////////////////////////////
/// \file CZone.h
///
/// \brief
///
/// \date 01/12/2024 : creation
/// \date 05/12/2024 : modification
///

#pragma once

////////////////////////////////////////////////////////////////////////////////
/// \brief Zone aerienne pour tri suivant nombre de points. Pour optimization
/// memoire/compression zone BertheVario
class CZone
{
public :
    std::string     m_Name ;                        ///< nom de la zone
    long            m_Bottom ;                      ///< altitude basse de la zone
    long            m_NbPtsAvantCompression = 0 ;   ///< nombre de points avant compression
    long            m_NbPtsApresCompression = 0 ;   ///< nombre de points apres compression
    double          m_ErreurMoyenneEnMetre = 0. ;   ///< erreur moyenne en metres de la zone compressee par rapport à la zone non compressee
    std::vector<CVecZoneReduce::st_coord_poly*> m_VecPtsBig ;   ///< vecteur des points avant compression
    std::vector<CVecZoneReduce::st_coord_poly*> m_VecPtsSmall ; ///< vecteur des points apres compression

    void    Compress() ;
    double  GetRayonMaxKm() const ;

    bool operator < ( const CZone & Zone ) const
        { return m_VecPtsSmall.size() < Zone.m_VecPtsSmall.size() ; } ;
    bool operator > ( const CZone & Zone ) const
        { return m_VecPtsSmall.size() > Zone.m_VecPtsSmall.size() ; } ;
    bool operator == ( const CZone & Zone ) const
        { return ! ( (*this) != Zone ) ; } ;
    bool operator != ( const CZone & Zone ) const ;
} ;
