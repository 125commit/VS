import unreal

enemy_dict = unreal.load_asset("/Game/Data/DA/DA_EnemyDictionary")
enemy_info_dt = unreal.load_asset("/Game/Data/DT/Enemy/DT_EnemyInfo")
enemies = enemy_dict.get_editor_property("Enemies")
ghost_info = enemies[unreal.VSEnemyType.GHOST]
enemy_class = ghost_info.get_editor_property("enemy_class")
damage_effect = ghost_info.get_editor_property("damage_effect_class")

try:
    info = unreal.new_object(enemy_dict, unreal.VSEnemyInfo)
    print("new_object ok", info)
    info.set_editor_property("enemy_class", enemy_class)
except Exception as e:
    print("new_object fail", e)

try:
    info2 = unreal.VSEnemyInfo()
    info2.enemy_class = enemy_class
    print("direct assign ok")
except Exception as e:
    print("direct assign fail", e)
