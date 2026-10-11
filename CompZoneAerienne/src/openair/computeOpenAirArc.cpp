////////////////////////////////////////////////////////////////////////////////
/// \file computeOpenAirArc.cpp
///
/// \brief
///
/// \date 11/10/2026 : creation
/// \date 11/10/2026 : modification
///

#include <iostream>
#include <cmath>
#include <vector>
#include <iomanip>

#include "computeOpenAirArc.h"

const double PI = 3.14159265358979323846;
const double EARTH_RADIUS = 6371000.0; // En mètres

// Convertit des degrés en radians
double toRadians(double degree) {
    return degree * PI / 180.0;
}

// Convertit des radians en degrés
double toDegrees(double radian) {
    return radian * 180.0 / PI;
}

// Calcule la distance de grand cercle (en radians) entre deux points
double calculateDistanceRad(const Point& p1, const Point& p2) {
    double lat1 = toRadians(p1.lat);
    double lat2 = toRadians(p2.lat);
    double dLon = toRadians(p2.lon - p1.lon);

    return acos(sin(lat1) * sin(lat2) + cos(lat1) * cos(lat2) * cos(dLon));
}

// Calcule l'azimut (bearing) du point 1 vers le point 2 (en radians, de 0 à 2*PI)
double calculateBearingRad(const Point& p1, const Point& p2) {
    double lat1 = toRadians(p1.lat);
    double lat2 = toRadians(p2.lat);
    double dLon = toRadians(p2.lon - p1.lon);

    double y = sin(dLon) * cos(lat2);
    double x = cos(lat1) * sin(lat2) - sin(lat1) * cos(lat2) * cos(dLon);
    double bearing = atan2(y, x);

    if (bearing < 0) {
        bearing += 2.0 * PI;
    }
    return bearing;
}

// Trouve un point à partir d'un centre, d'une distance (en rad) et d'un azimut (en rad)
Point destinationPoint(const Point& start, double distanceRad, double bearingRad) {
    double lat1 = toRadians(start.lat);
    double lon1 = toRadians(start.lon);

    double lat2 = asin(sin(lat1) * cos(distanceRad) + cos(lat1) * sin(distanceRad) * cos(bearingRad));
    double lon2 = lon1 + atan2(sin(bearingRad) * sin(distanceRad) * cos(lat1), cos(distanceRad) - sin(lat1) * sin(lat2));

    return { toDegrees(lat2), toDegrees(lon2) };
}

// Génère l'arc OpenAir en utilisant le centre, le départ et la fin
std::vector<Point> computeOpenAirArc(const Point& center, const Point& start, const Point& end, int numSegments, bool clockwise /* = true */) {
    std::vector<Point> arcPoints;

    // 1. Calcul du rayon de l'arc (distance du centre au point de départ)
    double radiusRad = calculateDistanceRad(center, start);

    // 2. Calcul des azimuts de départ et de fin depuis le centre
    double bearingStart = calculateBearingRad(center, start);
    double bearingEnd = calculateBearingRad(center, end);

    // 3. Calcul de l'angle total à parcourir
    double totalAngle = bearingEnd - bearingStart;

    if (clockwise) {
        if (totalAngle <= 0) totalAngle += 2.0 * PI;
    } else {
        if (totalAngle >= 0) totalAngle -= 2.0 * PI;
    }

    // 4. Échantillonnage le long de l'arc à rayon constant
    for (int i = 0; i <= numSegments; ++i) {
        double t = static_cast<double>(i) / numSegments;
        double currentBearing = bearingStart + t * totalAngle;

        // Normalisation de l'azimut entre 0 et 2*PI
        currentBearing = fmod(currentBearing, 2.0 * PI);
        if (currentBearing < 0) currentBearing += 2.0 * PI;

        // Calcul du point physique sur la sphère
        Point pt = destinationPoint(center, radiusRad, currentBearing);
        arcPoints.push_back(pt);
    }

    return arcPoints;
}
