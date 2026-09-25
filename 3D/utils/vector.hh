#ifndef VECTOR_HH
#define	VECTOR_HH

#include <cstdint>
#include <cstddef>
#include <cmath>
#include <ostream>

namespace isim {
    class Vector3 {

        public:
            float x;
            float y;
            float z;

            Vector3(float x, float y, float z) : x(x), y(y), z(z) {}
            ~Vector3();

            // Operators addition, substraction, negation
            Vector3 operator+(const Vector3& v) const;
            Vector3 operator-(const Vector3& v) const;
            Vector3 operator-() const;
            Vector3 operator/(const Vector3& v) const;

            // Operators scalar multiplication and division
            Vector3 operator*(float t) const;
            Vector3 operator/(float t) const;

            // Dot and cross products
            float dot(const Vector3& v) const;
            Vector3 cross(const Vector3& v) const;

            // Norms
            float norm() const;
            float normSq() const;
            Vector3 normalized() const;

            // Stream operator for printing and debugging
            friend std::ostream& operator<<(std::ostream& os, const Vector3& v);
        };

    class Vector4 {

        public:
            float x;
            float y;
            float z;
            float w;

            Vector4(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) {}
            ~Vector4();


            Vector4 operator+(const Vector4& v) const { return {x + v.x, y + v.y, z + v.z, w + v.w}; }
            Vector4 operator-(const Vector4& v) const { return {x - v.x, y - v.y, z - v.z, w - v.w}; }
            Vector4 operator-() const { return {-x, -y, -z, -w}; }

            Vector4 operator*(float t) const { return {x * t, y * t, z * t, w * t}; }
            Vector4 operator/(float t) const { return *this * (1.f / t); }

    };
}

#endif	/* VECTOR_HH */