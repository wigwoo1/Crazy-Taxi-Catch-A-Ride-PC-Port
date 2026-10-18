#include "boot_bitmap_transform.h"
#include <stdio.h>
#include <io.h>
#include <fcntl.h>
_Static_assert(sizeof(TaxiBitmapTransform)==16,"transform fixture layout");
int main(void){TaxiBitmapTransform state[2];
 if(_setmode(_fileno(stdin),_O_BINARY)<0||_setmode(_fileno(stdout),_O_BINARY)<0)return 1;
 if(fread(state,1,sizeof state,stdin)!=sizeof state)return 2;
 if(taxi_boot_bitmap_transforms(state))return 3;
 return fwrite(state,1,sizeof state,stdout)==sizeof state?0:4;
}
