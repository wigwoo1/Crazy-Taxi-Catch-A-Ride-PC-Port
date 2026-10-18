#include "obj_order.h"
static void swap(TaxiObjRequest *a,TaxiObjRequest *b){TaxiObjRequest t=*a;*a=*b;*b=t;}
static void source_sort(TaxiObjRequest *a,int low,int high){
    if(low>=high)return;
    if(low==high-1){
        if(a[low].anchor[2]>a[high].anchor[2]||
           (a[low].anchor[2]==a[high].anchor[2]&&a[low].anchor[0]<a[high].anchor[0]))swap(a+low,a+high);
        return;
    }
    int left=low,right=high;
    TaxiObjRequest pivot=a[(low+high)/2];
    a[(low+high)/2]=a[high];a[high]=pivot;
    while(left<right){
        while(left<right&&(a[left].anchor[2]<pivot.anchor[2]||
              (a[left].anchor[2]==pivot.anchor[2]&&a[left].anchor[0]<pivot.anchor[0])))left++;
        while(left<right&&(a[right].anchor[2]>pivot.anchor[2]||
              (a[right].anchor[2]==pivot.anchor[2]&&a[right].anchor[0]>=pivot.anchor[0])))right--;
        if(left<right)swap(a+left,a+right);
    }
    a[high]=a[right];a[right]=pivot;
    source_sort(a,low,left-1);source_sort(a,right+1,high);
}
int taxi_obj_sort(TaxiObjRequest *a,size_t count){
    if(count>128||(!a&&count))return -1;
    if(count>1)source_sort(a,0,(int)count-1);
    return 0;
}

#include <string.h>
int taxi_obj_merge(const uint16_t base[128][4],size_t n0,
    const uint16_t (*b1)[4],size_t n1,const TaxiObjRequest *b2,size_t n2,
    const int16_t (*matrices)[4],uint16_t output[128][4]) {
    if(!base||!output||n0>128||n1>128||n2>128||n0+n1+n2>128||
       (!b1&&n1)||(!b2&&n2))return -1;
    uint16_t result[128][4];TaxiObjRequest sorted[128];
    memcpy(result,base,sizeof result);
    if(n1)memcpy(result+n0,b1,n1*sizeof *b1);
    if(n2){
        memcpy(sorted,b2,n2*sizeof *b2);
        if(taxi_obj_sort(sorted,n2))return -1;
        for(size_t i=0;i<n2;i++)memcpy(result[n0+n1+i],sorted[i].attributes,sizeof sorted[i].attributes);
    }
    if(matrices)for(size_t i=0;i<32;i++)for(size_t j=0;j<4;j++)result[4*i+j][3]=(uint16_t)matrices[i][j];
    memcpy(output,result,sizeof result);return 0;
}

int taxi_obj_matrix_slot(TaxiObjMatrixSlot slots[32],const int16_t values[4]) {
    if(!slots||!values)return -1;
    for(unsigned i=0;i<32;i++)if(slots[i].state==1&&!memcmp(slots[i].values,values,4*sizeof *values))return (int)i;
    for(unsigned i=0;i<32;i++)if(!slots[i].state){memcpy(slots[i].values,values,4*sizeof *values);slots[i].state=1;return (int)i;}
    return -1;
}

static int16_t coefficient(uint32_t shifted,int16_t scale){
    int32_t numerator;memcpy(&numerator,&shifted,sizeof numerator);
    int64_t quotient=scale?(int64_t)numerator/scale:numerator;
    uint16_t narrowed=(uint16_t)quotient;int16_t result;memcpy(&result,&narrowed,sizeof result);return result;
}
int taxi_obj_matrix_values(int32_t sine,int32_t cosine,int16_t scale,int16_t values[4]){
    if(!values)return -1;
    uint32_t c=(uint32_t)cosine<<8,s=(uint32_t)sine<<8;
    values[0]=coefficient(c,scale);values[1]=coefficient(s,scale);
    values[2]=coefficient((0u-(uint32_t)sine)<<8,scale);values[3]=coefficient(c,scale);return 0;
}

