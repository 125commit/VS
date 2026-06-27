import unreal

enemy_dict = unreal.load_asset("/Game/Data/DA/DA_EnemyDictionary")
for fn in ["export_asset_to_text", "import_asset_from_text", "duplicate_asset"]:
    print(fn, hasattr(unreal.EditorAssetLibrary, fn))

try:
    text = unreal.EditorAssetLibrary.export_asset_to_text(enemy_dict)
    print("EXPORT_LEN", len(text) if text else 0)
    print(text[:2000] if text else "none")
except Exception as e:
    print("export fail", e)
