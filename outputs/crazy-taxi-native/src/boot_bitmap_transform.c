#include "boot_bitmap_transform.h"
int taxi_boot_bitmap_transforms(TaxiBitmapTransform state[2]){
 if(!state)return -1;
 for(unsigned i=0;i<2;i++)state[i]=(TaxiBitmapTransform){256,0,0,256,0,0};
 return 0;
}
