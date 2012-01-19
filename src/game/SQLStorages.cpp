/*
 * Copyright (C) 2005-2011 MaNGOS <http://getmangos.com/>
 *
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

#include "SQLStorages.h"
#include "Database/SQLStorage.h"
#include "Database/SQLStorageImpl.h"
#include "Database/DatabaseEnv.h"

const char CreatureInfosrcfmt[]="iiiiiiiisssiiiiiiiiiiifffiffiifiiiiiiiiiiffiiiiiiiiiiiiiiiiiiisiifflliiiiis";
const char CreatureInfodstfmt[]="iiiiiiiisssiiiiiiiiiiifffiffiifiiiiiiiiiiffiiiiiiiiiiiiiiiiiiisiifflliiiiii";
const char CreatureDataAddonInfofmt[]="iiibbiis";
const char CreatureModelfmt[]="iffbii";
const char CreatureInfoAddonInfofmt[]="iiibbiis";
const char EquipmentInfofmt[]="iiii";
const char EquipmentInfoRawfmt[]="iiiiiiiiii";
const char GameObjectInfosrcfmt[]="iiisssiifiiiiiiiiiiiiiiiiiiiiiiiiiis";
const char GameObjectInfodstfmt[]="iiisssiifiiiiiiiiiiiiiiiiiiiiiiiiiii";
const char ItemPrototypesrcfmt[]="iiiisiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiffiffiffiffiffiiiiiiiiiifiiifiiiiiifiiiiiifiiiiiifiiiiiifiiiisiiiiiiiiiiiiiiiiiiiiiiiiifsiiiiii";
const char ItemPrototypedstfmt[]="iiiisiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiffiffiffiffiffiiiiiiiiiifiiifiiiiiifiiiiiifiiiiiifiiiiiifiiiisiiiiiiiiiiiiiiiiiiiiiiiiifiiiiiii";
const char PageTextfmt[]="isi";
const char InstanceTemplatesrcfmt[]="iiiiiis";
const char InstanceTemplatedstfmt[]="iiiiiii";
const char WorldTemplatesrcfmt[]="is";
const char WorldTemplatedstfmt[]="ii";

SQLStorage sCreatureStorage(CreatureInfosrcfmt, CreatureInfodstfmt, "entry","creature_template");
SQLStorage sCreatureDataAddonStorage(CreatureDataAddonInfofmt,"guid","creature_addon");
SQLStorage sCreatureModelStorage(CreatureModelfmt,"modelid","creature_model_info");
SQLStorage sCreatureInfoAddonStorage(CreatureInfoAddonInfofmt,"entry","creature_template_addon");
SQLStorage sEquipmentStorage(EquipmentInfofmt,"entry","creature_equip_template");
SQLStorage sEquipmentStorageRaw(EquipmentInfoRawfmt,"entry","creature_equip_template_raw");
SQLStorage sGOStorage(GameObjectInfosrcfmt, GameObjectInfodstfmt, "entry","gameobject_template");
SQLStorage sItemStorage(ItemPrototypesrcfmt, ItemPrototypedstfmt, "entry","item_template");
SQLStorage sPageTextStore(PageTextfmt,"entry","page_text");
SQLStorage sInstanceTemplate(InstanceTemplatesrcfmt, InstanceTemplatedstfmt, "map","instance_template");
SQLStorage sWorldTemplate(WorldTemplatesrcfmt, WorldTemplatedstfmt, "map","world_template");
