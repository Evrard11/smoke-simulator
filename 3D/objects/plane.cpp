#include "plane.hh"

namespace isim {

    std::optional<float> Plane::intersect(const Ray& ray) const {
        float denom = ray.direction.dot(normal);

        if (std::abs(denom) < 1e-6f)
            return std::nullopt;

        float t = (point - ray.origin).dot(normal) / denom;

        if (t > 1e-4f)
            return t;
        return std::nullopt;
    }

    Vector3 Plane::normal_at(const Point3& p) const {
        return normal;
    }

}