#include "../src/results_state.h"
#undef NDEBUG
#include <assert.h>
#include <string.h>
#include <stdio.h>
int main(void) {
    char cash[32];assert(!taxi_results_cash_format(cash,0,0)&&!strcmp(cash,"0.00"));
    assert(!taxi_results_cash_format(cash,999,1)&&!strcmp(cash,"999.01"));
    assert(!taxi_results_cash_format(cash,1000,99)&&!strcmp(cash,"1,000.99"));
    assert(!taxi_results_cash_format(cash,2147483647,0)&&!strcmp(cash,"2,147,483,647.00"));
    assert(!taxi_results_cash_format(cash,0,100)&&cash[2]==':'); /* No fabricated carry. */
    TaxiResultsState money;assert(!taxi_results_state_begin_money(&money,1234,56));
    assert(!strcmp(money.target,"1,234.56"));
    TaxiResultsState s;assert(!taxi_results_state_begin(&s,"1,234.56"));
    TaxiResultsState before=s;assert(!taxi_results_state_step(&s,100,2,1,1));assert(!memcmp(&s,&before,sizeof s));
    for(int i=0;i<13;i++)taxi_results_state_step(&s,1,2,1,0);
    assert(s.customers==1&&s.phase==0);
    assert(taxi_results_state_step(&s,1,2,1,0)==TAXI_RESULTS_TICK&&s.phase==1);
    for(int i=0;i<600&&s.phase==1;i++)taxi_results_state_step(&s,1,2,1,0);
    assert(s.phase==2&&!strcmp(s.cash,"1,234.56"));
    for(int i=0;i<60;i++)taxi_results_state_step(&s,1,2,1,0);
    assert(s.phase==2);taxi_results_state_step(&s,1,2,1,0);assert(s.phase==3);
    for(int i=0;i<34;i++)assert(!taxi_results_state_step(&s,1,2,1,0));
    assert(taxi_results_state_step(&s,1,2,1,0)==TAXI_RESULTS_CLASS&&s.class_scale==256&&s.phase==4);
    for(int i=0;i<61;i++)taxi_results_state_step(&s,1,2,1,0);
    for(int i=0;i<15;i++)assert(!taxi_results_state_step(&s,1,2,1,0));
    assert(taxi_results_state_step(&s,1,2,1,0)==TAXI_RESULTS_RANK&&s.phase==6);
    for(int i=0;i<300;i++)assert(!taxi_results_state_step(&s,1,2,1,0));
    assert(s.rank==1&&!s.exiting);
    assert(taxi_results_state_step(&s,1,2,1,0)==TAXI_RESULTS_FADE_OUT);
    assert(!taxi_results_state_step(&s,1,2,1,0));
    puts("Original results stages: fade gate, customer count, right-to-left cash, class/rank shrink/holds and single exit fade passed.");
    return 0;
}
