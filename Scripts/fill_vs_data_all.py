import unreal
import json

def fill_dt(path, rows):
    dt = unreal.load_asset(path)
    ok = unreal.DataTableFunctionLibrary.fill_data_table_from_json_string(dt, json.dumps(rows))
    saved = unreal.EditorAssetLibrary.save_asset(path, only_if_is_dirty=False)
    print("FILL", path, ok, saved, len(rows))
    return ok and saved

def lvl(desc, dmg, cd=1.0, dur=0.0, area=1.0, proj=1, speed=600.0, pierce=0, spd_mult=1.0):
    return {
        "LevelDescription": desc,
        "BaseDamage": dmg,
        "Cooldown": cd,
        "Duration": dur,
        "Area": area,
        "ProjectileCount": proj,
        "ProjectileSpeed": speed,
        "PierceCount": pierce,
        "SpeedMultiplier": spd_mult,
    }

def rows_named(prefix, specs):
    out = []
    for i, spec in enumerate(specs, start=1):
        row = {"Name": f"{prefix}_L{i}"}
        row.update(spec)
        out.append(row)
    return out

level_rows = []
xp_curve = [0, 15, 35, 60, 80, 110, 150, 200, 260, 330, 410, 500, 600, 720, 850, 1000, 1180, 1380, 1600, 1800]
for i, xp in enumerate(xp_curve, start=1):
    level_rows.append({"Name": f"Level_{i}", "XPRequirement": float(xp)})
fill_dt("/Game/Data/DT/Level/DT_LevelUpTable", level_rows)

fill_dt("/Game/Data/DT/Ability/DT_Whip", rows_named("Whip", [
    lvl("获得神鞭", 10, 1.2, 0.0, 1.0),
    lvl("伤害提升", 14, 1.1, 0.0, 1.1),
    lvl("范围扩大", 18, 1.0, 0.0, 1.3),
    lvl("冷却缩短", 22, 0.9, 0.0, 1.4),
    lvl("满级：横扫强化", 30, 0.8, 0.0, 1.8),
]))

fill_dt("/Game/Data/DT/Ability/DT_Garlic", rows_named("Garlic", [
    lvl("获得大蒜", 5, 1.0, 0.2, 0.8),
    lvl("伤害提升", 7, 1.0, 0.2, 1.0),
    lvl("范围扩大", 9, 0.9, 0.2, 1.2),
    lvl("伤害再提升", 12, 0.9, 0.2, 1.4),
    lvl("满级：腐化光环", 16, 0.8, 0.2, 1.8),
]))

fill_dt("/Game/Data/DT/Ability/DT_Bible", rows_named("Bible", [
    lvl("获得魔法书", 8, 3.0, 3.0, 1.0, 1, 600.0, 0, 1.0),
    lvl("持续时间提升", 10, 2.8, 3.5, 1.0, 1, 600.0, 0, 1.1),
    lvl("投射物+1", 12, 2.6, 4.0, 1.0, 2, 600.0, 0, 1.2),
    lvl("转速提升", 14, 2.4, 4.5, 1.1, 2, 650.0, 0, 1.4),
    lvl("满级：环绕强化", 18, 2.0, 5.0, 1.2, 3, 700.0, 0, 1.6),
]))

fill_dt("/Game/Data/DT/Ability/DT_FireWand", rows_named("FireWand", [
    lvl("获得火焰魔杖", 15, 1.2, 0.0, 1.0, 1, 700.0),
    lvl("伤害提升", 20, 1.1, 0.0, 1.0, 1, 750.0),
    lvl("投射物+1", 24, 1.0, 0.0, 1.0, 2, 750.0),
    lvl("穿透+1", 28, 1.0, 0.0, 1.1, 2, 800.0, 1),
    lvl("满级：烈焰齐射", 35, 0.9, 0.0, 1.2, 3, 850.0, 2),
]))

