#ifndef POINT_HH
#define	POINT_HH

#include <cstdint>
#include <cstddef>
#include <cmath>
#include <ostream>

#include "vector.hh"

namespace isim {

    class Point3 {

        public:
            float x;
            float y;
            float z;

            Point3(float x, float y, float z) : x(x), y(y), z(z) {}

            // Opertators addition and substraction with vectors
            Point3 operator+(const Vector3& v) const;
            Point3 operator-(const Vector3& v) const;
            Vector3 operator-(const Point3& p) const;
            friend std::ostream& operator<<(std::ostream& os, const Point3& p);
};

    class Point4 {

        public:
            float x;
            float y;
            float z;
            float w;

            Point4(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) {}
    };

}

#endif	/* POINT_HH */