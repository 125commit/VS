import unreal

char_da = unreal.load_asset("/Game/Data/DA/DA_CharacterClassInfo")
info_map = char_da.get_editor_property("CharacterClassInformation")
antonio = info_map[unreal.CharacterClass.ANTONIO]
weapon_map = {
    unreal.CharacterClass.ANTONIO: "/Game/AbilitySystem/Ability/Offensive/BP_WhipAbility.BP_WhipAbility_C",
    unreal.CharacterClass.POE: "/Game/AbilitySystem/Ability/Offensive/BP_GarlicAbility.BP_GarlicAbility_C",
    unreal.CharacterClass.CLERICI: "/Game/AbilitySystem/Ability/Offensive/BP_BibleAbility.BP_BibleAbility_C",
    unreal.CharacterClass.ARCA: "/Game/AbilitySystem/Ability/Offensive/BP_FireWandAbility.BP_FireWandAbility_C",
    unreal.CharacterClass.IMELDA: "/Game/AbilitySystem/Ability/Offensive/BP_MaigcWandAbility.BP_MaigcWandAbility_C",
}
for cls, path in weapon_map.items():
    entry = info_map[cls] if cls in info_map else antonio
    weapon_cls = unreal.load_class(None, path)
    entry.set_editor_property("StartupAbilities", [weapon_cls])
    if cls not in info_map:
        entry.set_editor_property("CharacterMesh", antonio.get_editor_property("CharacterMesh"))
        entry.set_editor_property("PrimaryAttributes", antonio.get_editor_property("PrimaryAttributes"))
    info_map[cls] = entry
char_da.set_editor_property("CharacterClassInformation", info_map)
saved = unreal.EditorAssetLibrary.save_asset("/Game/Data/DA/DA_CharacterClassInfo", only_if_is_dirty=False)
print("SAVED CharacterClassInfo", saved)
for cls, path in weapon_map.items():
    sa = info_map[cls].get_editor_property("StartupAbilities")
    print(cls, sa[0].get_path_name() if sa else "NONE")