fill_dt("/Game/Data/DT/Ability/DT_MagicWand", rows_named("MagicWand", [
    lvl("获得强能魔杖", 8, 1.0, 0.0, 1.0, 1, 600.0),
    lvl("伤害提升", 10, 1.0, 0.0, 1.0, 2, 600.0),
    lvl("投射物+1", 12, 0.9, 0.0, 1.0, 3, 650.0),
    lvl("冷却缩短", 14, 0.8, 0.0, 1.0, 4, 650.0),
    lvl("满级：弹幕强化", 16, 0.7, 0.0, 1.0, 5, 700.0),
]))

passive_specs = {
    "Spinach": ["获得菠菜", "伤害+10%", "伤害+20%", "伤害+30%", "满级：伤害+50%"],
    "EmptyTome": ["获得空白之书", "冷却-8%", "冷却-16%", "冷却-24%", "满级：冷却-40%"],
    "Pummarola": ["获得愈伤番茄", "每秒回血+0.2", "每秒回血+0.4", "每秒回血+0.6", "满级：每秒回血+1.0"],
    "HollowHeart": ["获得虚空之心", "最大生命+20%", "最大生命+40%", "最大生命+60%", "满级：最大生命+100%"],
    "Spellbinder": ["获得咒缚盒", "持续时间+10%", "持续时间+20%", "持续时间+30%", "满级：持续时间+50%"],
}
for name, descs in passive_specs.items():
    rows = []
    for i, desc in enumerate(descs, start=1):
        rows.append({"Name": f"{name}_L{i}", "LevelDescription": desc, "BaseDamage": 0, "Cooldown": 1, "Duration": 0, "Area": 1, "ProjectileCount": 1, "ProjectileSpeed": 600, "PierceCount": 0, "SpeedMultiplier": 1})
    fill_dt(f"/Game/Data/DT/Level/Passive/DT_{name}", rows)

evolved_rows = [
    {"Name": "Abilities.Weapon.EvolvedWhip", "RequiredWeaponTag": "Abilities.Weapon.Whip", "bWeaponNeedsMaxLevel": True, "RequiredPassiveTag": "Abilities.Passive.HollowHeart", "bPassiveNeedsMaxLevel": True, "BaseDamage": 45, "Cooldown": 0.7, "Duration": 0.0, "Area": 2.2, "ProjectileCount": 1, "ProjectileSpeed": 600, "PierceCount": -1, "SpeedMultiplier": 1.0},
    {"Name": "Abilities.Weapon.EvolvedGarlic", "RequiredWeaponTag": "Abilities.Weapon.Garlic", "bWeaponNeedsMaxLevel": True, "RequiredPassiveTag": "Abilities.Passive.Pummarola", "bPassiveNeedsMaxLevel": True, "BaseDamage": 25, "Cooldown": 0.8, "Duration": 0.3, "Area": 2.5, "ProjectileCount": 1, "ProjectileSpeed": 600, "PierceCount": 0, "SpeedMultiplier": 1.0},
    {"Name": "Abilities.Weapon.EvolvedBook", "RequiredWeaponTag": "Abilities.Weapon.MagicBook", "bWeaponNeedsMaxLevel": True, "RequiredPassiveTag": "Abilities.Passive.Spellbinder", "bPassiveNeedsMaxLevel": True, "BaseDamage": 20, "Cooldown": 2.0, "Duration": 6.0, "Area": 1.2, "ProjectileCount": 4, "ProjectileSpeed": 700, "PierceCount": 0, "SpeedMultiplier": 1.8},
    {"Name": "Abilities.Weapon.EvolvedFireWand", "RequiredWeaponTag": "Abilities.Weapon.FireWand", "bWeaponNeedsMaxLevel": True, "RequiredPassiveTag": "Abilities.Passive.Spinach", "bPassiveNeedsMaxLevel": True, "BaseDamage": 50, "Cooldown": 0.8, "Duration": 0.0, "Area": 1.3, "ProjectileCount": 4, "ProjectileSpeed": 900, "PierceCount": 2, "SpeedMultiplier": 1.0},
    {"Name": "Abilities.Weapon.EvolvedMagicWand", "RequiredWeaponTag": "Abilities.Weapon.MagicWand", "bWeaponNeedsMaxLevel": True, "RequiredPassiveTag": "Abilities.Passive.EmptyTome", "bPassiveNeedsMaxLevel": True, "BaseDamage": 18, "Cooldown": 0.5, "Duration": 0.0, "Area": 1.0, "ProjectileCount": 8, "ProjectileSpeed": 750, "PierceCount": 1, "SpeedMultiplier": 1.0},
]
fill_dt("/Game/Data/DT/Ability/DT_EvolvedWeapon", evolved_rows)

