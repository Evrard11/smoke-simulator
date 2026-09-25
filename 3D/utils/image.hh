#ifndef IMAGE_HH
#define IMAGE_HH

#include <vector>
#include <string>
#include <fstream>
#include <stdexcept>
#include "color.hh"

namespace isim {

    class Image {
    public:
        Image(int width, int height)
            : width(width), height(height), pixels(width * height)
        {}
        Image(const std::string& filename);
        ~Image() = default;

        // Getters
        int get_width()  const;
        int get_height() const;

        // Get pixel at (x, y)
        Color& at(int x, int y);
        const Color& at(int x, int y) const;

        // Set pixel at (x, y)
        void set(int x, int y, const Color& c);

        // Save image as PPM file
        void save_ppm(const std::string& path) const;

    private:
        int width;
        int height;
        std::vector<Color> pixels;
    };

}

#endif /* IMAGE_HH */