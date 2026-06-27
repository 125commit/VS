import unreal

print("has VSEnemyInfo", hasattr(unreal, "VSEnemyInfo"))
print("has DataTableRowHandle", hasattr(unreal, "DataTableRowHandle"))
asset = unreal.load_asset("/Game/Data/DA/DA_EnemyDictionary")
enemies = asset.get_editor_property("Enemies")
print("current keys", [str(k) for k in enemies.keys()])
for k in list(enemies.keys()):
    v = enemies[k]
    print("entry", k, v)
