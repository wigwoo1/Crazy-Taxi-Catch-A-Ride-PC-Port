#include "vehicle_math.h"
#include <string.h>
/* Explicit bit conversions avoid signed overflow and implementation-defined
   right shifts. Every multiply matches the original low 32-bit result. */
static int32_t signed_bits(uint32_t x) { int32_t s; memcpy(&s, &x, 4); return s; }
static int32_t add(int32_t a, int32_t b) { return signed_bits((uint32_t)a + (uint32_t)b); }
static int32_t sub(int32_t a, int32_t b) { return signed_bits((uint32_t)a - (uint32_t)b); }
static int32_t mul(int32_t a, int32_t b) { return signed_bits((uint32_t)a * (uint32_t)b); }
static int32_t asr(int32_t x, unsigned n) {
    uint32_t u = (uint32_t)x;
    return signed_bits((u >> n) | (x < 0 ? (~UINT32_C(0) << (32-n)) : 0));
}
/* 08011fe4..0801203a, repeated for fields 74c, 750 and 34. */
int32_t taxi_vehicle_damp(int32_t value) {
    int32_t next = sub(value, value / 8);
    return (uint32_t)next + 8192u <= 16384u ? 0 : next;
}
/* 08015b00..08015b46. ROM 084a59dc is 60. Normal delta counts VBlanks (see timing.c); callers pass that integer,
   not elapsed seconds or rendered frames. */
void taxi_vehicle_vertical_step(TaxiVehicleMath *v, int32_t delta) {
    v->state_698 = add(v->state_698, mul(delta, 0xb481) / 60);
    v->y = sub(v->y, mul(v->state_698, mul(delta, 256)) / (60 * 4096));
}
/* Complete 08015b54..08015bae; the original ignores r1/r2. */
void taxi_vehicle_contact_decay(TaxiVehicleMath *v) {
    int32_t factor = (v->flags_28 & 0x10) ? 0xfae : 0xf99;
    if (!(v->flags_28 & 0x10)) v->state_544 = asr(mul(v->state_544, factor), 12);
    v->state_40 = asr(mul(v->state_40, factor), 12);
    v->state_44 = asr(mul(v->state_44, factor), 12);
    v->state_2c = 0;
}
/* Complete 08015bb8..08015c1c. Preserve even the original INT_MIN
   negation behavior, rather than replacing it with a mathematical clamp. */
static void response_step_impl(TaxiVehicleMath *v) {
    int32_t r = v->state_540;
    int32_t magnitude = r < 0 ? sub(0, r) : r;
    if (magnitude > 10240) r = r < 0 ? -10240 : 10240;
    int32_t target = asr(mul(r, (v->flags_4ac & 0x10) ? 45 : -136), 12);
    v->state_744 = add(v->state_744, sub(target, v->state_744) / 4);
}

static TaxiResponseObserver observer;
void taxi_vehicle_response_observer(TaxiResponseObserver value){observer=value;}
void taxi_vehicle_response_step(TaxiVehicleMath *v){if(!observer){response_step_impl(v);return;}TaxiVehicleMath before=*v;response_step_impl(v);observer(&before,v);}
