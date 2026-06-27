import unreal
import json

checks = []

def check(name, ok, detail=""):
    checks.append((name, ok, detail))
    print(("OK" if ok else "FAIL"), name, detail)

wave = unreal.load_asset("/Game/Data/DT/Wave/DT_WaveDirectorConfig")
wave_json = json.loads(unreal.DataTableFunctionLibrary.export_data_table_to_json_string(wave))
check("WaveRows", len(wave_json) == 7, str(len(wave_json)))

enemy = unreal.load_asset("/Game/Data/DT/Enemy/DT_EnemyInfo")
enemy_json = json.loads(unreal.DataTableFunctionLibrary.export_data_table_to_json_string(enemy))
ghost = next(r for r in enemy_json if r["Name"] == "Ghost")
boss = next(r for r in enemy_json if r["Name"] == "Boss")
check("GhostHP", ghost["MaxHealth"] == 80.0, str(ghost["MaxHealth"]))
check("BossKB", boss["KnockbackResistance"] >= 0.95, str(boss["KnockbackResistance"]))

level = unreal.load_asset("/Game/Data/DT/Level/DT_LevelUpTable")
level_json = json.loads(unreal.DataTableFunctionLibrary.export_data_table_to_json_string(level))
check("LevelRows", len(level_json) == 20, str(len(level_json)))
check("Level20XP", level_json[-1]["XPRequirement"] == 1800.0, str(level_json[-1]["XPRequirement"]))

evo = unreal.load_asset("/Game/Data/DT/Ability/DT_EvolvedWeapon")
evo_json = json.loads(unreal.DataTableFunctionLibrary.export_data_table_to_json_string(evo))
whip = next(r for r in evo_json if r["Name"] == "Abilities.Weapon.EvolvedWhip")
check("EvoWhipWeapon", "Abilities.Weapon.Whip" in whip["RequiredWeaponTag"], whip["RequiredWeaponTag"])
check("EvoWhipPassive", "Abilities.Passive.HollowHeart" in whip["RequiredPassiveTag"], whip["RequiredPassiveTag"])

passive_dt = unreal.load_asset("/Game/Data/DT/Level/Passive/DT_Spinach")
passive_rows = unreal.DataTableFunctionLibrary.get_data_table_row_names(passive_dt)
check("PassiveSpinachRows", len(passive_rows) == 5, str(len(passive_rows)))

drop_da = unreal.load_asset("/Game/Data/DA/DA_DropItems")
defs = drop_da.get_editor_property("DropTypeToTypeDef")
check("DropXPLarge", unreal.VSDropType.XP_LARGE in defs, str(list(defs.keys())))

wave_da = unreal.load_asset("/Game/Data/DA/DA_WaveDirectorConfig")
check("WaveMaxEnemies", wave_da.get_editor_property("MaxNormalEnemyNum") == 200, str(wave_da.get_editor_property("MaxNormalEnemyNum")))

ct = unreal.load_asset("/Game/Data/CT_PassiveValue")
_, spinach_val = unreal.DataTableFunctionLibrary.evaluate_curve_table_row(ct, "Spinach", 5.0, "validate")
check("PassiveSpinachL5", abs(spinach_val - 0.5) < 0.01, str(spinach_val))

char_da = unreal.load_asset("/Game/Data/DA/DA_CharacterClassInfo")
classes = char_da.get_editor_property("CharacterClassInformation")
check("CharacterCount", len(classes) >= 5, str(len(classes)))

print("SUMMARY", sum(1 for _, ok, _ in checks if ok), "/", len(checks), "passed")
