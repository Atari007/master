/* Copyright (C) 2006 - 2011 ScriptDev2 <http://www.scriptdev2.com/>
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
SDName: Spell_Scripts
SD%Complete: 100
SDComment: Spell scripts for dummy effects (if script need access/interaction with parts of other AI or instance, add it in related source files)
SDCategory: Spell
EndScriptData */

/* ContentData
spell 34665
spell 19512
spell 8913
spell 21014
spell 29528
spell 29866
spell 46770
spell 46023
spell 47575
spell 50706
spell 45109
spell 45111
spell 39246
spell 52090
spell 51331
spell 51332
spell 51366
EndContentData */

#include "precompiled.h"

/* When you make a spell effect:
- always check spell id and effect index
- always return true when the spell is handled by script
*/

enum
{
    // quest 9452
    SPELL_CAST_FISHING_NET      = 29866,
    GO_RED_SNAPPER              = 181616,
    NPC_ANGRY_MURLOC            = 17102,
    ITEM_RED_SNAPPER            = 23614,
    //SPELL_SUMMON_TEST           = 49214                   // ! Just wrong spell name? It summon correct creature (17102)but does not appear to be used.
};

bool EffectDummyGameObj_spell_dummy_go(Unit* pCaster, uint32 uiSpellId, SpellEffectIndex uiEffIndex, GameObject* pGOTarget)
{
    switch(uiSpellId)
    {
        case SPELL_CAST_FISHING_NET:
        {
            if (uiEffIndex == EFFECT_INDEX_0)
            {
                if (pGOTarget->GetRespawnTime() != 0 || pGOTarget->GetEntry() != GO_RED_SNAPPER || pCaster->GetTypeId() != TYPEID_PLAYER)
                    return true;

                if (urand(0, 2))
                {
                    if (Creature* pMurloc = pCaster->SummonCreature(NPC_ANGRY_MURLOC, pCaster->GetPositionX(), pCaster->GetPositionY()+20.0f, pCaster->GetPositionZ(), 0.0f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 10000))
                        pMurloc->AI()->AttackStart(pCaster);
                }
                else
                {
                    if (Item* pItem = ((Player*)pCaster)->StoreNewItemInInventorySlot(ITEM_RED_SNAPPER, 1))
                        ((Player*)pCaster)->SendNewItem(pItem, 1, true, false);
                }

                pGOTarget->SetLootState(GO_JUST_DEACTIVATED);
                return true;
            }
            return true;
        }
    }

    return false;
}

enum
{
    // quest 9629
    SPELL_TAG_MURLOC                    = 30877,
    SPELL_TAG_MURLOC_PROC               = 30875,
    NPC_BLACKSILT_MURLOC                = 17326,
    NPC_TAGGED_MURLOC                   = 17654,

    // quest 9447
    SPELL_HEALING_SALVE                 = 29314,
    SPELL_HEALING_SALVE_DUMMY           = 29319,
    NPC_MAGHAR_GRUNT                    = 16846,

    // quest 10190
    SPELL_RECHARGING_BATTERY            = 34219,
    NPC_DRAINED_PHASE_HUNTER            = 19595,

    // target hulking helboar
    SPELL_ADMINISTER_ANTIDOTE           = 34665,
    NPC_HELBOAR                         = 16880,
    NPC_DREADTUSK                       = 16992,

    // quest 6124/6129
    SPELL_APPLY_SALVE                   = 19512,

    NPC_SICKLY_DEER                     = 12298,
    NPC_SICKLY_GAZELLE                  = 12296,

    NPC_CURED_DEER                      = 12299,
    NPC_CURED_GAZELLE                   = 12297,

    // target morbent fel
    SPELL_SACRED_CLEANSING              = 8913,
    NPC_MORBENT                         = 1200,
    NPC_WEAKENED_MORBENT                = 24782,

    // quest 11515
    SPELL_FEL_SIPHON_DUMMY              = 44936,
    NPC_FELBLOOD_INITIATE               = 24918,
    NPC_EMACIATED_FELBLOOD              = 24955,

    // target nestlewood owlkin
    SPELL_INOCULATE_OWLKIN              = 29528,
    NPC_OWLKIN                          = 16518,
    NPC_OWLKIN_INOC                     = 16534,

    // Quest "Disrupt the Greengill Coast" (11541)
    SPELL_ORB_OF_MURLOC_CONTROL         = 45109,
    SPELL_GREENGILL_SLAVE_FREED         = 45110,
    SPELL_ENRAGE                        = 45111,
    NPC_FREED_GREENGILL_SLAVE           = 25085,
    NPC_DARKSPINE_MYRMIDON              = 25060,
    NPC_DARKSPINE_SIREN                 = 25073,

    // quest "The Big Bone Worm" 10930
    SPELL_FUMPING                       = 39246,
    SPELL_SUMMON_HAISHULUD              = 39248,
    NPC_SAND_GNOME                      = 22483,
    NPC_MATURE_BONE_SIFTER              = 22482,

