#pragma once

#define CLASS_BOAT          0
#define CLASS_SWIMMER       1
#define CLASS_JETSKI        2
#define CLASS_BUOY          3
#define CLASS_LIFE_SAVING   4

#define TINY_VESSEL_SIZE    32   // pixels

int is_tiny_vessel(float w_norm, float h_norm, int img_w, int img_h);
