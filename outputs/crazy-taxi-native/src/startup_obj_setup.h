#ifndef TAXI_STARTUP_OBJ_SETUP_H
#define TAXI_STARTUP_OBJ_SETUP_H
#include "startup_obj_transport.h"
/* Original28BD0. Matrix reset template is retained caller storage. */
int taxi_startup_obj_setup(TaxiStartupObjQueue *,TaxiStartupObjTransport *,
 uint16_t base_template[128][4],const TaxiObjMatrixSlot matrix_template[32],int resource_loaded);
#endif
