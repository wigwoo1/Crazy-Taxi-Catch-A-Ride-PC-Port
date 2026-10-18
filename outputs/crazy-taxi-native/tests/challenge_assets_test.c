#ifdef NDEBUG
#undef NDEBUG
#endif
#include "../src/challenge_assets.h"
#include <assert.h>
#include <stdio.h>
int main(void){TaxiChallengeAssets a={0};
for(unsigned mode=0;mode<9;mode++){
 assert(!taxi_challenge_assets_load(&a,"outputs/crazy-taxi-native/assets/challenge-worlds",mode));
 assert(a.submode==mode&&a.world.nv&&a.textures.count==45);
 TaxiGroundHit hit;assert(taxi_world_asset_ground(&a.world,a.spawn.query,&hit)==1);
 uint8_t *prior=a.world.storage;assert(taxi_challenge_assets_load(&a,"missing",mode)==-1&&a.world.storage==prior);
 assert(taxi_challenge_assets_load(&a,"outputs/crazy-taxi-native/assets/challenge-worlds",9)==-1&&a.world.storage==prior);
}
taxi_challenge_assets_free(&a);assert(!a.world.storage&&!a.textures.storage);puts("PASS9 complete native challenge resource replacements, original spawn ground hits and rollback.");return 0;}

