////////////////////////////////////////////////////////////////////////////////
/// \file CVecZoneReduce.cpp
///
/// \brief
///
/// \date creation     : 25/11/2024
/// \date modification : 30/11/2024
///

#include "CompZoneAerienne.h"

////////////////////////////////////////////////////////////////////////////////
/// \brief Reduit le vecteur en nombre de points de facon iteratives. Cumul de
/// 3 methodes. Par distance à une droite, distance entre les points et par angle
/// d'alignement si parametre != -1.
void CVecZoneReduce::ReduceToDistanceDroiteAngleDistancePoint( double DistanceMetresEcartDroite , double MemeDirectionEnDegres , double DistanceMetresEntrePoints )
{
// temps que un points a ete deletruit
bool DeleteVec = true ;
while ( DeleteVec )
    {
    DeleteVec = false ;

    const long TotalPts = 3 ;
    // supression des points 3 a 3
    for ( long ipa = 0 ; ipa+2 < (long)m_pVecOrigine->size() ; ipa += 1 )
        {
        // vecteur des 3 points
        std::vector< CPoint2D > Vect3pts ;
        for ( long ipb = 0 ; ipb < TotalPts && (ipa+ipb) < (long)m_pVecOrigine->size() ; ipb++ )
            Vect3pts.push_back( CPoint2D( (*m_pVecOrigine)[ipa+ipb]->m_Lon , (*m_pVecOrigine)[ipa+ipb]->m_Lat) ) ;

        // si pas assez de points
        if ( Vect3pts.size() < TotalPts )
            break;

        // calcul des vecteurs des 3 points
        CVecteur2D VecDir( Vect3pts[2] , Vect3pts[0] ) ;
        CVecteur2D VecAutre( Vect3pts[1] , Vect3pts[0] ) ;

        // calcul de la droite des 2 extremites
        CDroite2D  Droite( VecDir , Vect3pts[0] ) ;

        // vecteur perpendiculaire à la droite
        CPoint2D PtProj = Droite.GetProjectionDuPoint( Vect3pts[1] ) ;
        CVecteur2D VecPerendiculaire( PtProj , Vect3pts[1] ) ;

        // destruction de points si distance a la droite inferieur
        if ( DistanceMetresEcartDroite > 0 && VecPerendiculaire.GetNorm() * MilesParDegres * UnMileEnMetres < DistanceMetresEcartDroite )
            {
            DeleteVec = true ;
            delete (*m_pVecOrigine)[ipa+1] ;
            (*m_pVecOrigine)[ipa+1] = NULL ;
            ipa-- ;
            break ;
            }

        // destruction si distance entre points inferieur
        double Dist1 = Vect3pts[1].GetDistanceAuPoint( Vect3pts[0] ) * MilesParDegres * UnMileEnMetres ;
        double Dist2 = Vect3pts[1].GetDistanceAuPoint( Vect3pts[2] ) * MilesParDegres * UnMileEnMetres ;

        if ( DistanceMetresEntrePoints > 0 && Dist1 < DistanceMetresEntrePoints )
            {
            DeleteVec = true ;
            delete (*m_pVecOrigine)[ipa] ;
            (*m_pVecOrigine)[ipa] = NULL ;
            ipa-- ;
            break ;
            }
        if ( DistanceMetresEntrePoints > 0 && Dist2 < DistanceMetresEntrePoints )
            {
            DeleteVec = true ;
            delete (*m_pVecOrigine)[ipa+2] ;
            (*m_pVecOrigine)[ipa+2] = NULL ;
            ipa-- ;
            break ;
            }

        // destruction si angle plat inferieur
        double AngleDeg = VecDir.GetAngleDeg(VecAutre) ;
        if ( MemeDirectionEnDegres > 0 &&
             (AngleDeg < MemeDirectionEnDegres || AngleDeg > (180-MemeDirectionEnDegres)) )
            {
            DeleteVec = true ;
            delete (*m_pVecOrigine)[ipa+1] ;
            (*m_pVecOrigine)[ipa+1] = NULL ;
            ipa-- ;
            break ;
            }
        }

    // recopie des points non suprimmes
    std::vector<st_coord_poly*> VecReduce ;
    for ( long ipt = 0 ; ipt < (int)m_pVecOrigine->size() ; ipt++ )
        {
        if ( (*m_pVecOrigine)[ipt] != NULL )
            VecReduce.push_back( (*m_pVecOrigine)[ipt] ) ;
        }

    // recopie du vecteur reduit
    *m_pVecOrigine = VecReduce ;
    //m_pVecOrigine->shrink_to_fit() ;
    }
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Reduit le vecteur en nombre de points. Prends les points 3 a 3 et
/// supprime les points centreaux s'il est a une distance trop importante à
/// la droite qui relie les 2 extremes.
int CVecZoneReduce::ReduceMultiDistanceDroite( double DistanceMetresEcartDroite )
{
std::vector<st_coord_poly*> VecReduce ;
const long size = m_pVecOrigine->size() ;

// pour tous les points
long NbPts = 0 ;
for ( long ipall = 0 ; ipall < size ; ipall+=NbPts-1 )
    {
    bool PtsDeleted = false ;
    NbPts = 0 ;

    // nombre de points croissant/glissant
    std::vector<st_coord_poly*> VecSort ;
    for ( NbPts = 3 ; NbPts + ipall < size ; NbPts++ )
        {
        VecSort.clear() ;
        for ( long ipts = 0 ; ipts < NbPts ; ipts++ )
            if ( ipall+ipts < size )
                VecSort.push_back( (*m_pVecOrigine)[ipall+ipts] ) ;

        CSortPts::Sort( VecSort ) ;

        // vecteur de points en cours
        std::vector< CPoint2D > Vectxpts ;
        for ( long ipb = 0 ; ipb < NbPts ; ipb++ )
            Vectxpts.push_back( CPoint2D( VecSort[ipb]->m_Lon , VecSort[ipb]->m_Lat) ) ;

        /*CVecteur2D VecDir1( Vectxpts[0] , Vectxpts[Vectxpts.size()-1] ) ;
        CVecteur2D VecDir2( Vectxpts[0] , Vectxpts[Vectxpts.size()-2] ) ;
        double Angle = VecDir1.GetAngleDeg(VecDir2) ;*/

        // calcul de la droite des extremites
        CVecteur2D VecDir( Vectxpts[0] , Vectxpts[Vectxpts.size()-1] ) ;
        CDroite2D  Droite( VecDir , Vectxpts[0] ) ;

        // vecteur perpendiculaire à l'avant dernier point rajouté
        CPoint2D PtProj = Droite.GetProjectionDuPoint( Vectxpts[Vectxpts.size()-2] ) ;
        CVecteur2D VecPerendiculaire( PtProj , Vectxpts[Vectxpts.size()-2] ) ;

        // destruction de points si distance a la droite sufisante
        if ( DistanceMetresEcartDroite > 0 && VecPerendiculaire.GetNorm() * MilesParDegres * UnMileEnMetres > DistanceMetresEcartDroite )
        //if ( Angle > 3 )
            {
            //NbPts-- ;
            if ( VecSort.size() > 1 )
                {
                VecSort.resize( VecSort.size()-1 ) ;
                CSortPts::Sort( VecSort ) ;
                }
            break ;
            }

        PtsDeleted = true ;
        }

    // si pas de points supprime
    if ( PtsDeleted )
        {
        // tri en ligne des points par longitude croissante
        AjoutePointSansDoublons( VecReduce , VecSort[0] ) ;
        AjoutePointSansDoublons( VecReduce , VecSort[VecSort.size()-1] ) ;
        NbPts = VecSort.size()-1 ;
        }
    else
        {
        NbPts = 2 ;
        AjoutePointSansDoublons( VecReduce , (*m_pVecOrigine)[ipall] ) ;
        }
    }

// recopie du vecteur reduit
int ret = m_pVecOrigine->size() - VecReduce.size() ;
*m_pVecOrigine = VecReduce ;
return ret ;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Reduit les zones par approximation de nuage de points par une droite.
void CVecZoneReduce::ReduceNuageDroite( double DistanceMetresEcartDroite )
{
std::vector<st_coord_poly*> VecReduce ;

const long size = m_pVecOrigine->size() ;

CVecGroupAligne VecGroupAligne ;

// pour tous les points
long NbPts = 0 ;
for ( long ipall = 0 ; ipall < size ; ipall+=NbPts-1 )
    {
//    bool PtsDeleted = false ;
//    NbPts = 0 ;

    // nombre de points croissant/glissant
    const int debpts = 3 ;
    std::vector<st_coord_poly*> VecSort ;
    for ( NbPts = debpts ; NbPts + ipall < size ; NbPts++ )
        {
        // vecteur des points en cours
        VecSort.clear() ;
        for ( long ipts = 0 ; ipts < NbPts ; ipts++ )
            if ( ipall+ipts < size )
                VecSort.push_back( (*m_pVecOrigine)[ipall+ipts] ) ;

        if ( VecSort.size() < debpts )
            break ;

        // tri vecteur par longitude
        CSortPts::Sort( VecSort ) ;

        // droite du nuage de points
        CNuage2Droite Nuage2Droite ;
        CDroite2D Droite = Nuage2Droite.ApproxDroite( VecSort ) ;

        // vecteur de points 2D en cours
        std::vector< CPoint2D > Vectxpts ;
        for ( long ipb = 0 ; ipb < NbPts ; ipb++ )
            Vectxpts.push_back( CPoint2D( VecSort[ipb]->m_Lon , VecSort[ipb]->m_Lat) ) ;

        // calcul de la distance a la droite a chaque point
        int NuageOk = 0 ;
        for ( long ipts = 0 ; ipts < NbPts ; ipts++ )
            {
            // vecteur perpendiculaire à l'avant dernier point rajouté
            CPoint2D PtProj = Droite.GetProjectionDuPoint( Vectxpts[ipts] ) ;
            CVecteur2D VecPerendiculaire( PtProj , Vectxpts[ipts] ) ;

            // destruction de points si distance a la droite sufisante
            if ( DistanceMetresEcartDroite > 0 && VecPerendiculaire.GetNorm() * MilesParDegres * UnMileEnMetres > DistanceMetresEcartDroite )
                NuageOk++ ;
            }

        // si nuage pas aligne
        if ( NuageOk > 0 )
            {
            // on enleve le dernier points
            if ( VecSort.size() > 1 )
                {
                VecSort.resize( VecSort.size()-1 ) ;
                CGroupeAligne GroupAligne ;
                GroupAligne.m_Vect = VecSort ;
                VecGroupAligne.push_back( GroupAligne ) ;
                }
            NbPts-- ;
            break ;
            }
        }
    }

std::sort( VecGroupAligne.begin() , VecGroupAligne.end() ) ;


//for ( size_t ig = 0 ; ig < VecGroupAligne.size() ; ig++ )
for ( size_t ig = VecGroupAligne.size() - 1 ; ig > 0 ; ig-- )
    {
    std::vector<st_coord_poly*> & VecAligne = VecGroupAligne[ig].m_Vect ;

    // tri vecteur par longitude
    //CSortPts::Sort( VecAligne ) ;

    if ( VecAligne.size() > 3 )
        {
        //bool cont = false ;
        // si les bouts sont deja detuit
        //for ( size_t ip = 0 ; ip < VecAligne.size() ; ip++ )
        //if ( VecAligne[0]->m_PtADetuire || VecAligne[VecAligne.size()-1]->m_PtADetuire )
        //    continue ;
        //    if ( VecAligne[ip]->m_PtADetuire )
        //        cont = true ;

        //if ( cont )
        //    continue ;
        for ( size_t ip = 1 ; ip < VecAligne.size()-1 ; ip++ )
            VecAligne[ip]->m_PtADetuire = true ;
        }
    }

// recopie des points pas a detruire
VecReduce.clear() ;
for ( size_t ipo = 0 ; ipo < m_pVecOrigine->size() ; ipo++ )
    {
    st_coord_poly * pSt = (*m_pVecOrigine)[ipo] ;
    if ( ! pSt->m_PtADetuire )
        VecReduce.push_back( pSt ) ;
    }
// recopie du vecteur reduit
*m_pVecOrigine = VecReduce ;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Ajoute des points au vecteur sans doublons
void CVecZoneReduce::AjoutePointSansDoublons( std::vector<st_coord_poly*> & VecReduce , st_coord_poly* pStCoord )
{
if ( pStCoord == NULL )
    return ;

for ( size_t ip = 0 ; ip < VecReduce.size() ; ip++ )
    if ( pStCoord->m_Lat == VecReduce[ip]->m_Lat && pStCoord->m_Lon == VecReduce[ip]->m_Lon )
        return ;

VecReduce.push_back( pStCoord ) ;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Ajoute des points au vecteur sans doublons
void CVecZoneReduce::AjoutePointSansDoublons( std::vector<st_coord_poly*> & VecReduce , int ipa )
{
st_coord_poly* ppt = (*m_pVecOrigine)[ipa] ;

for ( size_t ip = 0 ; ip < VecReduce.size() ; ip++ )
    if ( ppt == VecReduce[ip] )
        return ;

VecReduce.push_back( ppt ) ;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief
void CVecZoneReduce::ReduceNuageBravaisPearson( double DistanceMetresEcartDroite , double CoefBravaisPearson )
{
std::vector<st_coord_poly*> VecReduce ;
const long size = m_pVecOrigine->size() ;

CVecGroupAligne VecGroupAligne ;

// pour tous les points
long NbPts = 0 ;
const int debpts = 3 ;
for ( long ipall = 0 ; ipall < size ; ipall+=1 )
    {
    // nombre de points croissant/glissant
    //double CoefBravaisPearsonOld = -DBL_MAX ;
    CGroupeAligne GroupeAligneOld ;
    CGroupeAligne GroupeAligneCur ;
    for ( NbPts = debpts ; NbPts + ipall < size ; NbPts++ )
        {
        // vecteur des points en cours
        GroupeAligneCur.m_Vect.clear() ;
        for ( long ipts = 0 ; ipts < NbPts ; ipts++ )
            if ( ipall+ipts < size )
                GroupeAligneCur.m_Vect.push_back( (*m_pVecOrigine)[ipall+ipts] ) ;

        // verif taille
        if ( GroupeAligneCur.m_Vect.size() < debpts )
            break ;

        // progression du coefficent BP
        //double CoefBravaisPearsonCur = GroupeAligneCur.GetBravaisPearson() ;
        //CoefBravaisPearsonCur = pow( CoefBravaisPearsonCur , 1./(1.+0.9*((1E-17)*GroupeAligneCur.m_Vect.size())) ) ;
        bool Break = false ;
        //if ( CoefBravaisPearsonOld > -DBL_MAX && abs(CoefBravaisPearsonCur) < 0.999*abs(CoefBravaisPearsonOld) )
            {
            std::vector< CPoint2D > Vectxpts ;
            for ( long ipb = 0 ; ipb < (long)GroupeAligneCur.m_Vect.size() ; ipb++ )
                Vectxpts.push_back( CPoint2D( GroupeAligneCur.m_Vect[ipb]->m_Lon , GroupeAligneCur.m_Vect[ipb]->m_Lat) ) ;

            CNuage2Droite Nuage ;
            CDroite2D Droite = Nuage.ApproxDroite( GroupeAligneCur.m_Vect ) ;
            // calcul de la distance a la droite a chaque point
            for ( long ipts = 0 ; ipts < NbPts ; ipts++ )
                {
                // vecteur perpendiculaire à l'avant dernier point rajouté

                CPoint2D PtProj = Droite.GetProjectionDuPoint( Vectxpts[ipts] ) ;
                CVecteur2D VecPerendiculaire( PtProj , Vectxpts[ipts] ) ;

                // destruction de points si distance a la droite sufisante
                if ( DistanceMetresEcartDroite > 0 && VecPerendiculaire.GetNorm() * MilesParDegres * UnMileEnMetres > DistanceMetresEcartDroite )
                    {
                    Break = true ;
                    break ;
                    }
                }
            if ( Break )
                break ;
            }

        // recopie vecteur
        //CoefBravaisPearsonOld = CoefBravaisPearsonCur ;
        GroupeAligneOld = GroupeAligneCur ;
        }

    // memorisation groupe
    NbPts = GroupeAligneOld.m_Vect.size() ;
    /*if ( NbPts <= debpts )
        {
        NbPts = debpts  ;
        //NbPts = 2 ;
        continue ;
        }*/

    if ( GroupeAligneOld.m_Vect.size() > debpts )
        VecGroupAligne.push_back( GroupeAligneOld ) ;
    }

std::sort(VecGroupAligne.begin(),VecGroupAligne.end()) ;

/*if ( VecGroupAligne.size() )
    for ( long ip = 0 ; ip < (long) m_pVecOrigine->size() ; ip++ )
        {
        st_coord_poly* StPoly = (*m_pVecOrigine)[ip] ;
        StPoly->m_PtADetuire = true ;
        StPoly->m_PtAppartientAUnGroup = false ;
        }*/

// on ne garde que le premier et le dernier
//for ( size_t ig = 0 ; ig < VecGroupAligne.size() ; ig++ )
for ( long ig = VecGroupAligne.size() - 1 ; ig > 0 ; ig-- )
    {
    std::vector<st_coord_poly*> & VecAligne = VecGroupAligne[ig].m_Vect ;

    // tri vecteur par longitude
    CSortPts::Sort( VecAligne ) ;

    bool JamaisTraite = true ;

    for ( size_t ip = 0 ; ip < VecAligne.size() ; ip++ )
        if ( VecAligne[ip]->m_PtAppartientAUnGroup == true )
            {
            JamaisTraite = false ;
            break ;
            }

    if ( JamaisTraite  )
        {
        //bool cont = false ;
        // si les bouts sont deja detuit
        //for ( size_t ip = 0 ; ip < VecAligne.size() ; ip++ )
        //if ( VecAligne[0]->m_PtADetuire || VecAligne[VecAligne.size()-1]->m_PtADetuire )
        //    continue ;
        //    if ( VecAligne[ip]->m_PtADetuire )
        //        cont = true ;

        //if ( cont )
        //    continue ;
        for ( size_t ip = 1 ; ip < VecAligne.size()-1 ; ip++ )
            VecAligne[ip]->m_PtADetuire = true ;
        for ( size_t ip = 0 ; ip < VecAligne.size() ; ip++ )
            VecAligne[ip]->m_PtAppartientAUnGroup = true ;

        VecAligne[0]->m_PtADetuire = false ;
        VecAligne[VecAligne.size()-1]->m_PtADetuire = false ;
        }
    }

// reduction vecteur de sortie
for ( size_t ip = 0 ; ip < m_pVecOrigine->size() ; ip++ )
    {
    if ( ! (*m_pVecOrigine)[ip]->m_PtADetuire )
        VecReduce.push_back( (*m_pVecOrigine)[ip] ) ;
    }
// recopie du vecteur reduit
*m_pVecOrigine = VecReduce ;
}
