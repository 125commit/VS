import unreal

wave = unreal.load_asset("/Game/Data/DA/DA_WaveDirectorConfig")
wave.set_editor_property("MaxNormalEnemyNum", 200)
print("wave max", wave.get_editor_property("MaxNormalEnemyNum"))

ability = unreal.load_asset("/Game/Data/DA/DA_AbilityInfo")
infos = ability.get_editor_property("AbilityInformation")
print("ability count", len(infos))
if len(infos) > 0:
    first = infos[0]
    try:
        first.set_editor_property("MaxLevel", 5)
        print("set max level ok")
    except Exception as e:
        print("set max level fail", e)

char_da = unreal.load_asset("/Game/Data/DA/DA_CharacterClassInfo")
classes = char_da.get_editor_property("CharacterClassInformation")
print("class keys", list(classes.keys()))
for k, v in classes.items():
    try:
        sa = v.get_editor_property("StartupAbilities")
        print(k, "startup count", len(sa))
    except Exception as e:
        print(k, "read fail", e)