    // quest 9849, item 24501
    SPELL_THROW_GORDAWG_BOULDER         = 32001,
    NPC_MINION_OF_GUROK                 = 18181,
};

bool EffectAuraDummy_spell_aura_dummy_npc(const Aura* pAura, bool bApply)
{
    switch(pAura->GetId())
    {
        case SPELL_HEALING_SALVE:
        {
            if (pAura->GetEffIndex() != EFFECT_INDEX_0)
                return true;

            if (bApply)
            {
                if (Unit* pCaster = pAura->GetCaster())
                    pCaster->CastSpell(pAura->GetTarget(), SPELL_HEALING_SALVE_DUMMY, true);
            }

            return true;
        }
        case SPELL_HEALING_SALVE_DUMMY:
        {
            if (pAura->GetEffIndex() != EFFECT_INDEX_0)
                return true;

            if (!bApply)
            {
                Creature* pCreature = (Creature*)pAura->GetTarget();

                pCreature->UpdateEntry(NPC_MAGHAR_GRUNT);

                if (pCreature->getStandState() == UNIT_STAND_STATE_KNEEL)
                    pCreature->SetStandState(UNIT_STAND_STATE_STAND);

                pCreature->ForcedDespawn(60*IN_MILLISECONDS);
            }

            return true;
        }
        case SPELL_RECHARGING_BATTERY:
        {
            if (pAura->GetEffIndex() != EFFECT_INDEX_0)
                return true;

            if (!bApply)
            {
                if (pAura->GetTarget()->HasAuraState(AURA_STATE_HEALTHLESS_20_PERCENT))
                    ((Creature*)pAura->GetTarget())->UpdateEntry(NPC_DRAINED_PHASE_HUNTER);
            }

            return true;
        }
        case SPELL_TAG_MURLOC:
        {
            Creature* pCreature = (Creature*)pAura->GetTarget();

            if (pAura->GetEffIndex() != EFFECT_INDEX_0)
                return true;

            if (bApply)
            {
                if (pCreature->GetEntry() == NPC_BLACKSILT_MURLOC)
                {
                    if (Unit* pCaster = pAura->GetCaster())
                        pCaster->CastSpell(pCreature, SPELL_TAG_MURLOC_PROC, true);
                }
            }
            else
            {
                if (pCreature->GetEntry() == NPC_TAGGED_MURLOC)
                    pCreature->ForcedDespawn();
            }

            return true;
        }
        case SPELL_ENRAGE:
        {
            if (!bApply || pAura->GetTarget()->GetTypeId() != TYPEID_UNIT)
                return false;

            Creature* pTarget = (Creature*)pAura->GetTarget();

            if (Creature* pCreature = GetClosestCreatureWithEntry(pTarget, NPC_DARKSPINE_MYRMIDON, 25.0f))
            {
                pTarget->AI()->AttackStart(pCreature);
                return true;
            }

            if (Creature* pCreature = GetClosestCreatureWithEntry(pTarget, NPC_DARKSPINE_SIREN, 25.0f))
            {
                pTarget->AI()->AttackStart(pCreature);
                return true;
            }

            return false;
        }
    }

    return false;
}

