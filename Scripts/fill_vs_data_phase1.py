import unreal
import json

def fill_dt(path, rows):
    dt = unreal.load_asset(path)
    ok = unreal.DataTableFunctionLibrary.fill_data_table_from_json_string(dt, json.dumps(rows))
    saved = unreal.EditorAssetLibrary.save_asset(path, only_if_is_dirty=False)
    print("FILL", path, ok, saved, "rows", len(rows))
    return ok and saved

def drop_table_ref(name):
    return f"/Script/Engine.DataTable'/Game/Data/DT/Enemy/Drop/{name}.{name}'"

def standard_xp_drop():
    return [
        {"Name": "GuaranteedSmall", "DropType": "XPSmall", "DropValue": 1.0, "bGuaranteed": True, "DropChance": 1.0},
        {"Name": "ChanceLarge", "DropType": "XPLarge", "DropValue": 10.0, "bGuaranteed": False, "DropChance": 0.08},
    ]

def boss_drop():
    return [
        {"Name": "GuaranteedLarge", "DropType": "XPLarge", "DropValue": 50.0, "bGuaranteed": True, "DropChance": 1.0},
        {"Name": "ChanceBox", "DropType": "Box", "DropValue": 3.0, "bGuaranteed": False, "DropChance": 0.5},
    ]

enemy_rows = [
    {"Name": "Ghost", "MaxHealth": 80.0, "MoveSpeed": 220.0, "BodyScale": 1.0, "AttackDamage": 3.0, "AttackInterval": 1.0, "KnockbackResistance": 0.0, "EliteHealthMultiplier": 2.0, "EliteScaleMultiplier": 1.3, "DropTable": drop_table_ref("DT_GhostDrop")},
    {"Name": "Goblin", "MaxHealth": 120.0, "MoveSpeed": 200.0, "BodyScale": 1.0, "AttackDamage": 5.0, "AttackInterval": 1.0, "KnockbackResistance": 0.1, "EliteHealthMultiplier": 2.0, "EliteScaleMultiplier": 1.3, "DropTable": drop_table_ref("DT_GoblinDrop")},
    {"Name": "Slime", "MaxHealth": 200.0, "MoveSpeed": 140.0, "BodyScale": 1.0, "AttackDamage": 6.0, "AttackInterval": 1.0, "KnockbackResistance": 0.2, "EliteHealthMultiplier": 2.0, "EliteScaleMultiplier": 1.3, "DropTable": drop_table_ref("DT_SlimeDrop")},
    {"Name": "Bat", "MaxHealth": 60.0, "MoveSpeed": 280.0, "BodyScale": 1.0, "AttackDamage": 4.0, "AttackInterval": 1.0, "KnockbackResistance": 0.0, "EliteHealthMultiplier": 2.0, "EliteScaleMultiplier": 1.3, "DropTable": drop_table_ref("DT_BatDrop")},
    {"Name": "Boss", "MaxHealth": 2000.0, "MoveSpeed": 160.0, "BodyScale": 1.2, "AttackDamage": 15.0, "AttackInterval": 1.5, "KnockbackResistance": 0.96, "EliteHealthMultiplier": 2.0, "EliteScaleMultiplier": 1.5, "DropTable": drop_table_ref("DT_BossDrop")},
]

fill_dt("/Game/Data/DT/Enemy/DT_EnemyInfo", enemy_rows)
fill_dt("/Game/Data/DT/Enemy/Drop/DT_GhostDrop", standard_xp_drop())
fill_dt("/Game/Data/DT/Enemy/Drop/DT_GoblinDrop", standard_xp_drop())
fill_dt("/Game/Data/DT/Enemy/Drop/DT_SlimeDrop", standard_xp_drop())
fill_dt("/Game/Data/DT/Enemy/Drop/DT_BatDrop", standard_xp_drop())
fill_dt("/Game/Data/DT/Enemy/Drop/DT_BossDrop", boss_drop())

enemy_dict = unreal.load_asset("/Game/Data/DA/DA_EnemyDictionary")
enemy_info_dt = unreal.load_asset("/Game/Data/DT/Enemy/DT_EnemyInfo")
damage_effect = unreal.load_class(None, "/Game/AbilitySystem/Ability/BP_EnemyDamage.BP_EnemyDamage_C")
enemy_class = unreal.load_class(None, "/Game/TestCharacter/ThirdPerson/Enemy/BP_Enemy1.BP_Enemy1_C")

enemies = enemy_dict.get_editor_property("Enemies")
for enum_name, row_name in [("GHOST", "Ghost"), ("GOBLIN", "Goblin"), ("SLIME", "Slime"), ("BAT", "Bat"), ("BOSS", "Boss")]:
    enemy_type = getattr(unreal.VSEnemyType, enum_name)
    info = unreal.VSEnemyInfo()
    info.set_editor_property("enemy_class", enemy_class)
    info.set_editor_property("damage_effect_class", damage_effect)
    handle = unreal.DataTableRowHandle()
    handle.set_editor_property("data_table", enemy_info_dt)
    handle.set_editor_property("row_name", row_name)
    info.set_editor_property("stat_row", handle)
    enemies[enemy_type] = info
    if hasattr(unreal.VSEnemyType, "NONE") and unreal.VSEnemyType.NONE in enemies:
        del enemies[unreal.VSEnemyType.NONE]

enemy_dict.set_editor_property("Enemies", enemies)
unreal.EditorAssetLibrary.save_asset("/Game/Data/DA/DA_EnemyDictionary", only_if_is_dirty=False)
print("SAVED DA_EnemyDictionary keys", [str(k) for k in enemies.keys()])

drop_da = unreal.load_asset("/Game/Data/DA/DA_DropItems")
drop_da.set_editor_property("DefaultMagnetRadius", 150.0)
drop_da.set_editor_property("MagnetMoveSpeed", 600.0)
drop_da.set_editor_property("PickupRadius", 60.0)
unreal.EditorAssetLibrary.save_asset("/Game/Data/DA/DA_DropItems", only_if_is_dirty=False)
print("SAVED DA_DropItems")
