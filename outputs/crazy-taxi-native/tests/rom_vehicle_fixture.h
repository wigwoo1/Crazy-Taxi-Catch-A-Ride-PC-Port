#ifndef TAXI_ROM_VEHICLE_FIXTURE_H
#define TAXI_ROM_VEHICLE_FIXTURE_H
static int32_t word(const uint8_t *b,unsigned offset) {
    b+=offset-0x4ac;uint32_t u=b[0]|((uint32_t)b[1]<<8)|((uint32_t)b[2]<<16)|((uint32_t)b[3]<<24);
    int32_t v;memcpy(&v,&u,4);return v;
}
static int16_t half(const uint8_t *b,unsigned offset) {
    b+=offset-0x4ac;uint16_t u=(uint16_t)(b[0]|((uint16_t)b[1]<<8));int16_t v;memcpy(&v,&u,2);return v;
}
static TaxiAcceleration load(const uint8_t *b) {
    TaxiVehicleConfig c;assert(!taxi_vehicle_config_decode(&c,b,0x248));
    TaxiAcceleration v={0};TaxiGearState *g=&v.gears;
    g->flags_4ac=c.flags_4ac;g->max_gear=c.gear_count;memcpy(g->ratios,c.ratios,sizeof g->ratios);
    g->final_ratio=c.final_ratio;g->reverse_ratio=word(b,0x4d4);
    g->min_530=c.rpm_min;g->max_532=c.rpm_max;g->up_534=half(b,0x534);g->down_536=half(b,0x536);
    g->scale_5b8=c.scale_5b8;g->scale_5cc=c.scale_5cc;g->boost_75c=-1;
    /* Diagnostic active-driver setup, not a translation of the whole spawn. */
    g->gear=g->next_gear=1;g->rpm_538=c.rpm_min;
    v.scale_e84=4096;v.scale_4b0=word(b,0x4b0);v.coasting_4e0=word(b,0x4e0);
    memcpy(v.curve,c.curve,sizeof v.curve);v.radius_5bc=c.scale_5bc;v.radius_5d0=c.scale_5d0;
    v.efficiency_4e4=word(b,0x4e4);v.drag_528=c.limit_528;v.rolling_52c=word(b,0x52c);
    v.drag_590=c.scale_590;v.mass_5a4=word(b,0x5a4);v.brake_608=word(b,0x608);v.brake_60c=word(b,0x60c);
    return v;
}
#endif
