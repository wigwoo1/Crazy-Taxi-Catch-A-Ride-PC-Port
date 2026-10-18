#include "font_sdf.h"
#include <math.h>
#include <stdlib.h>
#include <string.h>

#define TAXI_EDT_INF 1.0e30

static void edt_1d(const double *f,unsigned n,double *d,int *arg,
                   unsigned *sites,double *crossings){
    int k=0;
    sites[0]=0;crossings[0]=-TAXI_EDT_INF;crossings[1]=TAXI_EDT_INF;
    for(unsigned q=1;q<n;q++){
        double s;
        for(;;){
            unsigned p=sites[k];
            s=((f[q]+(double)q*q)-(f[p]+(double)p*p))/(2.0*((double)q-p));
            if(s>crossings[k]||k==0)break;
            k--;
        }
        if(s<=crossings[k]&&k==0){
            sites[0]=q;crossings[0]=-TAXI_EDT_INF;crossings[1]=TAXI_EDT_INF;
        }else{
            k++;sites[k]=q;crossings[k]=s;crossings[k+1]=TAXI_EDT_INF;
        }
    }
    k=0;
    for(unsigned q=0;q<n;q++){
        while(crossings[k+1]<(double)q)k++;
        unsigned p=sites[k];double delta=(double)q-p;
        d[q]=delta*delta+f[p];arg[q]=(int)p;
    }
}

static int distance_to_class(const uint8_t *mask,unsigned width,unsigned height,
    unsigned target,double *distance,int *nearest_x,int *nearest_y){
    unsigned pw=width+2,ph=height+2;size_t cells=(size_t)pw*ph;
    int found=0;
    for(size_t i=0;i<cells;i++){
        unsigned x=(unsigned)(i%pw),y=(unsigned)(i/pw);
        unsigned inside=x&&x+1<pw&&y&&y+1<ph?mask[(size_t)(y-1)*width+x-1]:0;
        if(inside==target){found=1;break;}
    }
    if(!found){for(size_t i=0;i<cells;i++){distance[i]=TAXI_EDT_INF;nearest_x[i]=nearest_y[i]=0;}return 0;}

    double *row_distance=malloc(cells*sizeof *row_distance);
    int *row_arg=malloc(cells*sizeof *row_arg);
    double *f=malloc((size_t)(pw>ph?pw:ph)*sizeof *f);
    double *d=malloc((size_t)(pw>ph?pw:ph)*sizeof *d);
    int *arg=malloc((size_t)(pw>ph?pw:ph)*sizeof *arg);
    unsigned *sites=malloc((size_t)(pw>ph?pw:ph)*sizeof *sites);
    double *crossings=malloc((size_t)((pw>ph?pw:ph)+1)*sizeof *crossings);
    if(!row_distance||!row_arg||!f||!d||!arg||!sites||!crossings){
        free(row_distance);free(row_arg);free(f);free(d);free(arg);free(sites);free(crossings);return -1;
    }
    for(unsigned y=0;y<ph;y++){
        for(unsigned x=0;x<pw;x++){
            unsigned inside=x&&x+1<pw&&y&&y+1<ph?mask[(size_t)(y-1)*width+x-1]:0;
            f[x]=inside==target?0.0:TAXI_EDT_INF;
        }
        edt_1d(f,pw,d,arg,sites,crossings);
        for(unsigned x=0;x<pw;x++){row_distance[(size_t)y*pw+x]=d[x];row_arg[(size_t)y*pw+x]=arg[x];}
    }
    for(unsigned x=0;x<pw;x++){
        for(unsigned y=0;y<ph;y++)f[y]=row_distance[(size_t)y*pw+x];
        edt_1d(f,ph,d,arg,sites,crossings);
        for(unsigned y=0;y<ph;y++){
            size_t i=(size_t)y*pw+x;unsigned sy=(unsigned)arg[y];
            distance[i]=d[y];nearest_y[i]=(int)sy;
            nearest_x[i]=row_arg[(size_t)sy*pw+x];
        }
    }
    free(row_distance);free(row_arg);free(f);free(d);free(arg);free(sites);free(crossings);return 0;
}

