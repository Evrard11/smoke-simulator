#include <cmath>
#include <filesystem>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include "scene.hh"
#include "../objects/sphere.hh"
#include "../objects/plane.hh"
#include "../objects/triangle.hh"
#include "../objects/box.hh"
#include "../light/point_light.hh"
#include "../camera/camera.hh"
#include "../utils/point.hh"
#include "../utils/vector.hh"
#include "../utils/image.hh"
#include "../utils/color.hh"
#include "../utils/texture.hh"
#include "engine.hh"
#include "../smoke/fluidGrid.h"

const std::string OUT_DIR = "outputs/3D";

int basic() {
     using namespace isim;

    float width = 800.f;
    float height = 600.f;

    float fov_h = 90.f * M_PI / 180.f;
    float aspect = width / height;
    float fov_v = 2.f * std::atan(std::tan(fov_h / 2.f) / aspect);

    Camera cam(
        Point3(0.f, 0.f, -1.f),
        Point3(0.f, 0.f, 0.f),
        Vector3(0.f, 1.f, 0.f),
        fov_h, fov_v, 1.f
    );

    auto red_texture = std::make_shared<UniformTexture>(
        MaterialParams{ Color(0.8f, 0.1f, 0.1f), Color(1.f, 1.f, 1.f), 128.f, 0.3f }
    );
    auto blue_texture = std::make_shared<UniformTexture>(
        MaterialParams{ Color(0.1f, 0.1f, 0.8f), Color(1.f, 1.f, 1.f), 32.f, 0.2f }
    );
    auto green_texture = std::make_shared<UniformTexture>(
        MaterialParams{ Color(0.1f, 0.3f, 0.1f), Color(1.f, 1.f, 1.f), 32.f, 0.2f }
    );
    auto floor_texture = std::make_shared<UniformTexture>(
        MaterialParams{ Color(0.7f, 0.7f, 0.7f), Color(0.3f, 0.3f, 0.3f), 8.f, 0.1f }
    );

    // auto img_texture = std::make_shared<ImageTexture>(
    //     img_for_texture,
    //     MaterialParams{ Color(0.7f, 0.7f, 0.7f), Color(0.3f, 0.3f, 0.3f), 8.f, 0.1f }
    // );

    // MaterialParams base;
    // base.ks = Color(1,1,1);
    // base.ns = 32;
    // base.kr = 0.0f;
    // ImageTexture img_texture(img_for_texture, base);

    Sphere sphere_red(Point3(0.f, 0.f, -6.f), 1.f, red_texture);
    Sphere sphere_blue(Point3(2.5f, 0.f, -6.f), 1.f, blue_texture);
    Sphere sphere_green(Point3(-2.5f, 0.f, -6.f), 1.f, green_texture);
    // Plane floor(Point3(0.f, -1.5f, 0.f), Vector3(0.f, 1.f, 0.f), floor_texture);
    Plane floor(Point3(0.f, -1.5f, 0.f), Vector3(0.f, 1.f, 0.f), floor_texture);

    // test triangle 
    auto yellow_texture = std::make_shared<UniformTexture>(
    MaterialParams{ Color(0.9f, 0.8f, 0.1f), Color(1.f, 1.f, 1.f), 64.f, 0.1f }
        );
    Triangle triangle(
        Point3(-1.f, 1.5f, -5.f),
        Point3( 1.f, 1.5f, -5.f),
        Point3( 0.f, 3.f,  -5.f),
        yellow_texture
    );

    PointLight light1(Point3(-3.f, 4.f, -2.f), Color(1.f, 1.f, 1.f));
    PointLight light2(Point3(3.f, 4.f, -2.f), Color(1.f, 1.f, 1.f));

    Scene scene(
        cam,
        { &sphere_red, &sphere_blue, &sphere_green, &floor, &triangle},
        { &light1, &light2 }
    );

    Image img(width, height);
    auto pixels = raytrace(scene, img);

    for (int y = 0; y < img.get_height(); ++y)
        for (int x = 0; x < img.get_width(); ++x)
            img.at(x, y) = pixels[y * img.get_width() + x];

    img.save_ppm(OUT_DIR + "/output_basic.ppm");
    std::cout << "Saved " << OUT_DIR << "/output_basic.ppm" << std::endl;
    return 0;
}


