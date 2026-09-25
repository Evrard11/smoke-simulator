#ifndef COLOR_HH
#define COLOR_HH

#include <ostream>
#include <algorithm>
#include <cmath>

namespace isim {

    class Color {
    public:
        float r, g, b;

        Color() : r(0.f), g(0.f), b(0.f) {}
        Color(float r, float g, float b) : r(r), g(g), b(b) {}

        Color operator+(float t) const { return {r + t, g + t, b + t};}
        Color operator+(const Color& c) const { return {r + c.r, g + c.g, b + c.b};}
        Color operator*(float t) const { return {r * t, g * t, b * t};}
        Color operator*(const Color& c) const { return {r * c.r, g * c.g, b * c.b};}

        Color& operator+=(const Color& c) { r += c.r; g += c.g; b += c.b; return *this;}
        Color& operator*=(const Color& c) { r *= c.r; g *= c.g; b *= c.b; return *this;}
        Color& operator+=(const float c) { r += c; g += c; b += c; return *this;}
        bool equals(const float c, float eps=1e-4) { return fabs(r - c) < eps && fabs(g - c) < eps && fabs(b - c) < eps; };

        Color clamped() const {
            auto clamp = [](float v) { return std::min(1.f, std::max(0.f, v)); };
            return { clamp(r), clamp(g), clamp(b) };
        }

        friend std::ostream& operator<<(std::ostream& os, const Color& c) {
            return os << "(" << c.r << ", " << c.g << ", " << c.b << ")";
        }
    };

}

#endif /* COLOR_HH */
