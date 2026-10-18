#include "render_viewport.h"
#include <stdio.h>
int main(void){int l,t,r,b;while(scanf("%d%d%d%d",&l,&t,&r,&b)==4){TaxiRenderViewport s={.near_plane=123,.far_plane=456};taxi_render_viewport_set(&s,l,t,r,b);printf("%d %d %d %d %d %d %d %d %d %d %d %d %d",s.left,s.top,s.right,s.bottom,s.center_x,s.center_y,s.width,s.height,s.focal,s.scale_x,s.scale_y,s.near_plane,s.far_plane);for(unsigned i=0;i<2;i++)for(unsigned j=0;j<4;j++)printf(" %d",s.callback_bounds[i][j]);puts("");}return 0;}
