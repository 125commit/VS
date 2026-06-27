import unreal

enemy_dict = unreal.load_asset("/Game/Data/DA/DA_EnemyDictionary")
enemy_info_dt = unreal.load_asset("/Game/Data/DT/Enemy/DT_EnemyInfo")
enemies = enemy_dict.get_editor_property("Enemies")
ghost_info = enemies[unreal.VSEnemyType.GHOST]

mapping = [
    (unreal.VSEnemyType.GHOST, "Ghost"),
    (unreal.VSEnemyType.GOBLIN, "Goblin"),
    (unreal.VSEnemyType.SLIME, "Slime"),
    (unreal.VSEnemyType.BAT, "Bat"),
    (unreal.VSEnemyType.BOSS, "Boss"),
]

for enemy_type, row_name in mapping:
    if enemy_type not in enemies:
        enemies[enemy_type] = ghost_info
    info = enemies[enemy_type]
    handle = unreal.DataTableRowHandle()
    handle.set_editor_property("data_table", enemy_info_dt)
    handle.set_editor_property("row_name", row_name)
    info.set_editor_property("stat_row", handle)
    enemies[enemy_type] = info

if unreal.VSEnemyType.NONE in enemies:
    del enemies[unreal.VSEnemyType.NONE]

enemy_dict.set_editor_property("Enemies", enemies)
saved = unreal.EditorAssetLibrary.save_asset("/Game/Data/DA/DA_EnemyDictionary", only_if_is_dirty=False)
print("SAVED", saved, [str(k) for k in enemies.keys()])
for enemy_type, row_name in mapping:
    info = enemies[enemy_type]
    sr = info.get_editor_property("stat_row")
    print(str(enemy_type), sr.get_editor_property("row_name"))