bool EffectDummyCreature_spell_dummy_npc(Unit* pCaster, uint32 uiSpellId, SpellEffectIndex uiEffIndex, Creature* pCreatureTarget)
{
    switch(uiSpellId)
    {
        case SPELL_ADMINISTER_ANTIDOTE:
        {
            if (uiEffIndex == EFFECT_INDEX_0)
            {
                if (pCreatureTarget->GetEntry() != NPC_HELBOAR)
                    return true;

                // possible needs check for quest state, to not have any effect when quest really complete

                pCreatureTarget->UpdateEntry(NPC_DREADTUSK);
                return true;
            }
            return true;
        }
        case SPELL_APPLY_SALVE:
        {
            if (uiEffIndex == EFFECT_INDEX_0)
            {
                if (pCaster->GetTypeId() != TYPEID_PLAYER)
                    return true;

                if (pCreatureTarget->GetEntry() == NPC_SICKLY_DEER && ((Player*)pCaster)->GetTeam() == ALLIANCE)
				{
                    pCreatureTarget->UpdateEntry(NPC_CURED_DEER);
					pCreatureTarget->ForcedDespawn(1000);
					if (((Player*) pCaster)->GetQuestStatus(6124))
						((Player*) pCaster)->KilledMonsterCredit(NPC_CURED_DEER);
				}
                if (pCreatureTarget->GetEntry() == NPC_SICKLY_GAZELLE && ((Player*)pCaster)->GetTeam() == HORDE)
				{
                    pCreatureTarget->UpdateEntry(NPC_CURED_GAZELLE);
					pCreatureTarget->ForcedDespawn(1000);
					if (((Player*) pCaster)->GetQuestStatus(6129))
						((Player*) pCaster)->KilledMonsterCredit(NPC_CURED_GAZELLE);
				}
                return true;
            }
            return true;
        }
        case SPELL_INOCULATE_OWLKIN:
        {
            if (uiEffIndex == EFFECT_INDEX_0)
            {
                if (pCreatureTarget->GetEntry() != NPC_OWLKIN)
                    return true;

                pCreatureTarget->UpdateEntry(NPC_OWLKIN_INOC);

                //set despawn timer, since we want to remove creature after a short time
                pCreatureTarget->ForcedDespawn(15000);

                return true;
            }
            return true;
        }
        case SPELL_FEL_SIPHON_DUMMY:
        {
            if (uiEffIndex == EFFECT_INDEX_0)
            {
                if (pCreatureTarget->GetEntry() != NPC_FELBLOOD_INITIATE)
                    return true;

                pCreatureTarget->UpdateEntry(NPC_EMACIATED_FELBLOOD);
                return true;
            }
            return true;
        }
        case SPELL_SACRED_CLEANSING:
        {
            if (uiEffIndex == EFFECT_INDEX_1)
            {
                if (pCreatureTarget->GetEntry() != NPC_MORBENT)
                    return true;

                pCreatureTarget->UpdateEntry(NPC_WEAKENED_MORBENT);
                return true;
            }
            return true;
        }
        case SPELL_TAG_MURLOC_PROC:
        {
            if (uiEffIndex == EFFECT_INDEX_0)
            {
                if (pCreatureTarget->GetEntry() == NPC_BLACKSILT_MURLOC)
                    pCreatureTarget->UpdateEntry(NPC_TAGGED_MURLOC);
            }
            return true;
        }
        case SPELL_ORB_OF_MURLOC_CONTROL:
        {
            pCreatureTarget->CastSpell(pCaster, SPELL_GREENGILL_SLAVE_FREED, true);

            // Freed Greengill Slave
            pCreatureTarget->UpdateEntry(NPC_FREED_GREENGILL_SLAVE);

            pCreatureTarget->CastSpell(pCreatureTarget, SPELL_ENRAGE, true);

            return true;
        }
        case SPELL_FUMPING:
        {
            if (uiEffIndex == EFFECT_INDEX_2)
            {
                switch(urand(0,2))
                {
                    case 0:
                    {
                        pCaster->CastSpell(pCreatureTarget, SPELL_SUMMON_HAISHULUD, true);
                        break;
                    }
                    case 1:
                    {
                        for (int i = 0; i<2; ++i)
                        {
                            if (Creature* pSandGnome = pCaster->SummonCreature(NPC_SAND_GNOME, pCreatureTarget->GetPositionX(), pCreatureTarget->GetPositionY(), pCreatureTarget->GetPositionZ(), 0.0f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 30000))
                                pSandGnome->AI()->AttackStart(pCaster);
                        }
                        break;
                    }
                    case 2:
                    {
                        for (int i = 0; i<2; ++i)
                        {
                            if (Creature* pMatureBoneSifter = pCaster->SummonCreature(NPC_MATURE_BONE_SIFTER, pCreatureTarget->GetPositionX(), pCreatureTarget->GetPositionY(), pCreatureTarget->GetPositionZ(), 0.0f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 30000))
                                pMatureBoneSifter->AI()->AttackStart(pCaster);
                        }
                        break;
                    }
                }
                pCreatureTarget->ForcedDespawn();
            }
            return true;
        }
        case SPELL_THROW_GORDAWG_BOULDER:
        {
            if (uiEffIndex == EFFECT_INDEX_0)
            {
                for(int i = 0; i < 3; ++i)
                {
                    if (irand(i, 2))                        // 2-3 summons
                        pCreatureTarget->SummonCreature(NPC_MINION_OF_GUROK, 0.0f, 0.0f, 0.0f, 0.0f, TEMPSUMMON_CORPSE_DESPAWN, 5000);
                }

                if (pCreatureTarget->getVictim())
                {
                    pCaster->DealDamage(pCreatureTarget, pCreatureTarget->GetMaxHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
                    return true;
                }

                // If not in combat, no xp or loot
                pCreatureTarget->SetDeathState(JUST_DIED);
                pCreatureTarget->SetHealth(0);
                return true;
            }
            return true;
        }
    }

    return false;
}

void AddSC_spell_scripts()
{
    Script* pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "spell_dummy_go";
    pNewScript->pEffectDummyGO = &EffectDummyGameObj_spell_dummy_go;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "spell_dummy_npc";
    pNewScript->pEffectDummyNPC = &EffectDummyCreature_spell_dummy_npc;
    pNewScript->pEffectAuraDummy = &EffectAuraDummy_spell_aura_dummy_npc;
    pNewScript->RegisterSelf();
}
