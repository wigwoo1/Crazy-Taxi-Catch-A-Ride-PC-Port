#include "actions.h"

/* Exact 3x7 table at ROM 0849d4d4. Resolver: 08015d98..080160fe.
   Native validation rejects invalid IDs/profile 3, which the stock call sites
   are expected not to supply. This is an intentional host-side boundary check. */
static const uint32_t profiles[3][7] = {
    {0x20,0x10,0x1,0x2,0xc0000,0xc0000,0x1000},
    {0x20,0x10,0x1,0x2,0x100,0x200,0x1000},
    {0x20,0x10,0x100,0x200,0x1,0x2,0x1000}
};

int taxi_action_query(const TaxiInput *input, TaxiActionContext context, unsigned action) {
    if (!input || context.profile>=3 || action>=7) return 0;
    if (context.override_enabled) {
        if (action>3) return 0;
        return !!(context.override_flags & (1u<<action));
    }
    uint32_t packed=0;
    for (unsigned button=0;button<10;++button) {
        if (input->held[button]) packed |= 1u<<button;
        if (input->pressed[button]) packed |= 1u<<(button+10);
        if (input->repeat[button]) packed |= 1u<<(button+20);
    }
    if (context.profile==0 && (action==4 || action==5)) {
        if (action==4 && !(context.vehicle_flags&0x20)) return 0;
        if (action==5 && (context.vehicle_flags&0x20)) return 0;
        /* Either shoulder edge is sufficient, unlike ordinary all-bits matches. */
        return !!(packed&0xc0000);
    }
    uint32_t mask=profiles[context.profile][action];
    return (packed&mask)==mask;
}
