#include "image.hh"

#include <iostream>
#include <cstdint>

namespace isim {

    int Image::get_width()  const { return width; }
    int Image::get_height() const { return height; }

    Color& Image::at(int x, int y) {
            return pixels[y * width + x];
        }

    const Color& Image::at(int x, int y) const {
        return pixels[y * width + x];
    }

    void Image::set(int x, int y, const Color &c) {
        pixels[y * width + x] = c;
    }

    void Image::save_ppm(const std::string& path) const {
        std::ofstream file(path, std::ios::binary);
        if (!file)
            throw std::runtime_error("Cannot open file: " + path);

        file << "P6\n" << width << " " << height << "\n255\n";

        for (const Color& c : pixels) {
            Color clamped = c.clamped();
            uint8_t r = static_cast<uint8_t>(clamped.r * 255.f);
            uint8_t g = static_cast<uint8_t>(clamped.g * 255.f);
            uint8_t b = static_cast<uint8_t>(clamped.b * 255.f);
            file.write(reinterpret_cast<const char*>(&r), 1);
            file.write(reinterpret_cast<const char*>(&g), 1);
            file.write(reinterpret_cast<const char*>(&b), 1);
        }
    }
    Image::Image(const std::string& filename) {
        std::ifstream file(filename, std::ios::binary);
        if (!file) throw std::runtime_error("no file found for image texture");

        std::string magic;
        file >> magic;

        int width, height, maxval;
        file >> width >> height >> maxval;
        file.get(); // skip newline

        this->width = width;
        this->height = height;
        this->pixels = std::vector<Color>(width * height);

        for (int i = 0; i < width; i++) {
            for (int j = 0; j < height; j++) {
                unsigned char rgb[3];
                file.read(reinterpret_cast<char*>(rgb), 3);

                this->pixels[i * height + j] = Color(
                    rgb[0] / 255.0f,
                    rgb[1] / 255.0f,
                    rgb[2] / 255.0f
                );
            }
        }
    }
}
