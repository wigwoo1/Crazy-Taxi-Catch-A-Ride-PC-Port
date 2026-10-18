#ifndef TAXI_PASSENGER_DELIVERY_H
#define TAXI_PASSENGER_DELIVERY_H
#include <stdint.h>
typedef struct {
    int32_t destination_index, gate_1700, pending_1704, speed_2b78;
    uint32_t material_e60;
    uint8_t active_1694, gate_1708;
    const uint8_t *destination_record; /* Selected original 48-byte record. */
} TaxiDeliveryInput;
typedef struct {
    uint8_t accepted;
    int32_t gate_1700;
    /* If accepted, caller must dispatch these original requests in order:
       0800df8c(),0802939c(),0800cd38(1),0800c540(2,-1),
       0800c540(0,-1),0800ca0c(1). These handlers are not implemented here. */
} TaxiDeliveryEffects;
/* 0800b48a..0800b4f4: acceptance only, not completed delivery/payment.
   Reads the ground material as a SIGNED halfword like original 0800b4c4.
   No state is changed; gate_1700 becomes 120 in the accepted effect. */
int taxi_passenger_delivery(const TaxiDeliveryInput *input,TaxiDeliveryEffects *effects);
#endif
