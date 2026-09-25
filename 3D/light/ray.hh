#ifndef RAY_HH
#define RAY_HH

#include "../utils/vector.hh"
#include "../utils/point.hh"

namespace isim {

    // P(t) = origin + t * direction
    class Ray {
    public:
        Point3 origin;
        Vector3 direction;

        Ray(const Point3& origin, const Vector3& direction)
            : origin(origin), direction(direction.normalized())
        {}

        // return point at distance t with the ray
        Point3 at(float t) const;
    };

}

#endif /* RAY_HH */