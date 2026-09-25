#include "vector.hh"

namespace isim {

    Vector3::~Vector3() = default;

    Vector3 Vector3::operator+(const Vector3& v) const { return {x + v.x, y + v.y, z + v.z}; }
    Vector3 Vector3::operator-(const Vector3& v) const { return {x - v.x, y - v.y, z - v.z}; }
    Vector3 Vector3::operator-() const { return {-x, -y, -z}; }
    Vector3 Vector3::operator/(const Vector3& v) const { return {x / v.x, y / v.y, z / v.z}; };

    // Operators scalar multiplication and division
    Vector3 Vector3::operator*(float t) const { return {x * t, y * t, z * t}; }
    Vector3 Vector3::operator/(float t) const { return *this * (1.f / t); }

    // Dot and cross products
    float Vector3::dot(const Vector3& v) const { return x * v.x + y * v.y + z * v.z; }
    Vector3 Vector3::cross(const Vector3& v) const {
        return {
            y * v.z - z * v.y,
            z * v.x - x * v.z,
            x * v.y - y * v.x
        };
    }

    // Norms
    float Vector3::norm() const { return std::sqrt(dot(*this)); }
    float Vector3::normSq() const { return dot(*this); }
    Vector3 Vector3::normalized() const { return *this / norm(); }

    // Stream operator for printing and debugging
    std::ostream& operator<<(std::ostream& os, const Vector3& v) {
        return os << "(" << v.x << ", " << v.y << ", " << v.z << ")";

    }

    Vector4::~Vector4() = default;

}