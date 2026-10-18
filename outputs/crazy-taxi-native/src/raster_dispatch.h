#ifndef TAXI_RASTER_DISPATCH_H
#define TAXI_RASTER_DISPATCH_H
#include <stdint.h>
typedef struct {unsigned vertices,consumer;} TaxiRasterDispatch;
/* Original2b8dc..2b95e. Consumers0/1/2 refer to material-selected
   callback bank03006920 slots0/4/8;3/4 are global overrides0691c/069b0. */
TaxiRasterDispatch taxi_raster_dispatch(uint16_t packet_flags);
/* Original2b720..8da material bank followed by finalpriority.
   Returns the original callback global address, before relocation dereference. */
uint32_t taxi_raster_callback_global(uint16_t packet_flags, int textured,
    uint8_t material_flags);
/* Normal world producer03584..36c4, excluding special packets. */
uint16_t taxi_world_packet_flags(unsigned vertices, unsigned clip_flags,
    int32_t screen_width, int32_t screen_height, int8_t normal_x,
    int8_t normal_y, int8_t normal_z, int flat_cache,
    int32_t max_depth, int32_t far_depth, int32_t shade_depth);
#endif
