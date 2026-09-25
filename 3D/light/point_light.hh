#ifndef POINT_LIGHT_HH
#define POINT_LIGHT_HH

#include "light.hh"

namespace isim {

    class PointLight : public Light {
    public:
        PointLight(const Point3& position, const Color& intensity)
            : Light(intensity), position(position)
        {}

        Vector3 direction_from(const Point3& point) const override;
        Color intensity_at(const Point3& point) const override;
        float distance_from(const Point3& point) const override;

    private:
        Point3 position;
    };

}

#endif /* POINT_LIGHT_HH */