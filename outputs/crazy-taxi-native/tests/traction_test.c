#ifdef NDEBUG
#undef NDEBUG
#endif
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "../src/traction.h"
static TaxiTrig trig;
static TaxiAngleTable table;
int main(int argc,char **argv) {
    table.values[256]=512;
    trig.sine[1024]=255;trig.sine[3072]=-256;
    trig.cosine[0]=256;trig.cosine[2048]=-256;
    if(argc==2) {
        FILE *f=fopen(argv[1],"rb");assert(f);
        uint8_t b[514],s[8192],c[8192];
        assert(!fseek(f,0x7f09ee,SEEK_SET));assert(fread(b,1,sizeof b,f)==sizeof b);
        assert(!fseek(f,0x4a0d08,SEEK_SET));assert(fread(s,1,sizeof s,f)==sizeof s);
        assert(fread(c,1,sizeof c,f)==sizeof c);fclose(f);
        taxi_angle_table_decode(&table,b);taxi_trig_decode(&trig,s,c);
        assert(table.values[0]==0&&table.values[256]==512);
        for(int i=1;i<=256;i++) assert(table.values[i]>=table.values[i-1]);
        for(int i=0;i<=256;i++) {
            int32_t angle;
            assert(!taxi_vector_angle(i,256,&table,&angle)&&angle==table.values[i]);
            assert(!taxi_vector_angle(i,-256,&table,&angle)&&angle==2048-table.values[i]);
            if(i) {
                assert(!taxi_vector_angle(-i,256,&table,&angle)&&angle==4096-table.values[i]);
                assert(!taxi_vector_angle(-i,-256,&table,&angle)&&angle==2048+table.values[i]);
            }
        }
    }
    int32_t angle;
    const int vectors[][3]={{0,0,1024},{1,0,1024},{0,1,0},{-1,0,3072},{0,-1,2048},{1,1,512},{1,-1,1536},{-1,-1,2560},{-1,1,3584}};
    for(unsigned i=0;i<sizeof vectors/sizeof vectors[0];i++) {
        assert(!taxi_vector_angle(vectors[i][0],vectors[i][1],&table,&angle));assert(angle==vectors[i][2]);
    }
    TaxiTraction v={.force_540=150,.capacity_570=100,.capacity_574=100,.speed_544=45520};
    TaxiTractionContext c={0};TaxiOrientationEffects e;
    assert(!taxi_traction_step(&v,&c,&trig,&table,&e));assert(v.state_564==1&&!e.score_requested);
    v.force_540=151;
    assert(!taxi_traction_step(&v,&c,&trig,&table,&e));assert(!v.state_564&&c.flag_262c&&e.score_requested&&e.score_c==25);
    assert(!taxi_traction_step(&v,&c,&trig,&table,&e));assert(!e.score_requested);
    const int speeds[]={45521,91040,91041,136560,136561,-100};
    const unsigned points[]={50,50,75,75,0,25};
    for(unsigned i=0;i<6;i++) {c.flag_262c=0;v.speed_544=speeds[i];assert(!taxi_traction_step(&v,&c,&trig,&table,&e));assert(e.score_requested&&e.score_c==points[i]&&e.score_b==(i==4));}
    v.force_540=10;v.phase_e6a=2;v.phase_e6b=0;v.speed_544=0;
    assert(!taxi_traction_step(&v,&c,&trig,&table,&e));assert(!v.state_564&&c.flag_25d4);
    c.settings=128;assert(!taxi_traction_step(&v,&c,&trig,&table,&e));assert(v.state_564);
    c.settings=0;v.phase_e6b=120;v.speed_544=34140;
    assert(!taxi_traction_step(&v,&c,&trig,&table,&e));assert(v.state_564);
    v.speed_544=34139;assert(!taxi_traction_step(&v,&c,&trig,&table,&e));assert(!v.state_564);
    c.flag_1696=1;v.flags_28=8;v.speed_544=4096;
    assert(!taxi_traction_step(&v,&c,&trig,&table,&e));assert(v.state_564&&v.speed_544==4075&&!c.flag_25d4);
    v.force_53c=-0x01000000;v.force_540=0x00800000;
    TaxiTraction before=v;TaxiTractionContext old=c;TaxiOrientationEffects prior=e;
    assert(taxi_traction_step(&v,&c,&trig,&table,&e)==-1);
    assert(!memcmp(&before,&v,sizeof v)&&!memcmp(&old,&c,sizeof c)&&!memcmp(&prior,&e,sizeof e));
    puts("Traction and direction-table tests passed");return 0;
}
