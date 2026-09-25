#include "ray.hh"

namespace isim {
    Point3 Ray::at(float t) const {
            return origin + direction * t;
        }
}