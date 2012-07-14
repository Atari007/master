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
SDName: Blades_Edge_Mountains
SD%Complete: 90
SDComment: Quest support: 10503, 10504, 10556, 10609. (npc_daranelle needs bit more work before consider complete)
SDCategory: Blade's Edge Mountains
EndScriptData */

/* ContentData
mobs_bladespire_ogre
mobs_nether_drake
npc_daranelle
EndContentData */

#include "precompiled.h"

/*######
## mobs_bladespire_ogre
######*/

//TODO: add support for quest 10512 + creature abilities
struct MANGOS_DLL_DECL mobs_bladespire_ogreAI : public ScriptedAI
{
    mobs_bladespire_ogreAI(Creature* pCreature) : ScriptedAI(pCreature) {Reset();}

    void Reset() { }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_mobs_bladespire_ogre(Creature* pCreature)
{
    return new mobs_bladespire_ogreAI(pCreature);
}

/*######
## mobs_nether_drake
######*/

#define SAY_NIHIL_1                 -1000169
#define SAY_NIHIL_2                 -1000170
#define SAY_NIHIL_3                 -1000171
#define SAY_NIHIL_4                 -1000172
#define SAY_NIHIL_INTERRUPT         -1000173

#define ENTRY_WHELP                 20021
#define ENTRY_PROTO                 21821
#define ENTRY_ADOLE                 21817
#define ENTRY_MATUR                 21820
#define ENTRY_NIHIL                 21823

#define SPELL_T_PHASE_MODULATOR     37573

#define SPELL_ARCANE_BLAST          38881
#define SPELL_MANA_BURN             38884
#define SPELL_INTANGIBLE_PRESENCE   36513

struct MANGOS_DLL_DECL mobs_nether_drakeAI : public ScriptedAI
{
    mobs_nether_drakeAI(Creature* pCreature) : ScriptedAI(pCreature) { Reset(); }

    bool IsNihil;
    uint32 NihilSpeech_Timer;
    uint32 NihilSpeech_Phase;

    uint32 ArcaneBlast_Timer;
    uint32 ManaBurn_Timer;
    uint32 IntangiblePresence_Timer;

    void Reset()
    {
        IsNihil = false;
        NihilSpeech_Timer = 3000;
        NihilSpeech_Phase = 0;

        ArcaneBlast_Timer = 7500;
        ManaBurn_Timer = 10000;
        IntangiblePresence_Timer = 15000;
    }

    void MoveInLineOfSight(Unit *who)
    {
        if (m_creature->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE))
            return;

        ScriptedAI::MoveInLineOfSight(who);
    }

    //in case creature was not summoned (not expected)
    void MovementInform(uint32 type, uint32 id)
    {
        if (type != POINT_MOTION_TYPE)
            return;

        if (id == 0)
        {
            m_creature->SetDeathState(JUST_DIED);
            m_creature->RemoveCorpse();
            m_creature->SetHealth(0);
        }
    }

    void SpellHit(Unit *caster, const SpellEntry *spell)
    {
        if (spell->Id == SPELL_T_PHASE_MODULATOR && caster->GetTypeId() == TYPEID_PLAYER)
        {
            const uint32 entry_list[4] = {ENTRY_PROTO, ENTRY_ADOLE, ENTRY_MATUR, ENTRY_NIHIL};
            int cid = rand()%(4-1);

            if (entry_list[cid] == m_creature->GetEntry())
                ++cid;

            //we are nihil, so say before transform
            if (m_creature->GetEntry() == ENTRY_NIHIL)
            {
                DoScriptText(SAY_NIHIL_INTERRUPT, m_creature);
                m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                IsNihil = false;
            }

            if (m_creature->UpdateEntry(entry_list[cid]))
            {
                if (entry_list[cid] == ENTRY_NIHIL)
                {
                    EnterEvadeMode();
                    m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                    IsNihil = true;
                }else
                    AttackStart(caster);
            }
        }
    }

