import unreal

char_da = unreal.load_asset("/Game/Data/DA/DA_CharacterClassInfo")
info_map = char_da.get_editor_property("CharacterClassInformation")
antonio = info_map[unreal.CharacterClass.ANTONIO]
mesh = antonio.get_editor_property("CharacterMesh")
attrs = antonio.get_editor_property("PrimaryAttributes")

weapon_paths = {
    unreal.CharacterClass.ANTONIO: "/Script/Engine.BlueprintGeneratedClass'/Game/AbilitySystem/Ability/Offensive/BP_WhipAbility.BP_WhipAbility_C'",
    unreal.CharacterClass.POE: "/Script/Engine.BlueprintGeneratedClass'/Game/AbilitySystem/Ability/Offensive/BP_GarlicAbility.BP_GarlicAbility_C'",
    unreal.CharacterClass.CLERICI: "/Script/Engine.BlueprintGeneratedClass'/Game/AbilitySystem/Ability/Offensive/BP_BibleAbility.BP_BibleAbility_C'",
    unreal.CharacterClass.ARCA: "/Script/Engine.BlueprintGeneratedClass'/Game/AbilitySystem/Ability/Offensive/BP_FireWandAbility.BP_FireWandAbility_C'",
    unreal.CharacterClass.IMELDA: "/Script/Engine.BlueprintGeneratedClass'/Game/AbilitySystem/Ability/Offensive/BP_MaigcWandAbility.BP_MaigcWandAbility_C'",
}

for cls, weapon_path in weapon_paths.items():
    info = unreal.CharacterClassDefaultInfo()
    text = f'(StartupAbilities=("{weapon_path}",))'
    try:
        info.import_text(text)
        info.set_editor_property("CharacterMesh", mesh)
        info.set_editor_property("PrimaryAttributes", attrs)
        info_map[cls] = info
        print("import ok", cls, info.get_editor_property("StartupAbilities"))
    except Exception as e:
        print("import fail", cls, e)

char_da.set_editor_property("CharacterClassInformation", info_map)
saved = unreal.EditorAssetLibrary.save_asset("/Game/Data/DA/DA_CharacterClassInfo", only_if_is_dirty=False)
print("saved", saved, "count", len(info_map))
