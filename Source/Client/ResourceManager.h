#pragma once

#include "Common.h"
#include "GraphicStructures.h"

#define RES_ATLAS_STATIC (1)
#define RES_ATLAS_DYNAMIC (2)
#define RES_ATLAS_SPLASH (3)
#define RES_ATLAS_TEXTURES (4)

#define SKILLDEX_PARAM(index) (index)
#define SKILLDEX_PERKS (1000)
#define SKILLDEX_KILLS (1001)
#define SKILLDEX_KARMA (1002)
#define SKILLDEX_TRAITS (1003)
#define SKILLDEX_REPUTATION (1004)
#define SKILLDEX_SKILLS (1005)
#define SKILLDEX_NEXT_LEVEL (1006)
#define SKILLDEX_DRUG_ADDICT (1007)
#define SKILLDEX_ALCOHOL_ADDICT (1008)
#define SKILLDEX_REPUTATION_RATIO(val) \
    (2100 + \
        ((val) >= GameOpt.ReputationLoved ? \
                0 : \
                ((val) >= GameOpt.ReputationLiked ? \
                        1 : \
                        ((val) >= GameOpt.ReputationAccepted ? \
                                2 : \
                                ((val) >= GameOpt.ReputationNeutral ? \
                                        3 : \
                                        ((val) >= GameOpt.ReputationAntipathy ? \
                                                4 : \
                                                ((val) >= GameOpt.ReputationHated ? 5 : 6)))))))

class SpriteManager;
struct SpriteInfo;
struct AnyFrames;
class IDataFile;
using DataFile = std::shared_ptr<IDataFile>;
using DataFileVec = vector<DataFile>;
class Animation3d;

struct LoadedAnim
{
    int ResType;
    AnyFrames* Anim;
};
typedef map<hash, LoadedAnim> LoadedAnimMap;

class ResourceManager : public NonCopyable
{
public:
    ResourceManager(SpriteManager& spr_mngr);
    ~ResourceManager();

    void Refresh();
    void FreeResources(int type);
    void ReinitializeDynamicAtlas();

    AnyFrames* GetAnim(hash name_hash, int res_type);
    AnyFrames* GetIfaceAnim(hash name_hash) { return GetAnim(name_hash, RES_ATLAS_STATIC); }
    AnyFrames* GetInvAnim(hash name_hash) { return GetAnim(name_hash, RES_ATLAS_STATIC); }
    AnyFrames* GetSkDxAnim(hash name_hash) { return GetAnim(name_hash, RES_ATLAS_STATIC); }
    AnyFrames* GetItemAnim(hash name_hash) { return GetAnim(name_hash, RES_ATLAS_DYNAMIC); }

    AnyFrames* GetCrit2dAnim(hash model_name, uint anim1, uint anim2, int dir);
    Animation3d* GetCrit3dAnim(hash model_name, uint anim1, uint anim2, int dir, int* layers3d = nullptr);
    uint GetCritSprId(hash model_name, uint anim1, uint anim2, int dir, int* layers3d = nullptr);

    AnyFrames* GetRandomSplash();

    StrMap& GetSoundNames() { return soundNames; }

    AnyFrames* ItemHexDefaultAnim;
    AnyFrames* CritterDefaultAnim;

private:
    void FixAnimOffs(AnyFrames* frames_base, AnyFrames* stay_frm_base);
    void FixAnimOffsNext(AnyFrames* frames_base, AnyFrames* stay_frm_base);

    SpriteManager& sprMngr;
    DataFileVec processedDats;
    UIntStrMap namesHash;
    LoadedAnimMap loadedAnims;
    AnimMap critterFrames;
    map<hash, Animation3d*> critter3d;
    StrVec splashNames;
    StrMap soundNames;

    AnyFrames* LoadFalloutAnim(hash model_name, uint anim1, uint anim2);
    AnyFrames* LoadFalloutAnimSpr(hash model_name, uint anim1, uint anim2);
};
