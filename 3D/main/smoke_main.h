//
// Created by Evrard on 11/04/2026.
//

#ifndef SMOKE_SIMULATOR_SMOKE_MAIN_H
#define SMOKE_SIMULATOR_SMOKE_MAIN_H

#include <iostream>
#include <sstream>
#include "../smoke/fluidGrid.h"

using namespace smoke;

void smoke_step(FluidGrid3D& grid, int frame_nb=10);
FluidGrid3D smoke_main();

#endif //SMOKE_SIMULATOR_SMOKE_MAIN_H