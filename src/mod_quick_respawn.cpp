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

        AreaTriggerTeleport customRepopLocation;
        AreaTriggerTeleport const *repopLocation;

        // Exceptions for specific maps
        switch (map->GetId())
        {
            // Eye of Eternity: Use custom location, otherwise player gets stuck dead inside the instance
            case 616:
                LOG_INFO("module", "mod_quick_respawn: Using custom repop location for map 616");
                customRepopLocation.target_mapId = 571;
                customRepopLocation.target_X = 3863.902588;
                customRepopLocation.target_Y = 6987.497559;
                customRepopLocation.target_Z = 152.042084;
                customRepopLocation.target_Orientation = 5.820590;
                break;
            
            default:
                LOG_INFO("module", "mod_quick_respawn: Determining repopping location for map {}", map->GetId());
                repopLocation = sObjectMgr->GetGoBackTrigger(map->GetId());

                if (!repopLocation) {
                    repopLocation = sObjectMgr->GetMapEntranceTrigger(map->GetId());
                }

                break;
        }

        if (customRepopLocation.target_mapId > 0) 
        {
            repopLocation = &customRepopLocation;
        }
    
        if (repopLocation)
        {
            LOG_INFO("module", "mod_quick_respawn: Custom repop location found, teleporting player");
            player->TeleportTo(
                repopLocation->target_mapId,
                repopLocation->target_X,
                repopLocation->target_Y,
                repopLocation->target_Z,
                repopLocation->target_Orientation);

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
