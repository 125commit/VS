import unreal

char_da = unreal.load_asset("/Game/Data/DA/DA_CharacterClassInfo")
info_map = char_da.get_editor_property("CharacterClassInformation")
antonio = info_map[unreal.CharacterClass.ANTONIO]
print("antonio export", antonio.export_text())

weapon_paths = {
    unreal.CharacterClass.ANTONIO: "/Script/Engine.BlueprintGeneratedClass'/Game/AbilitySystem/Ability/Offensive/BP_WhipAbility.BP_WhipAbility_C'",
    unreal.CharacterClass.POE: "/Script/Engine.BlueprintGeneratedClass'/Game/AbilitySystem/Ability/Offensive/BP_GarlicAbility.BP_GarlicAbility_C'",
    unreal.CharacterClass.CLERICI: "/Script/Engine.BlueprintGeneratedClass'/Game/AbilitySystem/Ability/Offensive/BP_BibleAbility.BP_BibleAbility_C'",
    unreal.CharacterClass.ARCA: "/Script/Engine.BlueprintGeneratedClass'/Game/AbilitySystem/Ability/Offensive/BP_FireWandAbility.BP_FireWandAbility_C'",
    unreal.CharacterClass.IMELDA: "/Script/Engine.BlueprintGeneratedClass'/Game/AbilitySystem/Ability/Offensive/BP_MaigcWandAbility.BP_MaigcWandAbility_C'",
}

base_text = antonio.export_text()
for cls, weapon_path in weapon_paths.items():
    info = unreal.CharacterClassDefaultInfo()
    text = base_text
    if "StartupAbilities=" in text:
        start = text.index("StartupAbilities=")
        end = text.find(")", start)
        text = text[:start] + f'StartupAbilities=("{weapon_path}",)' + text[end+1:]
    else:
        text = text[:-1] + f',StartupAbilities=("{weapon_path}",))'
    try:
        info.import_text(text)
        info_map[cls] = info
        print("ok", cls, info.get_editor_property("StartupAbilities")[0].get_path_name())
    except Exception as e:
        print("fail", cls, e)

char_da.set_editor_property("CharacterClassInformation", info_map)
saved = unreal.EditorAssetLibrary.save_asset("/Game/Data/DA/DA_CharacterClassInfo", only_if_is_dirty=False)
print("saved", saved, "count", len(info_map))
