#ifdef NDEBUG
#undef NDEBUG
#endif
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "../src/profile_edit.h"
#include "../src/name_entry.h"
int main(int argc,char **argv) {
    assert(argc==2);uint8_t chars[43];assert(!taxi_name_characters_load(chars,argv[1]));
    uint8_t names[2][3]={{'A','A','A'},{'A','A','A'}};TaxiProfileEdit s={0};
    assert(taxi_profile_edit_step(&s,names,chars,0,0,0,0,1,1,0)==TAXI_PROFILE_EDIT_DUPLICATE);
    for(unsigned i=1;i<=42;i++) {taxi_profile_edit_step(&s,names,chars,0,0,1,0,0,0,0);assert(s.character==i%42&&names[0][0]==chars[i%42]);}
    taxi_profile_edit_step(&s,names,chars,0,0,0,1,0,0,0);assert(s.character==41&&names[0][0]==' ');
    assert(taxi_profile_edit_step(&s,names,chars,0,0,0,0,1,1,0)==TAXI_PROFILE_EDIT_ACCEPT);
    assert(names[0][0]==' '); /* Confirm takes precedence over back. */
    taxi_profile_edit_step(&s,names,chars,1,1,0,0,0,0,0);assert(s.column==1&&s.character==0);
    taxi_profile_edit_step(&s,names,chars,0,1,0,0,0,0,0);assert(s.column==0&&s.character==41);
    taxi_profile_edit_step(&s,names,chars,0,1,1,1,0,0,0);assert(s.column==2&&s.character==1&&names[0][2]=='B');
    assert(taxi_profile_edit_step(&s,names,chars,0,0,0,0,0,1,0)==TAXI_PROFILE_EDIT_CANCEL);
    assert(!memcmp(names[0],"AAA",3));
    s=(TaxiProfileEdit){0,0,1};taxi_profile_edit_step(&s,names,chars,0,0,1,0,0,0,0);
    assert(names[1][0]=='B'&&names[0][0]=='A');
    TaxiProfileEdit prior=s;uint8_t saved[2][3];memcpy(saved,names,6);
    assert(!taxi_profile_edit_step(&s,names,chars,1,1,1,1,1,1,1));
    assert(!memcmp(&s,&prior,sizeof s)&&!memcmp(saved,names,6));
    puts("Native profile edit:42 characters, position wrap, original repeat priority, duplicate rejection, confirm/cancel precedence, two-profile ownership and fade gate passed; selection/persistence pending.");return 0;
}
