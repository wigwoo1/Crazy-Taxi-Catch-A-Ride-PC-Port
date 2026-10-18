#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <xinput.h>
#include <string.h>
#include "xbox.h"
int16_t taxi_xbox_steering_command(int16_t lx){
    int32_t magnitude=lx<0?-(int32_t)lx:lx;
    if(magnitude<=7849)return 0;
    int32_t maximum=lx<0?32768:32767;
    /* Original consumer ignores magnitudes<=7. Preserve a smooth useful
       8..192 range outside the native controller dead zone. */
    int32_t command=8+(magnitude-7850)*184/(maximum-7850);
    return (int16_t)(lx<0?command:-command);
}

TaxiXboxSample taxi_xbox_poll(unsigned controller_index) {
    typedef DWORD (WINAPI *GetStateFn)(DWORD, XINPUT_STATE *);
    static HMODULE module;
    static GetStateFn get_state;
    static int initialized;
    TaxiXboxSample result = {0, 0x3ff, 0, 0, 0, 0};
    if (!initialized) {
        initialized = 1;
        module = LoadLibraryExW(L"xinput1_4.dll", NULL, LOAD_LIBRARY_SEARCH_SYSTEM32);
        if (!module) module = LoadLibraryExW(L"xinput9_1_0.dll", NULL, LOAD_LIBRARY_SEARCH_SYSTEM32);
        if (module) {
            FARPROC symbol = GetProcAddress(module, "XInputGetState");
            _Static_assert(sizeof(symbol) == sizeof(get_state), "Windows function pointer size mismatch");
            memcpy(&get_state, &symbol, sizeof(get_state));
        }
    }
    XINPUT_STATE state = {0};
    if (!get_state || controller_index > 3 || get_state(controller_index, &state) != ERROR_SUCCESS)
        return result;
    return taxi_xbox_decode(state.Gamepad.wButtons,state.Gamepad.sThumbLX,state.Gamepad.sThumbLY,
        state.Gamepad.bRightTrigger,state.Gamepad.bLeftTrigger);
}
TaxiXboxSample taxi_xbox_decode(uint16_t buttons,int16_t lx,int16_t ly,uint8_t rt,uint8_t lt){
    TaxiXboxSample result={0};
    result.connected = 1;
    result.steering = lx;
    result.accelerator = rt;
    result.brake = lt;
    result.look_back=(uint8_t)!!(buttons&XINPUT_GAMEPAD_RIGHT_THUMB);
    uint16_t held = 0;
    /* Baseline physical GBA mapping, not a verified driving-action mapping. */
    if (buttons & XINPUT_GAMEPAD_A) held |= 1;
    if (buttons & XINPUT_GAMEPAD_B) held |= 2;
    if (buttons & XINPUT_GAMEPAD_BACK) held |= 4;
    if (buttons & XINPUT_GAMEPAD_START) held |= 8;
    if ((buttons & XINPUT_GAMEPAD_DPAD_RIGHT) || lx > XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE) held |= 16;
    if ((buttons & XINPUT_GAMEPAD_DPAD_LEFT) || lx < -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE) held |= 32;
    if ((buttons & XINPUT_GAMEPAD_DPAD_UP) || ly > XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE) held |= 64;
    if ((buttons & XINPUT_GAMEPAD_DPAD_DOWN) || ly < -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE) held |= 128;
    if (buttons & XINPUT_GAMEPAD_RIGHT_SHOULDER) held |= 256;
    if (buttons & XINPUT_GAMEPAD_LEFT_SHOULDER) held |= 512;
    result.active_low_keys = (uint16_t)(0x3ff ^ held);
    return result;
}

int16_t taxi_xbox_vehicle_steering_command(int16_t lx,uint8_t phase,int32_t phase_timer){
 int16_t command=taxi_xbox_steering_command(lx);
 return command&&phase==1&&phase_timer?0:command;
}

uint8_t taxi_xbox_camera_command(uint8_t original,uint8_t look_back){return (uint8_t)((original&~1u)|(look_back?1u:0u));}
uint16_t taxi_xbox_driving_keys(TaxiXboxSample sample,uint8_t profile){
 if(!sample.connected)return 0x3ff;
 uint16_t held=(uint16_t)(sample.active_low_keys^0x3ff);
 if(profile<3){
  if(sample.accelerator>30)held|=(uint16_t)(profile==2?256:1);
  if(sample.brake>30)held|=(uint16_t)(profile==2?512:2);
 }
 return (uint16_t)(held^0x3ff);
}

unsigned taxi_xbox_select_slot(unsigned mask,unsigned current){
 mask&=15u;
 if(current<4&&(mask&(1u<<current)))return current;
 for(unsigned i=0;i<4;i++)if(mask&(1u<<i))return i;
 return 4;
}
TaxiXboxSample taxi_xbox_poll_any(void){
 static unsigned current=4;
 if(current<4){TaxiXboxSample active=taxi_xbox_poll(current);if(active.connected)return active;}
 TaxiXboxSample samples[4];unsigned mask=0;
 for(unsigned i=0;i<4;i++){samples[i]=taxi_xbox_poll(i);if(samples[i].connected)mask|=1u<<i;}
 current=taxi_xbox_select_slot(mask,current);
 return current<4?samples[current]:(TaxiXboxSample){.active_low_keys=0x3ff};
}