int style() {
    using namespace isim;

    float width = 800.f;
    float height = 600.f;

    float fov_h = 90.f * M_PI / 180.f;
    float aspect = width / height;
    float fov_v = 2.f * std::atan(std::tan(fov_h / 2.f) / aspect);

    Camera cam(
        Point3(0.f, 0.f, -1.f),
        Point3(0.f, 0.2f, 0.2f),
        Vector3(0.f, 1.f, 0.f),
        fov_h, fov_v, 1.f
    );

    auto dark_sphere_texture = std::make_shared<UniformTexture>(
        MaterialParams{
            Color(0.05f, 0.05f, 0.05f),Color(0.1f, 0.1f, 0.1f),8.f,0.1f
        }
    );

    auto floor_texture = std::make_shared<UniformTexture>(
        MaterialParams{
            Color(0.05f, 0.05f, 0.05f),
            Color(0.3f, 0.3f, 0.3f),
            32.f,
            0.7f
        }
    );

    Sphere sphere(Point3(0.f, -0.3f, -6.f), 1.f, dark_sphere_texture);

    Plane floor(
        Point3(0.f, -1.5f, 0.f),
        Vector3(0.f, 1.f, 0.f),
        floor_texture
    );

    PointLight light_red(Point3(-10.f, 2.f, 10.f), Color(5.f, 0.1f, 0.1f));
    PointLight light_blue(Point3(10.f, 2.f, -10.f), Color(0.1f, 0.1f, 5.f));

    Scene scene(
        cam,
        { &sphere, &floor },
        { &light_red, &light_blue }
    );

    Image img(width, height);
    auto pixels = raytrace(scene, img);

    for (int y = 0; y < img.get_height(); ++y)
        for (int x = 0; x < img.get_width(); ++x)
            img.at(x, y) = pixels[y * img.get_width() + x];

    img.save_ppm(OUT_DIR + "/output_style.ppm");
    std::cout << "Saved " << OUT_DIR << "/output_style.ppm" << std::endl;
    return 0;
}


#include "smoke_main.h"
int screen_scene_elie(int MAX_FRAME)
{
    // smoke
    FluidGrid3D smoke = smoke_main();
    unique_ptr<FluidGrid3D> smoke_ptr = make_unique<FluidGrid3D>(smoke);

    using namespace isim;
    float width = 800.f;
    float height = 600.f;
    float fov_h = 90.f * M_PI / 180.f;
    float aspect = width / height;
    float fov_v = 2.f * std::atan(std::tan(fov_h / 2.f) / aspect);
    Point3 box_center(0.f, 0.f, 0.f);
    Camera cam(
        box_center,
        {0, 0, 0},
        Vector3(0.f, 1.f, 0.f),
        fov_h, fov_v, 1.f
    );
    // rotation with polar coord
    float radius = 6.f;
    float theta = 0.5f; // change
    float phi = 0.8f; // change
    cam.setPolarCoords(radius, theta, phi);
    // auto screen_texture = std::make_shared<UniformTexture>(
    //     MaterialParams{ Color(0.9f, 0.9f, 0.9f), Color(1.f, 1.f, 1.f), 64.f, 0.0f }
    // );
    auto smoke_texture = std::make_shared<ImageTexture>(smoke_ptr.get(),
        MaterialParams{ Color(0.9f, 0.9f, 0.9f), Color(1.f, 1.f, 1.f), 64.f, 0.0f }
    );
    Box screen(
        Point3(-2.5f, -2.5f, -2.5f),
        Point3( 2.5f,  2.5f, 2.5f),
        smoke_texture
    );
    auto floor_texture = std::make_shared<UniformTexture>(
        MaterialParams{ Color(0.7f, 0.7f, 0.7f), Color(0.3f, 0.3f, 0.3f), 8.f, 0.1f }
    );
    Plane floor(Point3(0.f, -1.5f, 0.f), Vector3(0.f, 1.f, 0.f), floor_texture);
    PointLight light1(Point3(-3.f, 4.f, -2.f), Color(1.f, 1.f, 1.f));
    auto red_texture = std::make_shared<UniformTexture>(
        MaterialParams{ Color(0.8f, 0.8f, 0.1f), Color(1.f, 1.f, 1.f), 128.f, 0.1f }
    );
    Sphere sphere_yellow(Point3(0.f, 0.f, 3.f), 0.25f, red_texture);
    Scene scene(
        cam,
        { &screen, /*&floor,*/ &sphere_yellow },
        { &light1 }
    );
    Image img(width, height);

    // smoke_step(*smoke_ptr);
    for (int frame = 0; frame < MAX_FRAME; frame++) {
        // theta += 0.05f;
        //scene.cam.setPolarCoords(radius, theta, phi);
        dynamic_cast<Sphere*>(scene.objects[1])->center.z -= 0.1f;
        smoke_step(*smoke_ptr);
        auto pixels = raytrace(scene, img);
        for (int y = 0; y < img.get_height(); ++y)
            for (int x = 0; x < img.get_width(); ++x)
                img.at(x, y) = pixels[y * img.get_width() + x];
        std::ostringstream path;
        path << OUT_DIR << "/frame_" << std::setw(4) << std::setfill('0') << frame << ".ppm";
        img.save_ppm(path.str());
        std::cout << "frame " << frame + 1 << "/" << MAX_FRAME << " -> " << path.str() << std::endl;
    }
    std::cout << "Done. Make a video with:\n"
              << "  ffmpeg -framerate 24 -i " << OUT_DIR << "/frame_%04d.ppm"
              << " -c:v libx264 -crf 18 -pix_fmt yuv420p " << OUT_DIR << "/smoke3D.mp4" << std::endl;
    return 0;
}

