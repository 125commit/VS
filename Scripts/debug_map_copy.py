import unreal

drop_da = unreal.load_asset("/Game/Data/DA/DA_DropItems")
defs = drop_da.get_editor_property("DropTypeToTypeDef")
small = defs[unreal.VSDropType.XP_SMALL]
print("small class", small.get_editor_property("DropActorClass"))
try:
    defs[unreal.VSDropType.XP_LARGE] = small
    drop_da.set_editor_property("DropTypeToTypeDef", defs)
    print("added xplarge ok")
except Exception as e:
    print("add xplarge fail", e)

classes = char_da = unreal.load_asset("/Game/Data/DA/DA_CharacterClassInfo")
info_map = char_da.get_editor_property("CharacterClassInformation")
antonio = info_map[unreal.CharacterClass.ANTONIO]
try:
    info_map[unreal.CharacterClass.POE] = antonio
    char_da.set_editor_property("CharacterClassInformation", info_map)
    print("added poe ok")
except Exception as e:
    print("add poe fail", e)
