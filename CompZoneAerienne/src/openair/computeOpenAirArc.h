////////////////////////////////////////////////////////////////////////////////
/// \file computeOpenAirArc.h
///
/// \brief
///
/// \date 11/10/2026 : creation
/// \date 11/10/2026 : modification
///

#pragma once

struct Point {
    double lat; // En degrés
    double lon; // En degrés
};

std::vector<Point> computeOpenAirArc(const Point& center, const Point& start, const Point& end, int numSegments, bool clockwise = true) ;
