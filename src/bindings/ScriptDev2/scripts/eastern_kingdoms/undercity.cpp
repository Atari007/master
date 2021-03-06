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
SDName: Undercity
SD%Complete: 95
SDComment: Quest support: 9180(post-event).
SDCategory: Undercity
EndScriptData */

/* ContentData
npc_lady_sylvanas_windrunner
npc_parqual_fintallas
EndContentData */

#include "precompiled.h"
#define HIGHBORNE_LOC_Y_NEW         -55.50f

/*######
## npc_lady_sylvanas_windrunner
######*/

enum
{
    EMOTE_LAMENT_START          = -1000193,
    SAY_LAMENT_END              = -1000196,
    EMOTE_LAMENT_END            = -1000197,

    SPELL_HIGHBORNE_AURA        = 37090,
    SPELL_SYLVANAS_CAST         = 36568,
    SPELL_RIBBON_OF_SOULS       = 37099,

    NPC_HIGHBORNE_LAMENTER      = 21628,
    NPC_HIGHBORNE_BUNNY         = 21641,

    QUEST_ID_JOURNEY_UNDERCITY  = 9180,

    MAX_LAMENTERS               = 4,
};

static const float aHighborneLoc[MAX_LAMENTERS][4]=
{
    {1285.41f, 312.47f, -61.0f, 0.51f},
    {1286.96f, 310.40f, -61.0f, 1.00f},
    {1289.66f, 309.66f, -61.0f, 1.52f},
    {1292.51f, 310.50f, -61.0f, 1.99f},
};

struct MANGOS_DLL_DECL npc_lady_sylvanas_windrunnerAI : public ScriptedAI
{
    npc_lady_sylvanas_windrunnerAI(Creature* pCreature) : ScriptedAI(pCreature) { Reset(); }

    uint32 m_uiLamentEventTimer;
    uint32 m_uiSummonTimer;

    void Reset()
    {
        m_uiLamentEventTimer = 0;
        m_uiSummonTimer = 0;
    }

    void JustSummoned(Creature* pSummoned)
    {
        if (pSummoned->GetEntry() == NPC_HIGHBORNE_BUNNY)
            pSummoned->CastSpell(pSummoned, SPELL_RIBBON_OF_SOULS, false);
        else if (pSummoned->GetEntry() == NPC_HIGHBORNE_LAMENTER)
        {
            pSummoned->CastSpell(pSummoned, SPELL_HIGHBORNE_AURA, false);

            pSummoned->SetLevitate(true);
            pSummoned->GetMotionMaster()->MovePoint(0, pSummoned->GetPositionX(), pSummoned->GetPositionY(), pSummoned->GetPositionZ() + 5.0f);
        }
    }