wave_rows = [
    {"Name": "EarlyGhost", "EnemyType": "Ghost", "StartTime": 0.0, "EndTime": 120.0, "SpawnInterval": 4.0, "EnemyNumPerWave": 6, "bIsElite": False},
    {"Name": "MidGhost", "EnemyType": "Ghost", "StartTime": 120.0, "EndTime": 300.0, "SpawnInterval": 3.0, "EnemyNumPerWave": 8, "bIsElite": False},
    {"Name": "MidGoblin", "EnemyType": "Goblin", "StartTime": 120.0, "EndTime": 300.0, "SpawnInterval": 5.0, "EnemyNumPerWave": 5, "bIsElite": False},
    {"Name": "LateSlime", "EnemyType": "Slime", "StartTime": 300.0, "EndTime": 480.0, "SpawnInterval": 6.0, "EnemyNumPerWave": 4, "bIsElite": False},
    {"Name": "LateBat", "EnemyType": "Bat", "StartTime": 300.0, "EndTime": 480.0, "SpawnInterval": 4.0, "EnemyNumPerWave": 6, "bIsElite": False},
    {"Name": "EliteGhost", "EnemyType": "Ghost", "StartTime": 480.0, "EndTime": 600.0, "SpawnInterval": 3.0, "EnemyNumPerWave": 10, "bIsElite": True},
    {"Name": "BossWindow", "EnemyType": "Boss", "StartTime": 540.0, "EndTime": 600.0, "SpawnInterval": 30.0, "EnemyNumPerWave": 1, "bIsElite": False},
]
fill_dt("/Game/Data/DT/Wave/DT_WaveDirectorConfig", wave_rows)

wave_da = unreal.load_asset("/Game/Data/DA/DA_WaveDirectorConfig")
wave_da.set_editor_property("MaxNormalEnemyNum", 200)
wave_da.set_editor_property("MinSpawnRadius", 1100.0)
wave_da.set_editor_property("MaxSpawnRadius", 1400.0)
unreal.EditorAssetLibrary.save_asset("/Game/Data/DA/DA_WaveDirectorConfig", only_if_is_dirty=False)

drop_da = unreal.load_asset("/Game/Data/DA/DA_DropItems")
defs = drop_da.get_editor_property("DropTypeToTypeDef")
if unreal.VSDropType.XP_SMALL in defs and unreal.VSDropType.XP_LARGE not in defs:
    defs[unreal.VSDropType.XP_LARGE] = defs[unreal.VSDropType.XP_SMALL]
drop_da.set_editor_property("DefaultMagnetRadius", 150.0)
drop_da.set_editor_property("MagnetMoveSpeed", 600.0)
drop_da.set_editor_property("PickupRadius", 60.0)
drop_da.set_editor_property("DropTypeToTypeDef", defs)
unreal.EditorAssetLibrary.save_asset("/Game/Data/DA/DA_DropItems", only_if_is_dirty=False)

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
unreal.EditorAssetLibrary.save_asset("/Game/Data/DA/DA_CharacterClassInfo", only_if_is_dirty=False)

print("ALL_PHASES_DONE")
