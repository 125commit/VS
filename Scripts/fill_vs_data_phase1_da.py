import unreal
import json

enemy_dict = unreal.load_asset("/Game/Data/DA/DA_EnemyDictionary")
enemy_info_dt = unreal.load_asset("/Game/Data/DT/Enemy/DT_EnemyInfo")
enemies = enemy_dict.get_editor_property("Enemies")

ghost_info = enemies[unreal.VSEnemyType.GHOST]
enemy_class = ghost_info.get_editor_property("enemy_class")
damage_effect = ghost_info.get_editor_property("damage_effect_class")

mapping = [
    (unreal.VSEnemyType.GHOST, "Ghost"),
    (unreal.VSEnemyType.GOBLIN, "Goblin"),
    (unreal.VSEnemyType.SLIME, "Slime"),
    (unreal.VSEnemyType.BAT, "Bat"),
    (unreal.VSEnemyType.BOSS, "Boss"),
]

for enemy_type, row_name in mapping:
    info = unreal.VSEnemyInfo()
    info.set_editor_property("enemy_class", enemy_class)
    info.set_editor_property("damage_effect_class", damage_effect)
    handle = unreal.DataTableRowHandle()
    handle.set_editor_property("data_table", enemy_info_dt)
    handle.set_editor_property("row_name", row_name)
    info.set_editor_property("stat_row", handle)
    enemies[enemy_type] = info

if unreal.VSEnemyType.NONE in enemies:
    del enemies[unreal.VSEnemyType.NONE]

enemy_dict.set_editor_property("Enemies", enemies)
saved = unreal.EditorAssetLibrary.save_asset("/Game/Data/DA/DA_EnemyDictionary", only_if_is_dirty=False)
print("SAVED DA_EnemyDictionary", saved, [str(k) for k in enemies.keys()])

drop_da = unreal.load_asset("/Game/Data/DA/DA_DropItems")
drop_da.set_editor_property("DefaultMagnetRadius", 150.0)
drop_da.set_editor_property("MagnetMoveSpeed", 600.0)
drop_da.set_editor_property("PickupRadius", 60.0)
saved_drop = unreal.EditorAssetLibrary.save_asset("/Game/Data/DA/DA_DropItems", only_if_is_dirty=False)
print("SAVED DA_DropItems", saved_drop)
