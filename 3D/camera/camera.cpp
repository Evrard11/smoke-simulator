#include "camera.hh"

namespace isim {
    Ray Camera::cast_ray(int x, int y, int W, int H, float dx, float dy) const {
        Vector3 forward = (P - C).normalized();
        Vector3 right = forward.cross(up).normalized();
        Vector3 up_cam = right.cross(forward);

        float half_w = std::tan(alpha / 2.f) * zmin;
        float half_h = std::tan(gamma / 2.f) * zmin;

        float u = (x + 0.5f + dx) / W - 0.5f;
        float v = 0.5f - (y + 0.5f + dy) / H;

        Vector3 dir = (forward * zmin + right * (u * 2.f * half_w) + up_cam * (v * 2.f * half_h)).normalized();
        return Ray(C, dir);
    }

    void Camera::setPolarCoords(float radius, float theta, float phi) {
        C = Point3(
            P.x + radius * std::cos(phi) * std::sin(theta),
            P.y + radius * std::sin(phi),
            P.z + radius * std::cos(phi) * std::cos(theta)
        );
        Vector3 forward = (P - C).normalized();
        Vector3 world_up(0.f, 1.f, 0.f);
        Vector3 right = world_up.cross(forward).normalized();
        up = forward.cross(right).normalized();
    }
}
