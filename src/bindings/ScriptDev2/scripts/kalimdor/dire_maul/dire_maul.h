/* Copyright (C) 2006 - 2012 ScriptDev2 <http://www.scriptdev2.com/>
 * This program is free software licensed under GPL version 2
 * Please see the included DOCS/LICENSE.TXT for more information */

#ifndef DEF_DM_H
#define DEF_DM_H

enum
{
    MAX_ENCOUNTER               = 12,
    MAX_GENERATORS              = 5,

    // East
    TYPE_ALZZIN                 = 0,                        // Do not change - Handled with Acid
    TYPE_ZEVRIM                 = 1,
    TYPE_IRONBARK               = 2,

    // West
    TYPE_WARPWOOD               = 3,
    TYPE_IMMOLTHAR              = 4,
    TYPE_PRINCE                 = 5,
    TYPE_PYLON_1                = 6,
    TYPE_PYLON_2                = TYPE_PYLON_1 + 1,
    TYPE_PYLON_3                = TYPE_PYLON_1 + 2,
    TYPE_PYLON_4                = TYPE_PYLON_1 + 3,
    TYPE_PYLON_5                = TYPE_PYLON_1 + 4,

    // North
    TYPE_KING_GORDOK            = 11,

    // East
    GO_CRUMBLE_WALL             = 177220,
    GO_CORRUPT_VINE             = 179502,
    GO_FELVINE_SHARD            = 179559,
    GO_CONSERVATORY_DOOR        = 176907,                   // Opened by Ironbark the redeemed

    NPC_ZEVRIM_THORNHOOF        = 11490,
    NPC_OLD_IRONBARK            = 11491,
    NPC_IRONBARK_REDEEMED       = 14241,

    // West
    NPC_TENDRIS_WARPWOOD        = 11489,
    NPC_PRINCE_TORTHELDRIN      = 11486,
    NPC_IMMOLTHAR               = 11496,
    NPC_ARCANE_ABERRATION       = 11480,
    NPC_MANA_REMNANT            = 11483,
    NPC_HIGHBORNE_SUMMONER      = 11466,

    GO_PRINCES_CHEST            = 179545,
    GO_PRINCES_CHEST_AURA       = 179563,
    GO_CRYSTAL_GENERATOR_1      = 177259,
    GO_CRYSTAL_GENERATOR_2      = 177257,
    GO_CRYSTAL_GENERATOR_3      = 177258,
    GO_CRYSTAL_GENERATOR_4      = 179504,
    GO_CRYSTAL_GENERATOR_5      = 179505,
    GO_FORCEFIELD               = 179503,
    GO_WARPWOOD_DOOR            = 177221,

    // North
    NPC_GUARD_MOLDAR            = 14326,
    NPC_STOMPER_KREEG           = 14322,
    NPC_GUARD_FENGUS            = 14321,
    NPC_GUARD_SLIPKIK           = 14323,
    NPC_CAPTAIN_KROMCRUSH       = 14325,
    NPC_CHORUSH                 = 14324,
    NPC_KING_GORDOK             = 11501,
    NPC_MIZZLE_THE_CRAFTY       = 14353,

    GO_KNOTS_CACHE              = 179501,
    GO_KNOTS_BALL_AND_CHAIN     = 179511,
    GO_GORDOK_TRIBUTE           = 179564,

    SAY_FREE_IMMOLTHAR          = -1429000,
    SAY_KILL_IMMOLTHAR          = -1429001,
    SAY_IRONBARK_REDEEM         = -1429002,

    FACTION_HOSTILE             = 14,
    SPELL_KING_OF_GORDOK        = 22799,
};

class MANGOS_DLL_DECL instance_dire_maul : public ScriptedInstance
{
    public:
        instance_dire_maul(Map* pMap);
        ~instance_dire_maul() {}

        void Initialize();

        void OnCreatureCreate(Creature* pCreature);
        void OnObjectCreate(GameObject* pGo);

        void SetData(uint32 uiType, uint32 uiData);
        uint32 GetData(uint32 uiType);

        void OnCreatureEnterCombat(Creature* pCreature);
        void OnCreatureDeath(Creature* pCreature);

        const char* Save() { return m_strInstData.c_str(); }
        void Load(const char* chrIn);

    protected:
        bool CheckAllGeneratorsDestroyed();
        void ProcessForceFieldOpening();
        void SortPylonGuards();
        void PylonGuardJustDied(Creature* pCreature);

        uint32 m_auiEncounter[MAX_ENCOUNTER];
        std::string m_strInstData;

        // East
        bool m_bWallDestroyed;
        GUIDList m_lFelvineShardGUIDs;

        // West
        ObjectGuid m_aCrystalGeneratorGuid[MAX_GENERATORS];

        GUIDList m_luiHighborneSummonerGUIDs;
        GUIDList m_lGeneratorGuardGUIDs;
        std::set<uint32> m_sSortedGeneratorGuards[MAX_GENERATORS];
};

#endif
