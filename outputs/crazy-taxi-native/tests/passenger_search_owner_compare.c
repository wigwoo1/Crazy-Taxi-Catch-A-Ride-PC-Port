#include "../src/passenger_search.h"
#include <stdio.h>
#include <io.h>
#include <fcntl.h>
int main(void){if(_setmode(_fileno(stdin),_O_BINARY)==-1||_setmode(_fileno(stdout),_O_BINARY)==-1)return 1;uint32_t count;TaxiRoutePoint taxi,spawns[128];TaxiPassengerState states[128];if(fread(&count,4,1,stdin)!=1||count>128||fread(&taxi,sizeof taxi,1,stdin)!=1||fread(states,44,count,stdin)!=count||fread(spawns,sizeof *spawns,count,stdin)!=count)return 2;int32_t index,distance;if(taxi_passenger_search(states,spawns,count,taxi,&index,&distance))return 3;if(fwrite(&index,4,1,stdout)!=1||fwrite(&distance,4,1,stdout)!=1||fwrite(states,44,count,stdout)!=count)return 4;return 0;}
