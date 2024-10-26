////////////////////////////////////////////////////////////////////////////////
/// \file Segments.h
///
/// \brief Entete pour segments secants
///
/// \date creation     : 13/08/2024
/// \date modification : 13/08/2024
///

#ifndef _SEG_SEC_
#define _SEG_SEC_

struct Point
{
float x;
float y;
};

bool doIntersect(Point p1, Point q1, Point p2, Point q2) ;

#endif
