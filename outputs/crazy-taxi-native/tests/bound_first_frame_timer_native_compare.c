#include "challenge_flag_hud.h"
#include <stdio.h>
int main(void){TaxiFlagHudPlan plan;if(taxi_challenge_flag_hud_plan(&plan,2400,160,0))return 1;return fwrite(plan.time,1,9,stdout)==9?0:2;}
