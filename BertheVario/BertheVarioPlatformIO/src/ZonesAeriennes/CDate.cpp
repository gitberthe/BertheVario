///////////////////////////////////////////////////////////////////////////////
/// \file CDate.cpp
/// \date 20/08/2013 : date de creation.
///
/// \brief Implementation de CDate.
///
/// \date 03/03/2016 : plus de librairie TRB.
/// \date 11/11/2022 : passage de julien vers gregorien
/// \date 25/03/2024 : derniere modification
///

#ifdef _BERTHE_VARIO_
 #include "../BertheVario.h"
#endif
#ifdef _BERTHE_VARIO_213_
 #include "../BertheVario213.h"
#endif
#ifdef _BERTHE_VARIO_TAC_
 #include "../BertheVarioTac.h"
#endif

//////////////////////////////////////////////////////////////////////
/// \brief Constructeur, initialise le jour julien.
/// \param JulianDay [in] : Jour Julien de l'instance.
CDate::CDate(float JulianDayTU/*=TMJ_ORIGINE_1900*/)
{
m_JD_TU=JulianDayTU;
}

//////////////////////////////////////////////////////////////////////
/// \brief Renvoie le temps T TU en siecles julien (origine le 00/01/1900
/// 12h00 (JD 2415020).
/// Non corrigé du ralentissement terrestre (Formulae p79).
float CDate::GetT_1900_UT() const
{
float T=m_JD_TU-TMJ_ORIGINE_1900; // temps en siecles julien
T/=TMJ_SIECLE_JD;
return T;
}

//////////////////////////////////////////////////////////////////////
/// \brief Renvoie le temps T en siecles julien (origine le 1 Janvier
/// 2000 12h TU note J2000.0)(JD 2451545). Temps utilisé pour pluton (TRB).
float CDate::GetT_J2000() const
{
float T=m_JD_TU-TMJ_ORIGINE_2000; // temps en siecles julien
T/=TMJ_SIECLE_JD;
return T;
}

//////////////////////////////////////////////////////////////////////
/// \brief positionne la date en jour julien (formulae p24)
/// reforme le 1582 10 15.
void CDate::SetDateTU( int annee, int mois, int jour, float heure_tu_0_24 )
{
/*ASSERT( mois >0 ) ;
ASSERT( mois <13 ) ;
ASSERT( jour >-1 ) ;
ASSERT( jour <33 ) ;
ASSERT( heure_tu_0_24 >= 0. ) ;
ASSERT( heure_tu_0_24 < 24. ) ;*/
int y,m;
int A=0;
int B=0;
// positionnement de variables
if (mois>2)
    {
    y=annee;
    m=mois;
    }
else
    {
    y=annee-1;
    m=mois+12;
    }

if ( y >0 )
    m_JD_TU=(int)(365.25*y) + (int)(30.6001*(m+1)) + jour + heure_tu_0_24/24. + 1720994.5 ;
else
    m_JD_TU=(int)(365.25*y-0.75) + (int)(30.6001*(m+1)) + jour + heure_tu_0_24/24. + 1720994.5 ;

// calcul de la date avant/apres reforme du 1582 10 15
float date=annee + (mois-1.)/12. + (jour-1.)/30. ;
float reforme=1582 + (10.-1.)/12. + (15.-1.)/30 ;
if ( date < reforme )
    B=0;
else
    {
    A=(int)(y/100.);
    B=2-A+(int)(A/4.);
    m_JD_TU += B ;
    }
}

//////////////////////////////////////////////////////////////////////
/// \brief donne la date en jour julien (formulae p24)
/// reforme le 1582 10 15.
void CDate::GetDateTU( int &annee, int &mois, int &jour, float &heure_tu_0_24 ) const
{
int A;
float JJ = m_JD_TU + 0.5 ;
int Z = (int) JJ ;
float F = JJ - Z ;

if ( Z < 2299161 )
    A=Z;
else
    {
    int a=(int)((Z-1867216.25)/36524.25);
    A=Z+1+a-(int)(a/4.);
    }

int B=A+1524;
int C=(int)((B-122.1)/365.25);
int D=(int)(365.25*C);
int E=(int)((B-D)/30.6001);

// jour
heure_tu_0_24=B-D-(int)(30.6001*E)+F;
jour=(int)heure_tu_0_24;
heure_tu_0_24=heure_tu_0_24-jour;
heure_tu_0_24 *=24. ;

//mois
mois = 1 ;
if ( E < 13.5 )
    mois=E-1;
if ( E > 13.5 )
    mois=E-13;

// annee
if ( mois >2.5 )
    annee=C-4716;
else
    annee=C-4715;
}

