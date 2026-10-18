#ifndef TAXI_CHALLENGE_BALL_H
#define TAXI_CHALLENGE_BALL_H
#include <stdint.h>
typedef struct {int16_t popped_1556,remaining_1558;int32_t velocity_155c[20][3];} TaxiChallengeBall;
/* Original AA0C after shared ABC4. Actor ownership remains with caller. */
void taxi_challenge_ball_begin(TaxiChallengeBall *state);
/* Original86FC..87DE. X/Z advance is gated by pause; friction is not.
   Caller owns contact861C impulse, actor setup, drawing and scalar timer. */
int taxi_challenge_ball_motion_step(TaxiChallengeBall *state,uint8_t actor[44],unsigned index,int pause,int32_t delta);
/* Original2DCD0 uses a linear slope, not the traction angle table.
   Inputs are source r0=Z difference and r1=X difference. */
int32_t taxi_challenge_ball_impulse_angle(int32_t z,int32_t x);
/* Original861C. Caller supplies original RAM axis tables and BIOS Sqrt
   service contract; no BIOS instruction execution or independent oracle. */
typedef uint32_t (*TaxiBallRoot)(uint32_t value);
int taxi_challenge_ball_impulse(TaxiChallengeBall *state,const uint8_t actors[20][44],unsigned popped_index,const int32_t x_table[4096],const int32_t z_table[4096],TaxiBallRoot root);
typedef struct {uint8_t phase,outer,intro,pause,effects_enabled;int32_t delta,car_x,car_z;} TaxiBallInput;
typedef struct {uint8_t success,ended;} TaxiBallStatus;
typedef struct {uint8_t warning_palette,warning_sound,request_rank,render_world;uint32_t popped_mask;unsigned pop_sound_count;} TaxiBallEffects;
/* Source8234 scalar/contact traversal; caller owns10B54/BA04/pools and rank. */
int taxi_challenge_ball_step(TaxiChallengeBall *state,uint8_t actors[20][44],const TaxiBallInput *input,TaxiBallStatus *status,TaxiBallEffects *effects,const int32_t x_table[4096],const int32_t z_table[4096],TaxiBallRoot root);
typedef struct {int32_t x,y,z,scale;uint32_t bitmap_identity;} TaxiBallRenderRequest;
/* Source233D8 submode2 draw, including renderer-owned pop animation.
   Caller resolves29B44 ground height and owns projection/queue1536. */
int taxi_challenge_ball_render_request(uint8_t actor[44],int32_t player_x,int32_t player_z,int32_t ground_y,int pause,int32_t delta,uint32_t factor,TaxiBallRenderRequest *out);
#endif
