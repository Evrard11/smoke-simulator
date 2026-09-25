#include "sphere.hh"

namespace isim {

    // ||ray.origin + t*ray.dir - center||^2 = radius^2
    std::optional<float> Sphere::intersect(const Ray& ray) const {
        Vector3 oc = ray.origin - center;

        float a = ray.direction.dot(ray.direction);
        float b = 2.f * oc.dot(ray.direction);
        float c = oc.dot(oc) - radius * radius;

        float discriminant = b*b - 4*a*c;

        if (discriminant < 0.f)
            return std::nullopt; // without intersection

        float t1 = (-b - std::sqrt(discriminant)) / (2.f * a);
        float t2 = (-b + std::sqrt(discriminant)) / (2.f * a);

        // t must be positive, we want the intersection in front of the ray
        if (t1 > 0.f) return t1;
        if (t2 > 0.f) return t2;
        return std::nullopt;
    }

    Vector3 Sphere::normal_at(const Point3& point) const {
        return (point - center).normalized(); // Point - Point -> Vector
    }

}
