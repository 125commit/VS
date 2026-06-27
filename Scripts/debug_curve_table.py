import unreal

ct = unreal.load_asset("/Game/Data/CT_PassiveValue")
print("methods", [x for x in dir(ct) if "curve" in x.lower() or "row" in x.lower() or "table" in x.lower()])
print("funcs", [x for x in dir(unreal) if "CurveTable" in x])

passive_values = {
    "Spinach": [0.10, 0.20, 0.30, 0.40, 0.50],
    "EmptyTome": [0.08, 0.16, 0.24, 0.32, 0.40],
    "Pummarola": [0.20, 0.40, 0.60, 0.80, 1.00],
    "HollowHeart": [0.20, 0.40, 0.60, 0.80, 1.00],
    "Spellbinder": [0.10, 0.20, 0.30, 0.40, 0.50],
}

for row_name, values in passive_values.items():
    try:
        curve = unreal.DataTableFunctionLibrary.evaluate_curve_table_row(ct, row_name, 1.0)
        print("existing", row_name, curve)
    except Exception as e:
        print("eval fail", row_name, e)
