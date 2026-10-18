#ifdef NDEBUG
#undef NDEBUG
#endif
#include "normal_exit.h"
#include <assert.h>
#include <string.h>
#include <stdio.h>
int main(void){
 TaxiNormalExit s={.outer=1,.frozen=1,.ended=1,.qualified=1};TaxiNormalExitRequests q;
 assert(taxi_normal_exit_begin(&s,&q)==2&&q.count==5&&q.calls[4]==0x0800ac70);
 TaxiNormalExit waiting=s;
 assert(taxi_normal_exit_finish(&s,0,&q)==0&&!q.count&&!memcmp(&s,&waiting,sizeof s));
 assert(taxi_normal_exit_begin(&s,&q)==-1); /* Never duplicate the child request. */
 assert(!taxi_normal_exit_finish(&s,1,&q)&&!q.count&&!memcmp(&s,&waiting,sizeof s));
 assert(!taxi_normal_exit_prepare(&s,1,&q)&&!q.count&&!memcmp(&s,&waiting,sizeof s));
 s=(TaxiNormalExit){.outer=2,.frozen=1,.exit_flag=1,.qualified=1};
 assert(taxi_normal_exit_begin(&s,&q)==1&&q.count==4);
 assert(taxi_normal_exit_finish(&s,0,&q)==1&&q.count==1&&q.calls[0]==0x080243a4&&!s.results);
 s=(TaxiNormalExit){.outer=1,.fade=1};assert(!taxi_normal_exit_begin(&s,&q)&&!q.count&&!s.phase);
 s=(TaxiNormalExit){.outer=2,.frozen=1,.ended=9,.qualified=1,.exit_flag=1,.callback=123};
 assert(taxi_normal_exit_begin(&s,&q)==1);
 assert(taxi_normal_exit_prepare(&s,0,&q)==2&&q.count==1&&q.calls[0]==0x080243a4);
 assert(!s.outer&&!s.ended&&s.main_mode&&s.redraw&&s.frozen&&s.callback==123&&s.phase==3);
 waiting=s;
 for(unsigned i=0;i<2100;i++)assert(!taxi_normal_exit_resume(&s,0,&q)&&!q.count&&!memcmp(&s,&waiting,sizeof s));
 assert(taxi_normal_exit_prepare(&s,0,&q)==-1);
 assert(taxi_normal_exit_resume(&s,1,&q)==1&&!q.count&&!s.frozen&&s.callback==0x080174dd&&!s.phase);
 assert(taxi_normal_exit_resume(&s,1,&q)==-1);
 s=(TaxiNormalExit){.outer=1,.frozen=1,.qualified=1};
 assert(taxi_normal_exit_begin(&s,&q)==2);
 waiting=s;assert(!taxi_normal_exit_prepare(&s,0,&q)&&!memcmp(&s,&waiting,sizeof s));
 assert(!taxi_normal_exit_prepare(&s,1,&q)&&!q.count&&!memcmp(&s,&waiting,sizeof s));
 s=(TaxiNormalExit){.outer=2,.frozen=1,.qualified=1};
 assert(taxi_normal_exit_begin(&s,&q)==1);
 assert(taxi_normal_exit_prepare(&s,0,&q)==2);
 assert(taxi_normal_exit_resume(&s,1,&q)==1&&q.count==4&&q.calls[0]==0x0802392c&&s.results);
 puts("PASS normal exit: Retry transfer cannot resume old parent; Quit cleanup and actual credits return.");return 0;
}
