//////////////////////////////////////////////////////////////////////////////
/// \file CLGString_inl.h
/// \date 23/03/2002 : date de creation
///
/// \brief Ce fichier implemente CLGString.
///
/// \date 19/02/2016 : Implementation de MakeReverse.
/// \date 16/02/2017 : Correction d'une fuite memoire?
/// \date 28/12/2022 : Derniere modification.
///

//////////////////////////////////////////////////////////////////////////////
/// \brief retourne la taille de la chaine.
inline int SafeStrlen(const char* psz)
    { return (psz == NULL) ? 0 : strlen(psz); }

// CLGString fonctions inline
inline int CLGString::Strlen()          ///< permet de reajuster GetLength() apres un strtok par example (rajout du 28/12/2022)
    { int length=strlen(m_pchData); if (length<m_nDataLength) m_nDataLength=length; return m_nDataLength;}
inline int CLGString::GetLength() const /// \brief Renvoie la longueur de la chaine sans le zero de la fin. bug si on utilise pas SetAt pour mettre un 0 (ex:strtok)
    { return m_nDataLength; }
inline int CLGString::IsEmpty() const
    { return m_nDataLength == 0; }
inline CLGString::operator const char*() const
    { return (const char*)m_pchData; }
//inline CLGString::operator char*()
//    { return m_pchData; }
inline int CLGString::Compare(const char* psz) const        ///< comparaison exacte
    { return strcmp(m_pchData, psz); }
inline int CLGString::CompareNoCase(const char* psz) const  /// \brief Comparaison independante de la casse.
    { return stricmp(m_pchData, psz); }
inline int CLGString::Collate(const char* psz) const        /// \brief comparaison utilisant les locales (C ou POSIX)
    { return strcoll(m_pchData, psz); }
inline void CLGString::MakeUpper()
    { strupr(m_pchData); }
inline void CLGString::MakeLower()
    { strlwr(m_pchData); }
inline void CLGString::MakeReverse()
    { strrev(m_pchData); }
inline const char & CLGString::GetAt(int nIndex) const
    {
    ASSERT(nIndex >= 0);
    ASSERT(nIndex < m_nDataLength);
    return m_pchData[nIndex];
    }
inline const char & CLGString::operator[](int nIndex) const /// \brief comme GetAt()
    {
    ASSERT(nIndex >= 0);
    ASSERT(nIndex < m_nDataLength);
    return m_pchData[nIndex];
    }
inline void CLGString::SetAt(int nIndex, char ch)
    {
    ASSERT(nIndex >= 0);
    ASSERT(nIndex < m_nDataLength);
    m_pchData[nIndex] = ch;
    // si caractere de fin de chaine
    if ( ch == 0 )
        m_nDataLength = strlen(m_pchData);
    }
inline int operator==(const CLGString& s1, const CLGString& s2)
    { return s1.Compare(s2) == 0; }
inline int operator==(const CLGString& s1, const char* s2)
    { return s1.Compare(s2) == 0; }
inline int operator==(const char* s1, const CLGString& s2)
    { return s2.Compare(s1) == 0; }
inline int operator!=(const CLGString& s1, const CLGString& s2)
    { return s1.Compare(s2) != 0; }
inline int operator!=(const CLGString& s1, const char* s2)
    { return s1.Compare(s2) != 0; }
inline int operator!=(const char* s1, const CLGString& s2)
    { return s2.Compare(s1) != 0; }
inline int operator<(const CLGString& s1, const CLGString& s2)
    { return s1.Compare(s2) < 0; }
inline int operator<(const CLGString& s1, const char* s2)
    { return s1.Compare(s2) < 0; }
inline int operator<(const char* s1, const CLGString& s2)
    { return s2.Compare(s1) > 0; }
inline int operator>(const CLGString& s1, const CLGString& s2)
    { return s1.Compare(s2) > 0; }
inline int operator>(const CLGString& s1, const char* s2)
    { return s1.Compare(s2) > 0; }
inline int operator>(const char* s1, const CLGString& s2)
    { return s2.Compare(s1) < 0; }
inline int operator<=(const CLGString& s1, const CLGString& s2)
    { return s1.Compare(s2) <= 0; }
inline int operator<=(const CLGString& s1, const char* s2)
    { return s1.Compare(s2) <= 0; }
inline int operator<=(const char* s1, const CLGString& s2)
    { return s2.Compare(s1) >= 0; }
inline int operator>=(const CLGString& s1, const CLGString& s2)
    { return s1.Compare(s2) >= 0; }
inline int operator>=(const CLGString& s1, const char* s2)
    { return s1.Compare(s2) >= 0; }
inline int operator>=(const char* s1, const CLGString& s2)
    { return s2.Compare(s1) <= 0; }
