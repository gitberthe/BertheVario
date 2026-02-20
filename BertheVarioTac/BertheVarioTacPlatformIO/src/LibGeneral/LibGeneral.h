//////////////////////////////////////////////////////////////////////////////
/// \file   LibGeneral.h
/// \date   15/11/2002 : date de creation
///
/// \brief Fichier include a inclure pour l'utilisation de la librairie
///
/// \date 20/12/2002 : Ajout de ListeStrData.
/// \date 25/04/2012 : Compilation __GNUWIN32__.
/// \date 18/02/2016 : Modification sans _AVEC_WXWIN_H pour snb-artmap (strlwr...).
/// \date 18/02/2016 : Ajout de CLGTObjets::GetData().
/// \date 18/02/2016 : CLGTObjets() m_nGrowBy = 50.
/// \date 19/02/2016 : Implementation de MakeReverse.
/// \date 20/11/2018 : Optimization CLGTObjets::RemoveAt().
/// \date 24/01/2019 : Ajout du tri par fusion en threads.
/// \date 19/10/2020 : NVIDIA_FUNC
/// \date 05/02/2023 : Derniere modification.
///

#ifndef _LIB_GENERAL_H_
#define _LIB_GENERAL_H_

class CLGString ;

void         ChangeCharIn( CLGString & spchar , char find , char replace , bool strlen = true ) ;
const char * FormatCharIn( CLGString & spchar , bool remplace_espace = false , bool aplha_seul = false , bool strlen = true ) ;
void         ReplaceStringIn( CLGString & sst, const char *orig, const char *repl , char * pBuff , bool strlen = true ) ;
void   echoOn(void) ;
void   echoOff(void) ;
bool   kbhit(void) ;

#ifndef MAX
    #define MAX(a,b) ((a>b) ? (a) : (b))
#endif // MAX
#ifndef MIN
    #define MIN(a,b) ((a<b) ? (a) : (b))
#endif // MIN

#ifndef SIGNE
    #define SIGNE(a) ((a>=0.) ? (1) : (-1))
#endif

#include "CLGCoGeneral.h"

extern int  static_Nb_Control_C ;
extern bool static_Control_C_Active ;
extern bool g_Verbose ;

#include "CLGString.h"
//#include "CLGLObjets.h"

#include "CLGString.h"
#include "CLGTObjets.h"
//#include "CLGLObjets.h"
 //#include "CLGThread.h"
 //#include "CLGMSMergeSort.h"
#include "CLGQuickSort.h"
 // #include "CLGQuickSortThread.h"
#include "CLGArray.h"
//#include "CLGList.h"
//#include "CLGMap.h"
 //#include "CLGThreadArr.h"
 //#include "CLGListDatapChar.h"
//#include "CLGTextFch.h"
 //#include "CLGCorrelation.h"
 //#include "CLGStatUtil.h"
 //#include "CLGCourbeLisse.h"
 //#include "CLGPgcdPpcm.h"
 //#include "CLGPermutArr.h"
 //#include "CLGPictAlgo.h"

//#ifdef _LIB_XML_
 //#include <pthread.h>
 //#include <libxml/threads.h>
 //#include <libxml/tree.h>
 //#include <libxml/parser.h>
 //#include <libxml/HTMLparser.h>
 //#include "CLGHtmlFch.h"
 //#endif

//extern stringstream g_CoutCin ;
//extern streambuf *g_CoutBuf ;

#endif
