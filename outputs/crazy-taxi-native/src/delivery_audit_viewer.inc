/* Optional read-only recording at the real post-traffic delivery boundary.
   All rows remain in memory until shutdown; no per-frame network writes. */
static struct {unsigned count,pending,written;int64_t rows[12000][27];} delivery_audit;
static void delivery_audit_before(const TaxiPassengerFrameInput *in){
 if(!delivery_audit_path)return;
 if(delivery_audit.pending||delivery_audit.count>=12000)fail("Delivery audit capacity or boundary failure.");
 int d=passenger_scene.pickup.destination_index;
 int64_t row[27]={live_host_time.counter,in->delta,in->taxi.x,in->taxi.y,in->taxi.z,in->speed,in->material,d,passenger_scene.session.trip.active_1694,passenger_scene.session.gate_1700,passenger_scene.pickup.pending_1704,in->delivery_gate,passenger_scene.session.outer_state_1ea8,passenger_scene.session.ended_1697,in->freeze,passenger_scene.session.clock.remaining_16a8,passenger_scene.session.trip.time_16ac,0,-1,-1,-1,-1,0,passenger_scene.session.end.completed_1692,passenger_scene.session.fare.total_whole_1698,0,0};
 if(d>=0){if((unsigned)d>=passenger_asset.destination_count)fail("Delivery audit destination out of range.");const uint8_t *r=passenger_asset.destinations[d];row[17]=1;for(unsigned j=0;j<4;j++)row[18+j]=(uint16_t)(r[40+2*j]|((uint16_t)r[41+2*j]<<8));}
 memcpy(delivery_audit.rows[delivery_audit.count],row,sizeof row);delivery_audit.pending=1;
}
static void delivery_audit_after(const TaxiPassengerFrameEffects *e){
 if(!delivery_audit_path)return;
 if(!delivery_audit.pending)fail("Delivery audit missing pre-call record.");
 int64_t *r=delivery_audit.rows[delivery_audit.count];r[22]=e->delivered;r[25]=passenger_scene.session.end.completed_1692;r[26]=passenger_scene.session.fare.total_whole_1698;
 delivery_audit.pending=0;delivery_audit.count++;
}
static void delivery_audit_write(void){
 if(!delivery_audit_path||delivery_audit.written)return;delivery_audit.written=1;
 if(delivery_audit.pending){fputs("Delivery audit incomplete boundary.\n",stderr);return;}
 FILE *f=fopen(delivery_audit_path,"wb");if(!f){fputs("Delivery audit open failed.\n",stderr);return;}
 fputs("counter,delta,x,y,z,speed,material,destination,active,gate,pending,delivery_gate,outer,ended,freeze,clock,trip,record,slot0,slot1,slot2,slot3,delivered,completed_before,cash_before,completed_after,cash_after\n",f);
 for(unsigned i=0;i<delivery_audit.count;i++)for(unsigned j=0;j<27;j++)fprintf(f,"%s%lld%s",j?",":"",(long long)delivery_audit.rows[i][j],j==26?"\n":"");
 int error=ferror(f);if(fclose(f))error=1;if(error)fputs("Delivery audit write failed.\n",stderr);
}
