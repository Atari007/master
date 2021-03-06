/* Copyright (C) 2006 - 2012 ScriptDev2 <http://www.scriptdev2.com/>
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

/* ScriptData
SDName: Bloodmyst_Isle
SD%Complete: 80
SDComment: Quest support: 9670
SDCategory: Bloodmyst Isle
EndScriptData */

/* ContentData
mob_webbed_creature
EndContentData */

#include "precompiled.h"

/*######
## mob_webbed_creature
######*/

enum
{
    NPC_EXPEDITION_RESEARCHER       = 17681,
};

// possible creatures to be spawned (too many to be added to enum)
const uint32 possibleSpawns[31] = {17322, 17661, 17496, 17522, 17340, 17352, 17333, 17524, 17654, 17348, 17339, 17345, 17359, 17353, 17336, 17550, 17330, 17701, 17321, 17325, 17320, 17683, 17342, 17715, 17334, 17341, 17338, 17337, 17346, 17344, 17327};

struct MANGOS_DLL_DECL mob_webbed_creatureAI : public ScriptedAI
{
    mob_webbed_creatureAI(Creature* pCreature) : ScriptedAI(pCreature) { Reset(); }

    void Reset() { }

    void JustDied(Unit* pKiller)
    {
        uint32 uiSpawnCreatureEntry = 0;

        switch(urand(0, 2))
        {
            case 0:
                uiSpawnCreatureEntry = NPC_EXPEDITION_RESEARCHER;
                if (pKiller->GetTypeId() == TYPEID_PLAYER)
                    ((Player*)pKiller)->KilledMonsterCredit(uiSpawnCreatureEntry, m_creature->GetObjectGuid());
                break;
            case 1:
            case 2:
                uiSpawnCreatureEntry = possibleSpawns[urand(0, 30)];
                break;
        }

        if (uiSpawnCreatureEntry)
            m_creature->SummonCreature(uiSpawnCreatureEntry, 0.0f, 0.0f, 0.0f, m_creature->GetOrientation(), TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 25000);
    }
};

CreatureAI* GetAI_mob_webbed_creature(Creature* pCreature)
{
    return new mob_webbed_creatureAI(pCreature);
}

/*######
## npc_captured_sunhawk_agent
######*/

#define C_SUNHAWK_TRIGGER 17974

bool GossipHello_npc_captured_sunhawk_agent(Player* pPlayer, Creature* pCreature)
{
    if (pPlayer->HasAura(31609, EFFECT_INDEX_1) && pPlayer->GetQuestStatus(9756) == QUEST_STATUS_INCOMPLETE)
    {
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "[PH] ", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);
        pPlayer->SEND_GOSSIP_MENU(9136, pCreature->GetObjectGuid());
    }
    else
        pPlayer->SEND_GOSSIP_MENU(9134, pCreature->GetObjectGuid());

    return true;
}

bool GossipSelect_npc_captured_sunhawk_agent(Player* pPlayer, Creature* pCreature, uint32 uiSender, uint32 uiAction)
{
    switch(uiAction)
    {
        case GOSSIP_ACTION_INFO_DEF+1:
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "[PH] ", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+2);
            pPlayer->SEND_GOSSIP_MENU(9137, pCreature->GetObjectGuid());
            break;
        case GOSSIP_ACTION_INFO_DEF+2:
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "[PH] ", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+3);
            pPlayer->SEND_GOSSIP_MENU(9138, pCreature->GetObjectGuid());
            break;
        case GOSSIP_ACTION_INFO_DEF+3:
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "[PH] ", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+4);
            pPlayer->SEND_GOSSIP_MENU(9139, pCreature->GetObjectGuid());
            break;
        case GOSSIP_ACTION_INFO_DEF+4:
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "[PH] ", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+5);
            pPlayer->SEND_GOSSIP_MENU(9140, pCreature->GetObjectGuid());
            break;
        case GOSSIP_ACTION_INFO_DEF+5:
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "[PH] ", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+6);
            pPlayer->SEND_GOSSIP_MENU(9141, pCreature->GetObjectGuid());
            break;
        case GOSSIP_ACTION_INFO_DEF+6:
            pPlayer->CLOSE_GOSSIP_MENU();
            pPlayer->TalkedToCreature(C_SUNHAWK_TRIGGER, pCreature->GetObjectGuid());
            break;
    }
    return true;
}

void AddSC_bloodmyst_isle()
{
    Script* pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "mob_webbed_creature";
    pNewScript->GetAI = &GetAI_mob_webbed_creature;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_captured_sunhawk_agent";
    pNewScript->pGossipHello =  &GossipHello_npc_captured_sunhawk_agent;
    pNewScript->pGossipSelect = &GossipSelect_npc_captured_sunhawk_agent;
    pNewScript->RegisterSelf();
}
