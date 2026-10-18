#ifndef TAXI_VIEW_COORDINATES_H
#define TAXI_VIEW_COORDINATES_H
#include <math.h>
#include <stdint.h>
/* The original camera projects forward +Z and screen-right +X. OpenGL
   looks down -Z. Reflect X for the native view, then use 180-heading yaw.
   Apply the same conversion to scene vertices, focus and taxi position. */
static inline double taxi_view_x(double raw_x) {return -raw_x;}
static inline double taxi_view_yaw(double heading_fixed,double orbit_degrees) {
    return fmod(180-heading_fixed*360/16777216.0+orbit_degrees,360);
}
static inline int16_t taxi_view_sprite_angle(double yaw_degrees) {
    /* The original selector receives camera rotation, not an inferred
       observer bearing. Keep visual frame identity separate from this math. */
    return (int16_t)(fmod(180-yaw_degrees,360)*4096/360);
}
/* Normal mode2 camera: global1674=(12288,4096,12288) gives camera
   height468 and distance3072 at scale_e84=4096. This render interpolation
   is floating point; original matrix/tick rounding is not reproduced here. */
static inline void taxi_view_camera_offset(double yaw_degrees,double pitch_degrees,
    double height,double distance,double out[3]) {
    double y=yaw_degrees*3.141592653589793/180,p=pitch_degrees*3.141592653589793/180;
    double z=distance*cos(p)-height*sin(p);
    out[0]=-sin(y)*z;out[1]=height*cos(p)+distance*sin(p);out[2]=cos(y)*z;
}
#endif