int taxi_font_sdf_expand_rgba(const uint8_t *source,unsigned width,unsigned height,
    unsigned scale,unsigned edge_width_q8,uint8_t *output,size_t output_bytes,
    unsigned *output_width,unsigned *output_height){
    if(!source||!output||!output_width||!output_height||!width||!height||
       width>256||height>256||scale<2||scale>16||!edge_width_q8||edge_width_q8>256)return -1;
    if(width>UINT32_MAX/scale||height>UINT32_MAX/scale)return -1;
    unsigned ow=width*scale,oh=height*scale;
    if((size_t)ow>SIZE_MAX/oh/4||output_bytes<(size_t)ow*oh*4)return -1;
    unsigned pw=width+2,ph=height+2;size_t cells=(size_t)pw*ph;
    uint8_t *mask=calloc((size_t)width*height,1);
    double *to_inside=malloc(cells*sizeof *to_inside),*to_outside=malloc(cells*sizeof *to_outside);
    int *inside_x=malloc(cells*sizeof *inside_x),*inside_y=malloc(cells*sizeof *inside_y);
    int *outside_x=malloc(cells*sizeof *outside_x),*outside_y=malloc(cells*sizeof *outside_y);
    if(!mask||!to_inside||!to_outside||!inside_x||!inside_y||!outside_x||!outside_y){
        free(mask);free(to_inside);free(to_outside);free(inside_x);free(inside_y);free(outside_x);free(outside_y);return -1;
    }
    unsigned foreground=0;
    for(size_t i=0;i<(size_t)width*height;i++)if(source[i*4+3]>=128){mask[i]=1;foreground++;}
    if(!foreground){memset(output,0,(size_t)ow*oh*4);*output_width=ow;*output_height=oh;free(mask);free(to_inside);free(to_outside);free(inside_x);free(inside_y);free(outside_x);free(outside_y);return 0;}
    if(distance_to_class(mask,width,height,1,to_inside,inside_x,inside_y)||
       distance_to_class(mask,width,height,0,to_outside,outside_x,outside_y)){
        free(mask);free(to_inside);free(to_outside);free(inside_x);free(inside_y);free(outside_x);free(outside_y);return -1;
    }
    double *signed_distance=malloc(cells*sizeof *signed_distance);
    if(!signed_distance){free(mask);free(to_inside);free(to_outside);free(inside_x);free(inside_y);free(outside_x);free(outside_y);return -1;}
    for(unsigned y=0;y<ph;y++)for(unsigned x=0;x<pw;x++){
        size_t i=(size_t)y*pw+x;unsigned inside=x&&x+1<pw&&y&&y+1<ph?mask[(size_t)(y-1)*width+x-1]:0;
        double d2=inside?to_outside[i]:to_inside[i];
        double d=sqrt(d2)-0.5;signed_distance[i]=inside?d:-d;
    }
    double half_width=(double)edge_width_q8/512.0;
    for(unsigned y=0;y<oh;y++)for(unsigned x=0;x<ow;x++){
        double sx=0.5+((double)x+0.5)/scale,sy=0.5+((double)y+0.5)/scale;
        unsigned x0=(unsigned)floor(sx),y0=(unsigned)floor(sy),x1=x0+1<pw?x0+1:x0,y1=y0+1<ph?y0+1:y0;
        double fx=sx-x0,fy=sy-y0;
        double a=signed_distance[(size_t)y0*pw+x0]*(1.0-fx)+signed_distance[(size_t)y0*pw+x1]*fx;
        double b=signed_distance[(size_t)y1*pw+x0]*(1.0-fx)+signed_distance[(size_t)y1*pw+x1]*fx;
        double signed_d=a*(1.0-fy)+b*fy;
        double coverage=0.5+signed_d/(2.0*half_width);
        if(coverage<0.0)coverage=0.0;else if(coverage>1.0)coverage=1.0;
        unsigned px=(unsigned)floor(sx+0.5),py=(unsigned)floor(sy+0.5);
        size_t out=((size_t)y*ow+x)*4;
        if(coverage<=0.0){output[out]=output[out+1]=output[out+2]=output[out+3]=0;continue;}
        if(px>=pw)px=pw-1;if(py>=ph)py=ph-1;size_t feature=(size_t)py*pw+px;
        int cx=inside_x[feature],cy=inside_y[feature];
        if(cx<1||cx>(int)width||cy<1||cy>(int)height){output[out]=output[out+1]=output[out+2]=0;output[out+3]=0;continue;}
        size_t src=((size_t)(cy-1)*width+(unsigned)(cx-1))*4;
        output[out]=source[src];output[out+1]=source[src+1];output[out+2]=source[src+2];
        output[out+3]=(uint8_t)(coverage*source[src+3]+0.5);
    }
    *output_width=ow;*output_height=oh;
    free(mask);free(to_inside);free(to_outside);free(inside_x);free(inside_y);free(outside_x);free(outside_y);free(signed_distance);return 0;
}
