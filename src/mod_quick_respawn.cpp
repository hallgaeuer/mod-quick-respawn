#include "ScriptMgr.h"
#include "ObjectMgr.h"
#include "Player.h"
#include "Config.h"
#include "Chat.h"

// Add player scripts
class QuickRespawn_Player : public PlayerScript
{
public:
    QuickRespawn_Player() : PlayerScript("QuickRespawn_Player") {}

    bool CanRepopAtGraveyard(Player *player) override
    {
        if (!sConfigMgr->GetOption<bool>("QuickRespawn.Enable", false))
        {
            return true;
        }

        Map *map = player->GetMap();

        if (!map->IsDungeon() && !map->IsRaid()) {
            return true;
        }

        if (map->IsBattlegroundOrArena()) {
            return true;
        }

        LOG_INFO("module", "mod_quick_respawn: Determining repopping location for map {}", map->GetId());
        AreaTriggerTeleport const *customRepopLocation = sObjectMgr->GetGoBackTrigger(map->GetId());

        if (!customRepopLocation) {
            customRepopLocation = sObjectMgr->GetMapEntranceTrigger(map->GetId());
        }

        if (customRepopLocation)
        {
            LOG_INFO("module", "mod_quick_respawn: Custom repop location found, teleporting player");
            player->TeleportTo(
                customRepopLocation->target_mapId,
                customRepopLocation->target_X,
                customRepopLocation->target_Y,
                customRepopLocation->target_Z,
                customRepopLocation->target_Orientation);

            player->RemovePlayerFlag(PLAYER_FLAGS_IS_OUT_OF_BOUNDS);
            return false;
        }

        LOG_INFO("module", "mod_quick_respawn: No custom repop location found. Repopping normally");

        return true;
    }
};

// Add all scripts in one
void AddQuickRespawnScripts()
{
    new QuickRespawn_Player();
}