    void DoStartLamentEvent()
    {
        DoScriptText(EMOTE_LAMENT_START, m_creature);
        DoCastSpellIfCan(m_creature, SPELL_SYLVANAS_CAST);
        m_uiSummonTimer = 13000;
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (m_uiLamentEventTimer)
        {
            if (m_uiLamentEventTimer <= uiDiff)
            {
                float fX, fY, fZ;
                m_creature->GetRandomPoint(m_creature->GetPositionX(), m_creature->GetPositionY(), m_creature->GetPositionZ(), 20.0f, fX, fY, fZ);
                m_creature->SummonCreature(NPC_HIGHBORNE_BUNNY, fX, fY, fZ + 15.0f, 0, TEMPSUMMON_TIMED_DESPAWN, 3000);

                m_uiLamentEventTimer = 2000;

                if (!m_creature->HasAura(SPELL_SYLVANAS_CAST))
                {
                    DoScriptText(SAY_LAMENT_END, m_creature);
                    DoScriptText(EMOTE_LAMENT_END, m_creature);
                    m_uiLamentEventTimer = 0;
                }
            }
            else
                m_uiLamentEventTimer -= uiDiff;
        }

        if (m_uiSummonTimer)
        {
            if (m_uiSummonTimer <= uiDiff)
            {
                for (uint8 i = 0; i < MAX_LAMENTERS; ++i)
                    m_creature->SummonCreature(NPC_HIGHBORNE_LAMENTER, aHighborneLoc[i][0], aHighborneLoc[i][1], aHighborneLoc[i][2], aHighborneLoc[i][3], TEMPSUMMON_TIMED_DESPAWN, 160000);

                m_uiLamentEventTimer = 2000;
                m_uiSummonTimer = 0;
            }
            else
                m_uiSummonTimer -= uiDiff;
        }

        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_npc_lady_sylvanas_windrunner(Creature* pCreature)
{
    return new npc_lady_sylvanas_windrunnerAI(pCreature);
}

bool QuestRewarded_npc_lady_sylvanas_windrunner(Player* pPlayer, Creature* pCreature, Quest const* pQuest)
{
    if (pQuest->GetQuestId() == QUEST_ID_JOURNEY_UNDERCITY)
    {
        if (npc_lady_sylvanas_windrunnerAI* pSylvanAI = dynamic_cast<npc_lady_sylvanas_windrunnerAI*>(pCreature->AI()))
            pSylvanAI->DoStartLamentEvent();
    }

    return true;
}

/*######
## npc_highborne_lamenter
######*/


struct MANGOS_DLL_DECL npc_highborne_lamenterAI : public ScriptedAI
{
    npc_highborne_lamenterAI(Creature* pCreature) : ScriptedAI(pCreature) { Reset(); }

    uint32 EventMove_Timer;
    uint32 EventCast_Timer;
    bool EventMove;
    bool EventCast;

    void Reset()
    {
        EventMove_Timer = 10000;
        EventCast_Timer = 17500;
        EventMove = true;
        EventCast = true;
    }

    void UpdateAI(const uint32 diff)
    {
        if (EventMove)
        {
            if (EventMove_Timer < diff)
            {
                m_creature->SetLevitate(true);
                m_creature->MonsterMoveWithSpeed(m_creature->GetPositionX(),m_creature->GetPositionY(),HIGHBORNE_LOC_Y_NEW,3.f);
                EventMove = false;
            }else EventMove_Timer -= diff;
        }
        if (EventCast)
        {
            if (EventCast_Timer < diff)
            {
                DoCastSpellIfCan(m_creature,SPELL_HIGHBORNE_AURA);
                EventCast = false;
            }else EventCast_Timer -= diff;
        }
    }
};
CreatureAI* GetAI_npc_highborne_lamenter(Creature* pCreature)
{
    return new npc_highborne_lamenterAI(pCreature);
}


/*######
## npc_parqual_fintallas
######*/

#define SPELL_MARK_OF_SHAME 6767

bool GossipHello_npc_parqual_fintallas(Player* pPlayer, Creature* pCreature)
{
    if (pCreature->isQuestGiver())
        pPlayer->PrepareQuestMenu(pCreature->GetObjectGuid());

    if (pPlayer->GetQuestStatus(6628) == QUEST_STATUS_INCOMPLETE && !pPlayer->HasAura(SPELL_MARK_OF_SHAME, EFFECT_INDEX_0))
    {
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Gul'dan", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Kel'Thuzad", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Ner'zhul", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+2);
        pPlayer->SEND_GOSSIP_MENU(5822, pCreature->GetObjectGuid());
    }
    else
        pPlayer->SEND_GOSSIP_MENU(5821, pCreature->GetObjectGuid());

    return true;
}

bool GossipSelect_npc_parqual_fintallas(Player* pPlayer, Creature* pCreature, uint32 uiSender, uint32 uiAction)
{
    if (uiAction == GOSSIP_ACTION_INFO_DEF+1)
    {
        pPlayer->CLOSE_GOSSIP_MENU();
        pCreature->CastSpell(pPlayer,SPELL_MARK_OF_SHAME,false);
    }
    if (uiAction == GOSSIP_ACTION_INFO_DEF+2)
    {
        pPlayer->CLOSE_GOSSIP_MENU();
        pPlayer->AreaExploredOrEventHappens(6628);
    }
    return true;
}
void AddSC_undercity()
{
    Script* pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "npc_lady_sylvanas_windrunner";
    pNewScript->GetAI = &GetAI_npc_lady_sylvanas_windrunner;
    pNewScript->pQuestRewardedNPC = &QuestRewarded_npc_lady_sylvanas_windrunner;
    pNewScript->RegisterSelf();

	pNewScript = new Script;
    pNewScript->Name = "npc_highborne_lamenter";
    pNewScript->GetAI = &GetAI_npc_highborne_lamenter;
    pNewScript->RegisterSelf();

	pNewScript = new Script;
    pNewScript->Name = "npc_parqual_fintallas";
    pNewScript->pGossipHello = &GossipHello_npc_parqual_fintallas;
    pNewScript->pGossipSelect = &GossipSelect_npc_parqual_fintallas;
    pNewScript->RegisterSelf();
}
