import unreal

cls = unreal.load_class(None, "/Game/AbilitySystem/Ability/Passive/BP_Passive_Spinach.BP_Passive_Spinach_C")
cdo = unreal.get_default_object(cls)
mag = cdo.get_editor_property("PassiveMagnitude")
print("mag dir", [x for x in dir(mag) if not x.startswith('_')])
print("mag", mag)
print("curve", mag.get_editor_property("curve"))
curve = mag.get_editor_property("curve")
print("curve dir", [x for x in dir(curve) if not x.startswith('_')])
for prop in ["curve_table", "row_name", "editor_curve_data", "external_curve"]:
    try:
        print(prop, curve.get_editor_property(prop))
    except Exception as e:
        print(prop, "err", e)
