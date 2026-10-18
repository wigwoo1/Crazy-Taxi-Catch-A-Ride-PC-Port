#include "taxi_sprite.h"
#include "orientation.h"
unsigned taxi_sprite_frame(int16_t camera_angle,int32_t heading_1c) {
    int32_t difference=taxi_angle_difference(camera_angle,heading_1c/4096);
    return ((32u-(uint32_t)difference)&4095u)>>6;
}
