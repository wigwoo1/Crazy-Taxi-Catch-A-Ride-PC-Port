#include "ordinary_loading_buffers.h"
static int resolve(const TaxiLoadingFrameBuffers *b,uint32_t id,uint8_t **pixels,size_t *size){
 for(unsigned i=0;i<2;i++)if(b->identity[i]==id){if(!b->pixels[i]||b->size[i]<38400)return -1;*pixels=b->pixels[i];*size=b->size[i];return 0;}return -1;
}
int taxi_ordinary_loading_buffers(TaxiLoadingPresent *s,const TaxiOrdinaryLoadingOwner *loading,TaxiPresentationService *service,TaxiOrdinaryPresentationOwner *presentation,const TaxiLoadingFrameBuffers *buffers){
 if(!s||!loading||!loading->frame||!service||!buffers||buffers->identity[0]==buffers->identity[1]||buffers->pixels[0]==buffers->pixels[1])return -1;
 uint8_t *first,*second;size_t first_size,second_size;
 if(resolve(buffers,service->first_754c,&first,&first_size)||resolve(buffers,service->second_7550,&second,&second_size))return -1;
 (void)first;(void)first_size;*loading->frame=second;TaxiOrdinaryLoadingOwner bound=*loading;bound.frame_size=second_size;
 int result=taxi_ordinary_loading_frame(s,&bound,service,presentation);
 size_t size;if(resolve(buffers,service->second_7550,loading->frame,&size))return -1;return result;
}

int taxi_ordinary_loading_pending_buffers(TaxiOrdinaryLoadingPending *p,const TaxiLoadingFrameBuffers *b){
 if(!p||p->phase!=TAXI_LOADING_IDLE||!p->loading||!p->loading->frame||
    p->loading->frame_size<38400||!p->service||!b||
    b->identity[0]==b->identity[1]||b->pixels[0]==b->pixels[1])return -1;
 uint8_t *first,*second;size_t first_size,second_size;
 if(resolve(b,p->service->first_754c,&first,&first_size)||
    resolve(b,p->service->second_7550,&second,&second_size)||
    first==second||first_size<p->loading->frame_size||second_size<p->loading->frame_size)return -1;
 *p->loading->frame=second;return 0;
}
