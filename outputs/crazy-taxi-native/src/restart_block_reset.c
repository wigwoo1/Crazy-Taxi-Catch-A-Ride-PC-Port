#include "restart_block_reset.h"
#include <string.h>
int taxi_restart_block_reset(TaxiRestartBlockReset *s){if(!s)return -1;s->word_1768=-1;s->word_176c=0;memset(s->block_2b8ec,0,sizeof s->block_2b8ec);return 0;}
