import unreal

char_da = unreal.load_asset("/Game/Data/DA/DA_CharacterClassInfo")
info_map = char_da.get_editor_property("CharacterClassInformation")
poe = info_map[unreal.CharacterClass.POE]
garlic = unreal.load_class(None, "/Game/AbilitySystem/Ability/Offensive/BP_GarlicAbility.BP_GarlicAbility_C")
try:
    poe.set_editor_property("StartupAbilities", [garlic])
    info_map[unreal.CharacterClass.POE] = poe
    char_da.set_editor_property("CharacterClassInformation", info_map)
    print("set poe startup ok", info_map[unreal.CharacterClass.POE].get_editor_property("StartupAbilities")[0].get_name())
except Exception as e:
    print("set poe startup fail", e)
