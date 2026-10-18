#include <stdio.h>
#include <stdlib.h>
#define taxi_game_start_step native_game_start_step
#include "../src/game_start.c"
#undef taxi_game_start_step
static void write_start(FILE *f,const TaxiGameStart *s){fprintf(f," %d %d %d %d %u %ld %ld %ld %u",s->phase,s->hold,s->offset,s->scale,s->rotation,(long)s->axes[0],(long)s->axes[1],(long)s->axes[2],s->active);}
unsigned taxi_game_start_step(TaxiGameStart *s,int32_t delta,int16_t width,int fade,int exiting,int audio){
 const char *path=getenv("TAXI_COUNTDOWN_TRACE");TaxiGameStart before=*s;
 unsigned events=native_game_start_step(s,delta,width,fade,exiting,audio);
 if(path){static unsigned count;FILE *f=fopen(path,count?"a":"w");if(!f){perror("countdown capture");exit(2);}fprintf(f,"%u %ld %d %d %d %d",count++,(long)delta,width,fade,exiting,audio);write_start(f,&before);write_start(f,s);fprintf(f," %u\n",events);int error=ferror(f);if(fclose(f)||error){perror("countdown capture write");exit(2);}}
 return events;
}
