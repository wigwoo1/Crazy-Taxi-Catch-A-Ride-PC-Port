#ifdef NDEBUG
#undef NDEBUG
#endif
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "../src/profile_choice.h"
#include "../src/name_entry.h"
int main(int argc,char **argv) {
    assert(argc==2);uint8_t chars[43];assert(!taxi_name_characters_load(chars,argv[1]));
    TaxiMenuInput in={0};in.pressed[0]=1;
    for(unsigned flags=0;flags<4;flags++) {
        int first=flags&1,second=flags&2;unsigned count=taxi_profile_option_count(first,second);
        assert(count==(flags?3:2));
        for(unsigned option=0;option<count;option++) {
            uint8_t names[2][3]={{'B','O','B'},{'S','U','E'}};
            TaxiProfileChoice s;taxi_profile_choice_begin(&s,first,second,1);
            assert(s.option==(flags==3?1:0));s.option=(uint8_t)option;
            unsigned e=taxi_profile_choice_step(&s,names,chars,first,second,&in,0);
            if(option==count-1)assert(e==TAXI_PROFILE_FADE_OUT&&s.exiting==2&&!s.choice);
            else if(flags==3||(flags&&option==0)) {
                unsigned choice=flags==3?option+1:(first?1:2);
                assert(e==TAXI_PROFILE_FADE_OUT&&s.exiting==1&&s.choice==choice&&s.current==choice-1&&!s.new_profile);
            } else {
                assert(e==TAXI_PROFILE_EDIT_BEGIN&&s.editing&&s.choice==(first?2:1));
                assert(!memcmp(names[s.current],"AAA",3));
                assert(taxi_profile_choice_step(&s,names,chars,first,second,&in,0)==TAXI_PROFILE_FADE_OUT);
                assert(s.new_profile&&s.exiting==1);
            }
            TaxiProfileChoice saved=s;assert(!taxi_profile_choice_step(&s,names,chars,first,second,&in,1));assert(!memcmp(&s,&saved,sizeof s));
        }
    }
    TaxiProfileChoice s;uint8_t names[2][3]={{'A','A','A'},{'A','A','A'}};
    taxi_profile_choice_begin(&s,0,0,0);
    assert(taxi_profile_choice_step(&s,names,chars,0,0,&in,0)==TAXI_PROFILE_EDIT_BEGIN);
    assert(taxi_profile_choice_step(&s,names,chars,0,0,&in,0)==TAXI_PROFILE_DUPLICATE&&!s.exiting);
    memset(&in,0,sizeof in);in.pressed[1]=1;
    assert(taxi_profile_choice_step(&s,names,chars,0,0,&in,0)==TAXI_PROFILE_CANCEL_EDIT&&s.choice==1);
    memset(&in,0,sizeof in);in.pressed[6]=in.pressed[7]=in.pressed[0]=1;
    assert(taxi_profile_choice_step(&s,names,chars,0,0,&in,0)==TAXI_PROFILE_MOVE&&s.option==1&&!s.exiting);
    in.pressed[6]=in.pressed[7]=0;
    assert(taxi_profile_choice_step(&s,names,chars,0,0,&in,0)==TAXI_PROFILE_FADE_OUT&&s.exiting==2&&s.choice==1);
    puts("Native profile choice: all existence/option cases, original profile ownership, fresh navigation priority, edit/duplicate/cancel and skip persistence gate passed; UI/commit pending.");return 0;
}
