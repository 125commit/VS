import unreal

ability = unreal.load_asset("/Game/Data/DA/DA_AbilityInfo")
print("GlobalEvolved", ability.get_editor_property("GlobalEvolvedWeaponStatsTable"))
infos = ability.get_editor_property("AbilityInformation")
for info in infos:
    tag = info.get_editor_property("AbilityTag")
    name = info.get_editor_property("Name")
    evolved = info.get_editor_property("bIsEvolvedWeapon")
    table = info.get_editor_property("LevelTable")
    cls = info.get_editor_property("AbilityClass")
    print(str(tag), str(name), "evolved", evolved, "table", table, "class", cls)

char_da = unreal.load_asset("/Game/Data/DA/DA_CharacterClassInfo")
classes = char_da.get_editor_property("CharacterClassInformation")
weapon_paths = {
    "Antonio": "/Game/AbilitySystem/Ability/Offensive/BP_WhipAbility.BP_WhipAbility_C",
    "Poe": "/Game/AbilitySystem/Ability/Offensive/BP_GarlicAbility.BP_GarlicAbility_C",
    "Clerici": "/Game/AbilitySystem/Ability/Offensive/BP_BibleAbility.BP_BibleAbility_C",
    "Arca": "/Game/AbilitySystem/Ability/Offensive/BP_FireWandAbility.BP_FireWandAbility_C",
    "Imelda": "/Game/AbilitySystem/Ability/Offensive/BP_MaigcWandAbility.BP_MaigcWandAbility_C",
}
for cls_enum, path in weapon_paths.items():
    enum_val = getattr(unreal.CharacterClass, cls_enum.upper())
    if enum_val in classes:
        info = classes[enum_val]
        startups = info.get_editor_property("StartupAbilities")
        if startups:
            print(cls_enum, startups[0].get_path_name())
        else:
            print(cls_enum, "NO STARTUP")
    else:
        print(cls_enum, "MISSING")

drop_da = unreal.load_asset("/Game/Data/DA/DA_DropItems")
defs = drop_da.get_editor_property("DropTypeToTypeDef")
print("drop types", list(defs.keys()))
