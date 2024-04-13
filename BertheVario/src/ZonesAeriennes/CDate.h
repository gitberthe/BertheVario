///////////////////////////////////////////////////////////////////////////////
/// \file CDate.h
/// \date 19/07/2013 : date de creation.
///
/// \brief Declaration CDate.
///
/// \date 11/11/2021 : passage de julien vers gregorien
/// \date 25/03/2024 : dernierre modification
///

#ifndef _CMJDATE_
#define _CMJDATE_

#define TMJ_ORIGINE_1900 (2415020.) ///< Origine le 00/01/1900 12h00.
#define TMJ_ORIGINE_2000 (2451545.) ///< Origine le 1 Janvier 2000 12h TU note J2000.0 . Origine an 2000
#define TMJ_SIECLE_JD    (36525.)   ///< Nombre de jours dans un siecle julien.

//////////////////////////////////////////////////////////////////////
/// \brief Permet de calculer le jour julien TU et le temps T en siecles
/// julien. Avec comme origine le 00/01/1900 12h00 (JD 2415020) ou J2000
/// (Formulae p23).
class CDate
{
public:
    CDate(float JulianDayTU=TMJ_ORIGINE_1900);
    CDate(long annee, long mois, long jour, float heure_tu)
            { SetDateTU(annee,mois,jour,heure_tu) ; } ;

    void    GetDateTU( long & annee, long & mois, long & jour, float & heure_tu ) const ;
    void    SetDateTU( long annee, long mois, long jour, float heure_tu ) ;
    void    JulienVersGregorien() ;

    float GetT_1900_UT() const;
    float GetT_J2000() const;
    float GetJD_TU() const
            { return m_JD_TU ; } ;
    const char * GetNomJour() const;
    bool         IsWeekEnd() const;
    bool        IsBissextile() const ;

    static long GetJJArrondiANbJour( long JJ , long NbJour ) ;

protected:

private:
    float m_JD_TU; ///< Jour julien TU avec heure decimale.
} ;

#endif
