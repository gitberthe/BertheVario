////////////////////////////////////////////////////////////////////////////////
/// \file CGroupeAligne.h
///
/// \brief
///
/// \date creation     : 28/11/2024
/// \date modification : 30/11/2024
///

#pragma once

////////////////////////////////////////////////////////////////////////////////
/// \brief Classe pour tri des groupes de points aligné par ordre croissant de taille.
class CGroupeAligne
{
public :
    std::vector<CVecZoneReduce::st_coord_poly*> m_Vect ;

    double GetBravaisPearson() ;

    bool operator > ( const CGroupeAligne & group ) const
            { return m_Vect.size() > group.m_Vect.size() ; } ;
    bool operator < ( const CGroupeAligne & group ) const
            { return m_Vect.size() < group.m_Vect.size() ; } ;

} ;

////////////////////////////////////////////////////////////////////////////////
/// \brief Classe de vecteur de groupes de points aligne pour tri.
class CVecGroupAligne : public std::vector<CGroupeAligne>
{
} ;

////////////////////////////////////////////////////////////////////////////////
/// \brief Classe de reorganisation de points par longitude.
class CSortPts
{
public :
    double m_Lon  ;
    int   m_ipts ;

    static void Sort( std::vector<CVecZoneReduce::st_coord_poly*> & VecToSort ) ;

    bool operator > ( const CSortPts & pts ) const
            { return m_Lon > pts.m_Lon ; } ;
    bool operator < ( const CSortPts & pts ) const
            { return m_Lon < pts.m_Lon ; } ;
} ;
