#include "passenger_delivery.h"
#include "destination.h"
int taxi_passenger_delivery(const TaxiDeliveryInput *in,TaxiDeliveryEffects *out) {
    if(!in||!out) return -1;
    TaxiDeliveryEffects e={.gate_1700=in->gate_1700};
    if(!in->gate_1708&&in->destination_index!=-1&&in->active_1694&&
       !in->gate_1700&&!in->pending_1704&&!in->speed_2b78) {
        if(in->destination_index<0||!in->destination_record) return -1;
        uint32_t half=in->material_e60&65535u;
        uint32_t signed_material=half|(half&32768u?0xffff0000u:0);
        if(taxi_destination_accepts(in->destination_index,in->destination_record,signed_material)) {
            e.accepted=1;e.gate_1700=120;
        }
    }
    *out=e;return 0;
}
