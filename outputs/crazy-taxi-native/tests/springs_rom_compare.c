#include "../src/body_pose.h"
#include <stdio.h>
int main(void){int p,v,q,w,a,b;while(scanf("%d%d%d%d%d%d",&p,&v,&q,&w,&a,&b)==6){TaxiBodyPose s={.position_734=p,.velocity_73c=v,.position_740=q,.velocity_748=w,.denominator_754=a,.denominator_758=b};int rc=taxi_body_springs(&s);printf("%d %d %d %d %d %d %d\n",rc,s.position_734,s.velocity_73c,s.position_740,s.velocity_748,s.response_704,s.response_70c);}return 0;}
