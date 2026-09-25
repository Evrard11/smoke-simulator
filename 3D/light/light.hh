#ifndef LIGHT_HH
#define LIGHT_HH

#include "../utils/point.hh"
#include "../utils/color.hh"

namespace isim {

    class Light {
    public:
        Light(const Color& intensity) : intensity(intensity) {}
        virtual ~Light() = default;

        // direction from the light to a point
        virtual Vector3 direction_from(const Point3& point) const = 0;

        // Intensity at a given point
        virtual Color intensity_at(const Point3& point) const = 0;

        // distance from the light to a point
        virtual float distance_from(const Point3& point) const = 0;

    protected:
        Color intensity;
    };

}

#endif /* LIGHT_HH */