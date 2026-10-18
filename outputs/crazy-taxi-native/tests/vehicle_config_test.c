#ifdef NDEBUG
#undef NDEBUG
#endif
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "../src/vehicle_config.h"
static void put(uint8_t *b,unsigned o,uint32_t x) {for(unsigned i=0;i<4;i++) b[o-0x4ac+i]=(uint8_t)(x>>(8*i));}
int main(int argc,char **argv) {
    uint8_t b[0x248]={0};TaxiVehicleConfig v={0};
    put(b,0x548,1); /* Explicit retained audio divisor for this isolated curve fixture. */
    put(b,0x528,1);assert(!taxi_vehicle_config_decode(&v,b,sizeof b));
    assert(v.peak_6a4==15000&&v.state_564==1);
    put(b,0x4e8,100);put(b,0x4ec,100);assert(!taxi_vehicle_config_decode(&v,b,sizeof b)&&v.peak_6a4==1000);
    put(b,0x690,0xfffffed4u);put(b,0x658,0xfffffff1u);put(b,0x660,680);put(b,0x670,101);
    assert(!taxi_vehicle_config_decode(&v,b,sizeof b));
    assert(v.camera_offset_690==-300&&v.camera_rear_658==-15&&v.camera_mode3_base_660==680&&v.camera_mode3_offset_670==101);
    TaxiVehicleConfig old=v;assert(taxi_vehicle_config_decode(&v,b,sizeof b-1)==-1&&!memcmp(&v,&old,sizeof v));
    put(b,0x528,0);put(b,0x548,0);assert(taxi_vehicle_config_decode(&v,b,sizeof b)==-1&&!memcmp(&v,&old,sizeof v));
    put(b,0x4b4,1);assert(taxi_vehicle_config_decode(&v,b,sizeof b)==-1);
    put(b,0x4b8,4096);put(b,0x4d8,4096);assert(!taxi_vehicle_config_decode(&v,b,sizeof b));
    put(b,0x5ac,100);put(b,0x5b0,50);put(b,0x5c0,200);put(b,0x5c4,50);
    b[0x532-0x4ac]=0x70;b[0x533-0x4ac]=0x17; /* 6000 RPM */
    assert(!taxi_vehicle_config_decode(&v,b,sizeof b)&&v.limit_528==0&&v.speed_divisor_548==257300);
    b[0]=4|32;assert(!taxi_vehicle_config_decode(&v,b,sizeof b)&&v.limit_528==0&&v.speed_divisor_548==128600&&v.flags_4ac==4);
    b[0]=8;assert(!taxi_vehicle_config_decode(&v,b,sizeof b)&&v.limit_528==0);
    for(unsigned i=0;i<16;i++) put(b,0x4e8+i*4,UINT32_MAX);
    put(b,0x6a4,123);assert(!taxi_vehicle_config_decode(&v,b,sizeof b)&&v.peak_6a4==123);
    if(argc==2) {
        FILE *f=fopen(argv[1],"rb");assert(f);
        const long offsets[]={0x4a554c,0x4a5794};
        const int expected[][7]={{7578,1206,7578,1206,9445,7000,3},{8376,1333,8556,1361,9396,4000,6}};
        for(unsigned i=0;i<2;i++) {
            assert(!fseek(f,offsets[i],SEEK_SET)&&fread(b,1,sizeof b,f)==sizeof b);
            assert(!taxi_vehicle_config_decode(&v,b,sizeof b));
            assert(v.scale_5b8==expected[i][0]&&v.scale_5bc==expected[i][1]);
            assert(v.scale_5cc==expected[i][2]&&v.scale_5d0==expected[i][3]);
            assert(v.scale_590==expected[i][4]&&v.peak_6a4==expected[i][5]&&v.gear_count==expected[i][6]);
            assert(v.limit_528==1228&&v.rpm_min==500&&v.rpm_max==8500);
            printf("Template %08lx: scales %d/%d, peak %d\n",offsets[i]+0x08000000,v.scale_5b8,v.scale_5cc,v.peak_6a4);
        }
        fclose(f);
    }
    puts("PASS: native configuration calculations, table ties and invalid-input rollback.");return 0;
}
