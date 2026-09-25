# smoke-simulator
ISIM Project, image synthesis of a smoke

Authors : Evrard & Elie

## Results

### Coffee smoke
Smoke rising from a source at the bottom of the grid, then integrated into a coffee cup scene that we modeled in Blender.

<table>
  <tr>
    <td><a href="https://github.com/user-attachments/assets/a05bf665-6780-4071-8295-8b2eb1cdbe42"><img src="assets/previews/smoke_coffee_improved.jpg" alt="Coffee smoke simulation"></a></td>
    <td><a href="https://github.com/user-attachments/assets/3c34289e-406c-4764-b0ab-7b2f92aa307a"><img src="assets/previews/cafe.jpg" alt="Smoke integrated in the Blender scene"></a></td>
  </tr>
  <tr>
    <td align="center">Simulation</td>
    <td align="center">Integration in the Blender scene</td>
  </tr>
</table>

### Buoyancy
A grayscale image is loaded as the initial smoke density. The buoyancy force pushes the densest (brightest) areas upward and the image dissolves into smoke.

[![Buoyancy](assets/previews/buoyancy.jpg)](https://github.com/user-attachments/assets/45c396ea-0fce-4e45-a8a4-3897cb08f920)

### Live demo
Real-time interactive session: a color image turned into smoke, pushed with the left click, with colored smoke added with the right click.

[![Live demo](assets/previews/live_demo_small.jpg)](https://github.com/user-attachments/assets/87fe49fa-5a5b-4f90-adea-7f7afeef6d07)

## Requirements
- CMake (>= 3.10) and a C++20 compiler
- SFML 3 (`brew install sfml` on macOS)
- ffmpeg (optional, to turn the frames into a video)

## Build
```sh
cmake -S . -B build
cmake --build build -j
```

## 2D simulation (real time)
```sh
# grayscale smoke (draw with the mouse)
./build/2D/smoke-simulator-2D
# a color image turned into smoke (default: assets/ladybug.png)
./build/2D/smoke-simulator-2D --image
./build/2D/smoke-simulator-2D --image my.png
```
Controls: left click + drag = push the smoke, right click = add smoke, D / P / V = density / pressure / velocity view, Esc = quit.

## 3D simulation (offline ray marching)
```sh
./build/3D/smoke-simulator-3D --frames 100

# gather images into video
ffmpeg -framerate 24 -i outputs/3D/frame_%04d.ppm -c:v libx264 -crf 18 -pix_fmt yuv420p outputs/3D/smoke3D.mp4
```
Each frame is saved to `outputs/3D/` as soon as it is rendered, so the result can be watched while the render goes on. The other test scenes render a single image: `basic`, `style`.

## Artistic fail
Not every run went as planned. This unexpected result came up during development, and we liked it enough to keep it.

[![Artistic fail](assets/previews/fail_artistique.jpg)](https://github.com/user-attachments/assets/38ebad54-5dcb-4dc0-b91e-026806ee53aa)
