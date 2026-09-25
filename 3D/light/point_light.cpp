#include "point_light.hh"

namespace isim {

    Vector3 PointLight::direction_from(const Point3& point) const {
        return (position - point).normalized();
    }


    Color PointLight::intensity_at(const Point3& point) const {
        return intensity;
    }

    float PointLight::distance_from(const Point3& point) const {
        return (position - point).norm();
    }

} // namespace isim