int screen_scene_evrard() {
    using namespace isim;

    float width = 800.f;
    float height = 600.f;

    float fov_h = 90.f * M_PI / 180.f;
    float aspect = width / height;
    float fov_v = 2.f * std::atan(std::tan(fov_h / 2.f) / aspect);

    // Camera cam(
    //     Point3(0.f, 0.f, -1.f),
    //     Point3(0.f, 0.f, 0.f),
    //     Vector3(0.f, 1.f, 0.f),
    //     fov_h, fov_v, 1.f
    // );

    // rotation with polar coord
    float theta = 0.5f; // change for rotate

    Point3 center(0.f, 0.f, -13.5f);
    float radius = 10.f;

    Point3 cam_pos(
        radius * std::sin(theta),
        0.f,
        center.z + radius * std::cos(theta)
    );

    Camera cam(
        cam_pos,
        center,
        Vector3(0.f, 1.f, 0.f),
        fov_h, fov_v, 1.f
    );

    auto floor_texture = std::make_shared<UniformTexture>(
        MaterialParams{ Color(0.7f, 0.7f, 0.7f), Color(0.3f, 0.3f, 0.3f), 8.f, 0.1f }
    );

    Box screen(
        Point3(-0.5f, -1.f, -6.1f),
        Point3( 0.5f,  1.f, -20.9f),
        floor_texture
    );

    Plane floor(Point3(0.f, -1.5f, 0.f), Vector3(0.f, 1.f, 0.f), floor_texture);

    PointLight light1(Point3(-3.f, 4.f, -2.f), Color(1.f, 1.f, 1.f));
    Scene scene(
        cam,
        { &screen, &floor},
        { &light1 }
    );

    Image img(width, height);
    auto pixels = raytrace(scene, img);

    for (int y = 0; y < img.get_height(); ++y)
        for (int x = 0; x < img.get_width(); ++x)
            img.at(x, y) = pixels[y * img.get_width() + x];

    img.save_ppm(OUT_DIR + "/output_screen.ppm");
    std::cout << "Saved " << OUT_DIR << "/output_screen.ppm" << std::endl;
    return 0;
}

int main(int argc, char* argv[]) {
    // usage: smoke-simulator-3D [smoke|basic|style] [--frames N]
    std::string scene = "smoke";
    int frames = 100;
    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];
        if (arg == "--frames" && i + 1 < argc) frames = std::stoi(argv[++i]);
        else if (arg == "smoke" || arg == "basic" || arg == "style") scene = arg;
        else {
            std::cerr << "Usage: " << argv[0] << " [smoke|basic|style] [--frames N]" << std::endl;
            return 1;
        }
    }

    std::filesystem::create_directories(OUT_DIR);
    if (scene == "basic") basic();
    else if (scene == "style") style();
    else screen_scene_elie(frames);
    return 0;
}
