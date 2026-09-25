#include "point.hh"
#include "vector.hh"

namespace isim {

    Point3 Point3::operator+(const Vector3& v) const { return {x + v.x, y + v.y, z + v.z}; }
    Point3 Point3::operator-(const Vector3& v) const { return {x - v.x, y - v.y, z - v.z}; }

    // Useful for sphere.cpp and intersect
    Vector3 Point3::operator-(const Point3& p) const { 
        return {x - p.x, y - p.y, z - p.z}; }

    std::ostream& operator<<(std::ostream& os, const Point3& p) {
    return os << "(" << p.x << ", " << p.y << ", " << p.z << ")";
}

}