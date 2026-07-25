//
// Copyright (c) Dmitry Akulov. All rights reserved.
//
// Repository info:     https://github.com/pink0D/CameraBrick
// Contact information: pink0D.github@gmail.com
//
// Licensed under the MIT license. See LICENSE file in the project root for details.
//

#include <CameraBrick.h>

void setup() {
    CameraBrick.begin(M5STACK_ATOM_S3R_M12_OV3660(FRAMESIZE_CIF));
}

void loop() {
    CameraBrick.update();
}