    void UpdateAI(const uint32 diff)
    {
        if (IsNihil)
        {
            if (NihilSpeech_Timer <= diff)
            {
                switch(NihilSpeech_Phase)
                {
                    case 0:
                        DoScriptText(SAY_NIHIL_1, m_creature);
                        ++NihilSpeech_Phase;
                        break;
                    case 1:
                        DoScriptText(SAY_NIHIL_2, m_creature);
                        ++NihilSpeech_Phase;
                        break;
                    case 2:
                        DoScriptText(SAY_NIHIL_3, m_creature);
                        ++NihilSpeech_Phase;
                        break;
                    case 3:
                        DoScriptText(SAY_NIHIL_4, m_creature);
                        ++NihilSpeech_Phase;
                        break;
                    case 4:
                        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                        //take off to location above
                        m_creature->GetMotionMaster()->MovePoint(0, m_creature->GetPositionX()+50.0f, m_creature->GetPositionY(), m_creature->GetPositionZ()+50.0f);
                        ++NihilSpeech_Phase;
                        break;
                }
                NihilSpeech_Timer = 5000;
            }else NihilSpeech_Timer -=diff;

            //anything below here is not interesting for Nihil, so skip it
            return;
        }

        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (IntangiblePresence_Timer <= diff)
        {
            DoCastSpellIfCan(m_creature->getVictim(),SPELL_INTANGIBLE_PRESENCE);
            IntangiblePresence_Timer = urand(15000, 30000);
        }else IntangiblePresence_Timer -= diff;

        if (ManaBurn_Timer <= diff)
        {
            Unit* target = m_creature->getVictim();
            if (target && target->getPowerType() == POWER_MANA)
                DoCastSpellIfCan(target,SPELL_MANA_BURN);
            ManaBurn_Timer = urand(8000, 16000);
        }else ManaBurn_Timer -= diff;

        if (ArcaneBlast_Timer <= diff)
        {
            DoCastSpellIfCan(m_creature->getVictim(),SPELL_ARCANE_BLAST);
            ArcaneBlast_Timer = urand(2500, 7500);
        }else ArcaneBlast_Timer -= diff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_mobs_nether_drake(Creature* pCreature)
{
    return new mobs_nether_drakeAI(pCreature);
}

/*######
## npc_daranelle
######*/

enum
{
    SAY_SPELL_INFLUENCE     = -1000174,
    NPC_KALIRI_AURA_DISPEL  = 21511,
    SPELL_LASHHAN_CHANNEL   = 36904
};

struct MANGOS_DLL_DECL npc_daranelleAI : public ScriptedAI
{
    npc_daranelleAI(Creature* pCreature) : ScriptedAI(pCreature) {Reset();}

    void Reset() { }

    void MoveInLineOfSight(Unit* pWho)
    {
        if (pWho->GetTypeId() == TYPEID_PLAYER)
        {
            if (pWho->HasAura(SPELL_LASHHAN_CHANNEL, EFFECT_INDEX_0) && m_creature->IsWithinDistInMap(pWho, 10.0f))
            {
                DoScriptText(SAY_SPELL_INFLUENCE, m_creature, pWho);

                //TODO: Move the below to updateAI and run if this statement == true
                ((Player*)pWho)->KilledMonsterCredit(NPC_KALIRI_AURA_DISPEL, m_creature->GetObjectGuid());
                pWho->RemoveAurasDueToSpell(SPELL_LASHHAN_CHANNEL);
            }
        }

        ScriptedAI::MoveInLineOfSight(pWho);
    }
};

CreatureAI* GetAI_npc_daranelle(Creature* pCreature)
{
    return new npc_daranelleAI(pCreature);
}

/*######
## AddSC
######*/

void AddSC_blades_edge_mountains()
{
    Script* pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "mobs_bladespire_ogre";
    pNewScript->GetAI = &GetAI_mobs_bladespire_ogre;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "mobs_nether_drake";
    pNewScript->GetAI = &GetAI_mobs_nether_drake;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_daranelle";
    pNewScript->GetAI = &GetAI_npc_daranelle;
    pNewScript->RegisterSelf();
}
