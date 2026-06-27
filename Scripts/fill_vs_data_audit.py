import unreal

DT_PATHS = [
    "/Game/Data/DT/Wave/DT_WaveDirectorConfig",
    "/Game/Data/DT/Enemy/DT_EnemyInfo",
    "/Game/Data/DT/Level/DT_LevelUpTable",
    "/Game/Data/DT/Ability/DT_Whip",
    "/Game/Data/DT/Ability/DT_Garlic",
    "/Game/Data/DT/Ability/DT_Bible",
    "/Game/Data/DT/Ability/DT_FireWand",
    "/Game/Data/DT/Ability/DT_MagicWand",
    "/Game/Data/DT/Ability/DT_EvolvedWeapon",
    "/Game/Data/DT/Level/Passive/DT_Spinach",
    "/Game/Data/DT/Level/Passive/DT_EmptyTome",
    "/Game/Data/DT/Level/Passive/DT_Pummarola",
    "/Game/Data/DT/Level/Passive/DT_HollowHeart",
    "/Game/Data/DT/Level/Passive/DT_Spellbinder",
    "/Game/Data/DT/Enemy/Drop/DT_GhostDrop",
    "/Game/Data/DT/Enemy/Drop/DT_GoblinDrop",
    "/Game/Data/DT/Enemy/Drop/DT_SlimeDrop",
    "/Game/Data/DT/Enemy/Drop/DT_BatDrop",
    "/Game/Data/DT/Enemy/Drop/DT_BossDrop",
]

DA_PATHS = [
    "/Game/Data/DA/DA_AbilityInfo",
    "/Game/Data/DA/DA_CharacterClassInfo",
    "/Game/Data/DA/DA_DropItems",
    "/Game/Data/DA/DA_EnemyDictionary",
    "/Game/Data/DA/DA_WaveDirectorConfig",
]

for path in DT_PATHS:
    dt = unreal.load_asset(path)
    if not dt:
        print("MISSING", path)
        continue
    names = unreal.DataTableFunctionLibrary.get_data_table_row_names(dt)
    print("DT", path, "rows", [str(n) for n in names])
    print(unreal.DataTableFunctionLibrary.export_data_table_to_json_string(dt))

for path in DA_PATHS:
    asset = unreal.load_asset(path)
    print("DA", path, asset.get_class().get_name() if asset else "None")
