#include "title_font_setup.h"
int taxi_title_font_setup(TaxiFontSetup *s){
 if(taxi_font_setup(s,0,32,0,0x083fd3b4,3,0))return -1;
 return taxi_font_setup(s,1,32,1,0x084097b8,1,0);
}
