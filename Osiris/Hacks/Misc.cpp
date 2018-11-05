#include <string_view>

#include "../Config.h"
#include "../Interfaces.h"
#include "../Memory.h"
#include "Misc.h"

void Misc::bunnyHop(UserCmd* cmd)
{
    if (config.misc.bunnyHop) {
        constexpr int ON_GROUND{ 1 };
        constexpr int IN_JUMP{ 2 };

        static auto jumped = false;
        static auto fake = false;
        if (!jumped && fake) {
            fake = false;
            cmd->buttons |= IN_JUMP;
        }
        else if (cmd->buttons & IN_JUMP) {
            if (*((*memory.localPlayer)->getFlags()) & ON_GROUND) {
                jumped = true;
                fake = true;
            }
            else {
                cmd->buttons &= ~IN_JUMP;
                jumped = false;
            }
        }
        else {
            jumped = false;
            fake = false;
        }
    }
}

void Misc::disablePostProcessing()
{
    if (*memory.disablePostProcessing != config.misc.disablePostProcessing)
        *memory.disablePostProcessing = config.misc.disablePostProcessing;
}

void Misc::skybox()
{
    if (config.misc.skybox != xorstr_("Default"))
        reinterpret_cast<void(__fastcall*)(const char*)>(memory.loadSky)(config.misc.skybox.c_str());
}

void Misc::clanTag()
{
    static std::string_view clanTag = config.misc.clanTag;
    if (clanTag != config.misc.clanTag) {
        reinterpret_cast<void(__fastcall*)(const char*, const char*)>(memory.setClanTag)(config.misc.clanTag.c_str(), "");
        clanTag = config.misc.clanTag;
    }
}

void Misc::colorWorld()
{
    static auto red = interfaces.cvar->FindVar(xorstr_("mat_ambient_light_r"));
    static auto green = interfaces.cvar->FindVar(xorstr_("mat_ambient_light_g"));
    static auto blue = interfaces.cvar->FindVar(xorstr_("mat_ambient_light_b"));

    red->SetValue(config.misc.worldColor[0]);
    green->SetValue(config.misc.worldColor[1]);
    blue->SetValue(config.misc.worldColor[2]);
}

void Misc::reduceFlashEffect()
{
    *reinterpret_cast<float*>((*memory.localPlayer) + 0xA328) = 255.0f - static_cast<float>(config.misc.flashReduction);
}

void Misc::removeSmoke(ClientFrameStage stage)
{
    static const std::string smoke_materials[]{
        xorstr_("particle/beam_smoke_01"),
        xorstr_("particle/particle_smokegrenade"),
        xorstr_("particle/particle_smokegrenade1"),
        xorstr_("particle/particle_smokegrenade2"),
        xorstr_("particle/particle_smokegrenade3"),
        xorstr_("particle/particle_smokegrenade_sc"),
        xorstr_("particle/smoke1/smoke1"),
        xorstr_("particle/smoke1/smoke1_ash"),
        xorstr_("particle/smoke1/smoke1_nearcull"),
        xorstr_("particle/smoke1/smoke1_nearcull2"),
        xorstr_("particle/smoke1/smoke1_snow"),
        xorstr_("particle/smokesprites_0001"),
        xorstr_("particle/smokestack"),
        xorstr_("particle/vistasmokev1/vistasmokev1"),
        xorstr_("particle/vistasmokev1/vistasmokev1_emods"),
        xorstr_("particle/vistasmokev1/vistasmokev1_emods_impactdust"),
        xorstr_("particle/vistasmokev1/vistasmokev1_fire"),
        xorstr_("particle/vistasmokev1/vistasmokev1_nearcull"),
        xorstr_("particle/vistasmokev1/vistasmokev1_nearcull_fog"),
        xorstr_("particle/vistasmokev1/vistasmokev1_nearcull_nodepth"),
        xorstr_("particle/vistasmokev1/vistasmokev1_smokegrenade"),
        xorstr_("particle/vistasmokev1/vistasmokev4_emods_nocull"),
        xorstr_("particle/vistasmokev1/vistasmokev4_nearcull"),
        xorstr_("particle/vistasmokev1/vistasmokev4_nocull")
    };

    if (stage == ClientFrameStage::FRAME_NET_UPDATE_START)
    {
        for (auto material_name : smoke_materials)
        {
            Material* mat = interfaces.materialSystem->findMaterial(material_name.c_str(), xorstr_("Other textures"));
            mat->setMaterialVarFlag(MaterialVar::NO_DRAW, config.misc.noSmoke);
        }
    }
}
