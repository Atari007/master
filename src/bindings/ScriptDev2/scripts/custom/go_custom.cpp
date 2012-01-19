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
SDName: go_custom
SD%Complete: 
SDComment: Custom gameobjects scripts
SDCategory: Custom
SDAuthor: Dexter, <http://excalibur-wow.com>
EndScriptData */

/* ContentData
go_scryer_orb
EndContentData*/

#include "precompiled.h"

/*######
## go_scryer_orb
######*/

enum
{
    QUEST_THE_SCRYERS_SCRYER	= 11490,
	NPC_KALECGOS_HUMAN			= 24848
};

bool GOUse_go_scryer_orb(Player* pPlayer, GameObject* pGo)
{
    if (pPlayer->GetQuestStatus(QUEST_THE_SCRYERS_SCRYER) == QUEST_STATUS_INCOMPLETE)
	{
        pGo->SummonCreature(NPC_KALECGOS_HUMAN, 0.0f, 0.0f, 0.0f, 0.0f, TEMPSUMMON_TIMED_DESPAWN, 300000);
		pPlayer->SetQuestStatus(QUEST_THE_SCRYERS_SCRYER,QUEST_STATUS_COMPLETE);
	}

    return true;
}

void AddSC_go_custom()
{
	Script* pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "go_scryer_orb";
    pNewScript->pGOUse = &GOUse_go_scryer_orb;
    pNewScript->RegisterSelf();
}