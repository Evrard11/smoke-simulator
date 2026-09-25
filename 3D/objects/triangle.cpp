#include "triangle.hh"

namespace isim {

    std::optional<float> Triangle::intersect(const Ray& ray) const {
        // usedful vectors
        Vector3 AB = B - A;
        Vector3 AC = C - A;
        Vector3 AO = ray.origin - A;

        Vector3 h = ray.direction.cross(AC);
        float det = AB.dot(h);

        float inv_det = 1.f / det;

        // barycentrique coordinate u
        float u = AO.dot(h) * inv_det;
        if (u < 0.f || u > 1.f)
            return std::nullopt;

        // barycentrique coordinate v
        Vector3 q = AO.cross(AB);
        float v = ray.direction.dot(q) * inv_det;
        if (v < 0.f || u + v > 1.f)
            return std::nullopt;

        // Distance t
        float t = AC.dot(q) * inv_det;
        if (t > 1e-4f)
            return t;

        return std::nullopt;
    }

    Vector3 Triangle::normal_at(const Point3& point) const {
        Vector3 AB = B - A;
        Vector3 AC = C - A;
        Vector3 normal = AB.cross(AC).normalized();
        return normal;
    }

}