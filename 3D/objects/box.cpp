#include "box.hh"
#include <algorithm>
#include <cmath>

namespace isim {

Box::Box(const Point3& min,
         const Point3& max,
         std::shared_ptr<TextureMaterial> texture)
    : Object(std::move(texture)), min(min), max(max) {}

std::optional<float> Box::intersect(const Ray& ray) const {
    const std::optional<std::array<float, 2>> intersection = intersect_box(ray);
    if (intersection) {
        return intersection.value()[0];
    }
    return std::nullopt;
}

std::optional<std::array<float, 2>> Box::intersect_box(const Ray& ray) const {
    const float epsilon = 1e-8;

    float tmin = -INFINITY;
    float tmax = INFINITY;

    // X axis
    if (std::abs(ray.direction.x) < epsilon) {
        if (ray.origin.x < min.x || ray.origin.x > max.x)
            return std::nullopt;
    } else {
        float invD = 1.f / ray.direction.x;
        float t1 = (min.x - ray.origin.x) * invD;
        float t2 = (max.x - ray.origin.x) * invD;

        if (t1 > t2) std::swap(t1, t2);

        tmin = std::max(tmin, t1);
        tmax = std::min(tmax, t2);

        if (tmin > tmax)
            return std::nullopt;
    }

    // Y axis
    if (std::abs(ray.direction.y) < epsilon) {
        if (ray.origin.y < min.y || ray.origin.y > max.y)
            return std::nullopt;
    } else {
        float invD = 1.f / ray.direction.y;
        float t1 = (min.y - ray.origin.y) * invD;
        float t2 = (max.y - ray.origin.y) * invD;

        if (t1 > t2) std::swap(t1, t2);

        tmin = std::max(tmin, t1);
        tmax = std::min(tmax, t2);

        if (tmin > tmax)
            return std::nullopt;
    }

    // Z axis
    if (std::abs(ray.direction.z) < epsilon) {
        if (ray.origin.z < min.z || ray.origin.z > max.z)
            return std::nullopt;
    } else {
        float invD = 1.f / ray.direction.z;
        float t1 = (min.z - ray.origin.z) * invD;
        float t2 = (max.z - ray.origin.z) * invD;

        if (t1 > t2) std::swap(t1, t2);

        tmin = std::max(tmin, t1);
        tmax = std::min(tmax, t2);

        if (tmin > tmax)
            return std::nullopt;
    }

    if (tmin > 0.f && tmax > 0.f) return std::array{ tmin, tmax };

    return std::nullopt;
}

Vector3 Box::normal_at(const Point3& point) const {
    const float epsilon = 1e-4;

    if (std::abs(point.x - min.x) < epsilon) return Vector3(-1, 0, 0);
    if (std::abs(point.x - max.x) < epsilon) return Vector3(1, 0, 0);

    if (std::abs(point.y - min.y) < epsilon) return Vector3(0, -1, 0);
    if (std::abs(point.y - max.y) < epsilon) return Vector3(0, 1, 0);

    if (std::abs(point.z - min.z) < epsilon) return Vector3(0, 0, -1);
    if (std::abs(point.z - max.z) < epsilon) return Vector3(0, 0, 1);

    // fallback
    return Vector3(0, 0, 0);
}

}