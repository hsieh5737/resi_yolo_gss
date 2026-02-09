#include "yolo_label.h"

/*
 * YOLO format:
 * class x_center y_center width height (normalized)
 */
int is_tiny_vessel(float w_norm, float h_norm, int img_w, int img_h) {
    int w_px = (int)(w_norm * img_w);
    int h_px = (int)(h_norm * img_h);

    return (w_px < TINY_VESSEL_SIZE && h_px < TINY_VESSEL_SIZE);
}