/////////////////////////////////////////////////////////////////////////////
/// \brief Donner le jour de la semaine en  fonction du jour julien. (voir TRBJulien.cpp)
///
/// \param m_JJ_float [class in] : Nombre de jours Julien.
///
/// \return Un pointeur sur le tableau de caractere contenant le nom du jour de
/// la semaine.
///
const char * CDate::GetNomJour() const
{
static const char * TabNomJours[] = {"Dimanche","Lundi","Mardi","Mercredi",
               "Jeudi","Vendredi","Samedi","Erreur"} ;

// Formule qui donne la position du jour de la semaine en prenant comme
// on doit le dimanche pour premier jour.
int l_Q = (int) (FLOOR(FMOD(m_JD_TU,7.)) + 2. ) ;

// correction pour le changement de jour a 12 H TU
float ent ;
if ( MODF(m_JD_TU, &ent) >= 0.5 ) // retourne le reste
    l_Q++ ;

// correspondance numerique pour l'ecriture des resultats.
if ( l_Q > 7. )
    l_Q -= (int) 7. ;

//ASSERT( l_Q < 8. ) ;
//ASSERT( l_Q > 0. ) ;

return TabNomJours[l_Q-1] ;
}

/////////////////////////////////////////////////////////////////////////////
/// \brief Donner si on est en week-end.
///
bool CDate::IsWeekEnd() const
{
// Formule qui donne la position du jour de la semaine en prenant comme
// on doit le dimanche pour premier jour.
int l_Q = (int) (FLOOR(FMOD(m_JD_TU,7.)) + 2. ) ;

// correction pour le changement de jour a 12 H TU
float ent ;
if ( MODF(m_JD_TU, &ent) >= 0.5 ) // retourne le reste
    l_Q++ ;

// correspondance numerique pour l'ecriture des resultats.
if ( l_Q > 7. )
    l_Q -= (int) 7. ;

if ( l_Q == 1 || l_Q == 7 )
    return true ;
return false ;
}

/////////////////////////////////////////////////////////////////////////////
/// \brief retourne si l'annee est bissextile.
bool CDate::IsBissextile() const
{
int annee,mois,jour;
float heure_tu_0_24 ;
GetDateTU( annee, mois, jour, heure_tu_0_24 ) ;

return ((annee % 4) == 0) && (((annee % 100) != 0) || ((annee % 400) == 0)) ;
}

/////////////////////////////////////////////////////////////////////////////
/// \brief Passe de julien vers gregorien (d'apres wikipedia)
void CDate::JulienVersGregorien()
{
struct stDateDecalJour
    {
    int m_Annee ;
    int m_DecalJour ;
    } ;

static stDateDecalJour DateDecalJourArr[] =
    {{   4, -2} ,
     { 100, -1} ,
     { 200,  0} ,
     { 300,  1} ,
     { 400,  1} ,
     { 500,  2} ,
     { 600,  3} ,
     { 700,  4} ,
     { 800,  4} ,
     { 900,  5} ,
     {1000,  6} ,
     {1100,  7} ,
     {1200,  7} ,
     {1300,  8} ,
     {1400,  9} ,
     {1500, 10} ,
     {1600, 10} ,
     {1700, 11} ,
     {1800, 12} ,
     {1900, 13} ,
     {2000, 13} ,
     {2100, 14} ,
     {2200, 15} ,
     {2300, 16} ,
     {2400, 16} ,
     {2500, 17} } ;

CDate DateCompar ;
for ( int id = 0 ; true ; id++ )
    {
    stDateDecalJour & DateBascule = DateDecalJourArr[id] ;
    DateCompar.SetDateTU( DateBascule.m_Annee , 3 , 1 , 0. ) ;
    if ( m_JD_TU < DateCompar.GetJD_TU() )
        {
        m_JD_TU += DateDecalJourArr[id-1].m_DecalJour ;
        break ;
        }
    }
}

/////////////////////////////////////////////////////////////////////////////
/// \brief Renvoi le JJ de date arrondi a nb jours
int CDate::GetJJArrondiANbJour( int JJ , int NbJours )
{
JJ = JJ - JJ % NbJours ;
return JJ ;
}