static int floor256(int32_t n){return n>=0?n/256:-(int)((-(int64_t)n+255)/256);}
int taxi_obj_affine_point(const int16_t m[4],unsigned w,unsigned h,int doubled,int x,int y,int *sx,int *sy){
    if(!m||!sx||!sy||w<8||w>64||h<8||h>64||(w&(w-1))||(h&(h-1))||(doubled!=0&&doubled!=1))return -1;
    int fw=(int)w*(doubled+1),fh=(int)h*(doubled+1);
    if(x<0||y<0||x>=fw||y>=fh)return 0;
    int dx=x-fw/2,dy=y-fh/2;
    int u=floor256((int32_t)m[0]*dx+(int32_t)m[1]*dy+(int32_t)(w/2)*256);
    int v=floor256((int32_t)m[2]*dx+(int32_t)m[3]*dy+(int32_t)(h/2)*256);
    if(u<0||v<0||u>=(int)w||v>=(int)h)return 0;
    *sx=u;*sy=v;return 1;
}

int taxi_obj_field(int32_t x,int32_t y,unsigned w,unsigned h,int16_t scale,int16_t angle,TaxiObjField *out){
    if(!out||w<8||w>64||h<8||h>64||(w&(w-1))||(h&(h-1)))return -1;
    int doubled=scale>256||angle!=0;unsigned fw=w*(unsigned)(doubled+1),fh=h*(unsigned)(doubled+1);
    uint32_t left=(uint32_t)x-fw/2,top=(uint32_t)y-fh/2;TaxiObjField field={.width=fw,.height=fh,.double_size=doubled};
    memcpy(&field.left,&left,4);memcpy(&field.top,&top,4);*out=field;return 0;
}
/* Source1570a..579c and2dc78. Resolved trig belongs to caller's -bodyroll. */
static int32_t obj_signed(uint32_t n){int32_t v;memcpy(&v,&n,4);return v;}
static int32_t obj_asr8(uint32_t n){
 return obj_signed((n>>8)|((n&0x80000000u)?0xff000000u:0u));
}
int taxi_obj_half_centers(int32_t anchor_x,int32_t anchor_y,int32_t scale,
 int rotate,int32_t sine,int32_t cosine,int32_t centers[2][2]){
 if(!centers)return -1;
 uint32_t extent=(uint32_t)scale<<5;
 for(unsigned half=0;half<2;half++){
  uint32_t x=half?extent-(uint32_t)scale:0u-extent,y=0u-extent;
  if(rotate){
   uint32_t rx=x*(uint32_t)cosine-y*(uint32_t)sine;
   uint32_t ry=x*(uint32_t)sine+y*(uint32_t)cosine;
   x=(uint32_t)obj_asr8(rx);y=(uint32_t)obj_asr8(ry);
  }
  centers[half][0]=obj_signed((uint32_t)anchor_x+(uint32_t)obj_asr8(x));
  centers[half][1]=obj_signed((uint32_t)anchor_y+(uint32_t)obj_asr8(y));
 }
 return 0;
}

uint16_t taxi_obj_palette_mask_attribute(uint16_t attr0,uint16_t obj_mask){
    return (uint16_t)(attr0|((obj_mask&0x400)?0x1000:0));
}

int taxi_obj_screen_origin(TaxiObjField *field,uint8_t mode,
 const int32_t offsets[3],const uint8_t *rows,size_t size){
 if(!field)return -1;
 TaxiObjField next=*field;
 if(mode>=1&&mode<=3){
  if(!offsets)return -1;
  uint32_t bits=(uint32_t)offsets[mode-1];
  int32_t amount=obj_signed((bits>>12)|((bits&0x80000000u)?0xfff00000u:0u));
  if(mode==2)next.top=obj_signed((uint32_t)next.top+(uint32_t)amount);
  else next.left=obj_signed((uint32_t)next.left+(uint32_t)amount);
 }else if(mode==4&&(uint32_t)next.left<=255){
  if(!rows||size<2048)return -1;
  const uint8_t *p=rows+((uint32_t)next.left>>1)*16+4;
  uint32_t value=p[0]|((uint32_t)p[1]<<8)|((uint32_t)p[2]<<16)|((uint32_t)p[3]<<24);
  next.top=obj_signed((uint32_t)next.top+(uint32_t)obj_asr8(value));
 }
 *field=next;return 0;
}
