#ifdef NDEBUG
#undef NDEBUG
#endif
#include "../src/obj_order.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
static void fixture(const int16_t *xs,const unsigned *ids,size_t count){
 TaxiObjRequest a[128]={0};
 for(size_t i=0;i<count;i++){a[i].anchor[0]=xs[i];a[i].anchor[1]=(int16_t)i;a[i].attributes[0]=(uint16_t)i;a[i].attributes[3]=(uint16_t)(100+i);}
 assert(!taxi_obj_sort(a,count));
 for(size_t i=0;i<count;i++){assert(a[i].attributes[0]==ids[i]);assert(a[i].anchor[1]==(int16_t)ids[i]);assert(a[i].attributes[3]==100+ids[i]);}
}
int main(void){
 assert(taxi_obj_palette_mask_attribute(0x0400,0)==0x0400);
 assert(taxi_obj_palette_mask_attribute(0x0400,0x400)==0x1400);
 assert(taxi_obj_palette_mask_attribute(0x1200,0x400)==0x1200);
 assert(taxi_obj_palette_mask_attribute(0xffff,0xffff)==0xffff);
 assert(taxi_obj_palette_mask_attribute(0x0400,0xfbff)==0x0400);
 const int16_t two[]={1,2},three[]={1,2,3},equal[]={1,1,1},four[]={4,3,2,1};
 const unsigned two_ids[]={1,0},three_ids[]={0,1,2},equal_ids[]={1,2,0},four_ids[]={2,3,1,0};
 fixture(two,two_ids,2);fixture(three,three_ids,3);fixture(equal,equal_ids,3);fixture(four,four_ids,4);
 assert(!taxi_obj_sort(NULL,0));assert(taxi_obj_sort(NULL,1)==-1);
 TaxiObjRequest a[128]={0},copy[128];uint32_t seed=7;
 for(unsigned trial=0;trial<4096;trial++){
  unsigned n=trial%129;
  for(unsigned i=0;i<n;i++){seed=seed*1664525u+1013904223u;a[i]=(TaxiObjRequest){.anchor={(int16_t)seed,(int16_t)i,(int16_t)(seed>>16),0},.attributes={(uint16_t)i,0,0,0}};}
  memcpy(copy,a,sizeof a);assert(!taxi_obj_sort(a,n));
  unsigned seen[128]={0};
  for(unsigned i=0;i<n;i++){unsigned id=a[i].attributes[0];assert(id<n&&!seen[id]++);assert(!memcmp(a+i,copy+id,sizeof *a));if(i)assert(a[i-1].anchor[2]<=a[i].anchor[2]);}
 }
 memcpy(copy,a,sizeof a);assert(taxi_obj_sort(a,129)==-1);assert(!memcmp(a,copy,sizeof a));
 uint16_t base[128][4],merged[128][4],saved[128][4],bucket1[2][4]={{900,901,902,903},{910,911,912,913}};
 int16_t matrices[32][4];TaxiObjRequest bucket2[3]={0},original2[3];
 for(unsigned i=0;i<128;i++)for(unsigned j=0;j<4;j++)base[i][j]=(uint16_t)(i*4+j);
 for(unsigned i=0;i<32;i++)for(unsigned j=0;j<4;j++)matrices[i][j]=(int16_t)(-32768+i*4+j);
 for(unsigned i=0;i<3;i++){bucket2[i].anchor[2]=(int16_t)(3-i);for(unsigned j=0;j<4;j++)bucket2[i].attributes[j]=(uint16_t)(1000+10*i+j);}
 memcpy(original2,bucket2,sizeof bucket2);
 assert(!taxi_obj_merge(base,2,bucket1,2,bucket2,3,NULL,merged));
 for(unsigned i=0;i<128;i++)for(unsigned j=0;j<4;j++){
  uint16_t expected=base[i][j];if(i>=2&&i<4)expected=bucket1[i-2][j];if(i>=4&&i<7)expected=original2[6-i].attributes[j];assert(merged[i][j]==expected);
 }
 assert(!memcmp(bucket2,original2,sizeof bucket2));
 memcpy(saved,merged,sizeof saved);
 assert(!taxi_obj_merge(base,2,bucket1,2,bucket2,3,matrices,merged));
 for(unsigned i=0;i<128;i++){assert(merged[i][3]==(uint16_t)matrices[i/4][i%4]);for(unsigned j=0;j<3;j++)assert(merged[i][j]==saved[i][j]);}
 memcpy(saved,merged,sizeof saved);assert(taxi_obj_merge(base,127,bucket1,2,NULL,0,NULL,merged)==-1);assert(!memcmp(saved,merged,sizeof saved));
 assert(!taxi_obj_merge(base,128,NULL,0,NULL,0,NULL,merged));assert(!memcmp(base,merged,sizeof base));
 TaxiObjMatrixSlot slots[32]={0},slots_saved[32];int16_t matrix_values[4]={-32768,32767,-1,256};
 slots[7].state=1;memcpy(slots[7].values,matrix_values,sizeof matrix_values);
 assert(taxi_obj_matrix_slot(slots,matrix_values)==7);assert(!slots[0].state);
 slots[7].state=2;assert(taxi_obj_matrix_slot(slots,matrix_values)==0);assert(slots[0].state==1);
 for(unsigned i=1;i<32;i++){if(i==7)continue;matrix_values[0]=(int16_t)i;assert(taxi_obj_matrix_slot(slots,matrix_values)==(int)i);}
 memcpy(slots_saved,slots,sizeof slots);matrix_values[0]=300;assert(taxi_obj_matrix_slot(slots,matrix_values)==-1);assert(!memcmp(slots_saved,slots,sizeof slots));
 matrix_values[0]=31;assert(taxi_obj_matrix_slot(slots,matrix_values)==31);assert(!memcmp(slots_saved,slots,sizeof slots));
 assert(taxi_obj_matrix_slot(NULL,matrix_values)==-1);assert(taxi_obj_matrix_slot(slots,NULL)==-1);
 int16_t generated[4];
 assert(!taxi_obj_matrix_values(0,256,256,generated));assert(generated[0]==256&&generated[1]==0&&generated[2]==0&&generated[3]==256);
 assert(!taxi_obj_matrix_values(256,0,256,generated));assert(generated[0]==0&&generated[1]==256&&generated[2]==-256&&generated[3]==0);
 assert(!taxi_obj_matrix_values(0,256,257,generated));assert(generated[0]==255&&generated[3]==255);
 assert(!taxi_obj_matrix_values(256,0,-256,generated));assert(generated[1]==-256&&generated[2]==256);
 assert(!taxi_obj_matrix_values(-1,1,3,generated));assert(generated[0]==85&&generated[1]==-85&&generated[2]==85);
 assert(!taxi_obj_matrix_values(256,256,0,generated));for(unsigned i=0;i<4;i++)assert(!generated[i]);
 assert(!taxi_obj_matrix_values(0,8388608,-1,generated));assert(!generated[0]);
 assert(taxi_obj_matrix_values(0,256,256,NULL)==-1);
 int u=-7,v=-9;const int16_t identity[4]={256,0,0,256};
 for(int y=0;y<64;y++)for(int x=0;x<64;x++){assert(taxi_obj_affine_point(identity,64,64,0,x,y,&u,&v)==1);assert(u==x&&v==y);assert(taxi_obj_affine_point(identity,64,64,1,x+32,y+32,&u,&v)==1);assert(u==x&&v==y);}
 u=-7;v=-9;assert(!taxi_obj_affine_point(identity,64,64,1,31,32,&u,&v));assert(u==-7&&v==-9);
 assert(!taxi_obj_affine_point(identity,64,64,1,96,32,&u,&v));assert(taxi_obj_affine_point(identity,63,64,0,0,0,&u,&v)==-1);
 const int16_t half[4]={128,0,0,128};assert(taxi_obj_affine_point(half,64,64,1,1,1,&u,&v)==1);assert(u==0&&v==0);
 TaxiObjField field,saved_field;
 assert(!taxi_obj_field(120,80,64,64,256,0,&field));assert(field.left==88&&field.top==48&&field.width==64&&!field.double_size);
 assert(!taxi_obj_field(120,80,64,64,257,0,&field));assert(field.left==56&&field.top==16&&field.width==128&&field.double_size);
 assert(!taxi_obj_field(120,80,64,32,-257,0,&field));assert(field.left==88&&field.top==64&&field.height==32&&!field.double_size);
 assert(!taxi_obj_field(120,80,64,32,256,4096,&field));assert(field.left==56&&field.top==48&&field.height==64&&field.double_size);
 saved_field=field;assert(taxi_obj_field(0,0,7,64,256,0,&field)==-1);assert(!memcmp(&field,&saved_field,sizeof field));
 assert(!taxi_obj_field(INT32_MIN,INT32_MIN,64,64,256,0,&field));assert(field.left==INT32_MAX-31&&field.top==INT32_MAX-31);
 puts("PASS source OBJ ordering tie fixtures, paired records, signed depth and host bounds; renderer not integrated.");return 0;
}
