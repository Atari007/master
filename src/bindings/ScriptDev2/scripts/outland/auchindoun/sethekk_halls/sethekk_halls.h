/* Copyright (C) 2006 - 2012 ScriptDev2 <http://www.scriptdev2.com/>
 * This program is free software licensed under GPL version 2
 * Please see the included DOCS/LICENSE.TXT for more information */

#ifndef DEF_SETHEKK_HALLS_H
#define DEF_SETHEKK_HALLS_H

enum
{
    MAX_ENCOUNTER               = 3,

    TYPE_SYTH                   = 0,
    TYPE_ANZU                   = 1,
    TYPE_IKISS                  = 2,

    NPC_ANZU                    = 23035,
    NPC_RAVEN_GOD_TARGET        = 23057,

    GO_IKISS_DOOR               = 177203,
    GO_IKISS_CHEST              = 187372,
    GO_RAVENS_CLAW              = 185554,

    SAY_ANZU_INTRO_1            = -1556016,
    SAY_ANZU_INTRO_2            = -1556017,

    // possible spells used for Anzu summoning event
    SPELL_PORTAL                = 39952,
    SPELL_SUMMONING_BEAMS       = 39978,
    SPELL_RED_LIGHTNING         = 39990,
};

class MANGOS_DLL_DECL instance_sethekk_halls : public ScriptedInstance
{
    public:
        instance_sethekk_halls(Map* pMap);
        ~instance_sethekk_halls() {}

        void Initialize();

        void OnCreatureCreate(Creature* pCreature);
        void OnObjectCreate(GameObject* pGo);

        void SetData(uint32 uiType, uint32 uiData);
        uint32 GetData(uint32 uiType);

        const char* Save() { return m_strInstData.c_str(); }
        void Load(const char* chrIn);

    private:
        uint32 m_auiEncounter[MAX_ENCOUNTER];
        std::string m_strInstData;
};

#endif
