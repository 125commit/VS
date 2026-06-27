import unreal
import os

csv_path = os.path.join(unreal.Paths.project_dir(), "Scripts", "CT_PassiveValue.csv")
ct = unreal.load_asset("/Game/Data/CT_PassiveValue")
print("csv exists", os.path.exists(csv_path), csv_path)

if hasattr(unreal, "AssetImportTask"):
    task = unreal.AssetImportTask()
    task.filename = csv_path
    task.destination_path = "/Game/Data"
    task.destination_name = "CT_PassiveValue"
    task.replace_existing = True
    task.automated = True
    task.save = True
    unreal.AssetToolsHelpers.get_asset_tools().import_asset_tasks([task])
    print("import task done")

try:
    factory = unreal.CurveTableFactory()
    print("factory", factory)
except Exception as e:
    print("factory fail", e)

for row in ["Spinach", "EmptyTome", "Pummarola", "HollowHeart", "Spellbinder"]:
    result, out = unreal.DataTableFunctionLibrary.evaluate_curve_table_row(ct, row, 1.0, "fill_passive")
    print("eval", row, "lvl1", out, result)

saved = unreal.EditorAssetLibrary.save_asset("/Game/Data/CT_PassiveValue", only_if_is_dirty=False)
print("saved", saved)
