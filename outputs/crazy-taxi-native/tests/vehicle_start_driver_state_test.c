#ifdef NDEBUG
#undef NDEBUG
#endif
#include "../src/vehicle_start_driver.h"
#include <assert.h>
int main(void){
 TaxiDriver source={0},restored={0};TaxiDriverContext context={0};TaxiVehicleStartPacked packed={0};
 source.rpm_53a=-321;
 assert(!taxi_vehicle_start_driver_import(&packed,&source));
 assert((int16_t)(packed.actor[0x53a]|((uint16_t)packed.actor[0x53b]<<8))==-321);
 packed.actor[0x53a]=0;packed.actor[0x53b]=1;
 assert(!taxi_vehicle_start_driver_export(&restored,&context,&packed,0));
 assert(restored.rpm_53a==256);
 return 0;
}
