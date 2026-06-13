# Nexus Mods — page copy (F4SE v1.0.0)

Use this as a starting point for the Nexus description. Adjust tags/categories as needed.

---

## Summary

Native **F4SE** rewrite of Building Budget Extender. Automatically raises settlement build limits when you approach the cap — no ESP, no Papyrus scripts, no manual console commands.

**This is v1.0 of the F4SE version**, separate from the Papyrus mod (v4.x). Do not run both.

## Requirements

- [Fallout 4 Script Extender (F4SE)](https://f4se.silverlock.org/)
- [Address Library for F4SE Plugins](https://www.nexusmods.com/fallout4/mods/47327)

## Installation

1. Install F4SE and Address Library.
2. Install this mod (Vortex/Mod Organizer 2 or manual).
3. **Remove or disable `BuildingBudgetExtender.esp`** from the old Papyrus version.
4. Launch with `f4se_loader.exe`.

Manual paths:

```
Data/F4SE/Plugins/BuildingBudgetExtender.dll
Data/F4SE/Plugins/BuildingBudgetExtender.ini
```

## How it works

When draw calls or triangles reach **95%** of a settlement's limit (configurable), the plugin increases the maximum by `floor(originalDefault × 1.5)` — same formula as the Papyrus mod.

The workshop UI updates automatically so you can keep building.

## Configuration

Optional INI next to the DLL: `Data/F4SE/Plugins/BuildingBudgetExtender.ini`

| Setting | Default | Description |
|---------|---------|-------------|
| ExtendThreshold | 0.95 | Extend before hitting 100% |
| BudgetIncreaseFactor | 1.5 | Extension size multiplier |
| ShowNotification | true | HUD message on extend |

## Upgrading from the Papyrus version

1. Disable/remove `BuildingBudgetExtender.esp` and its scripts.
2. Install this F4SE plugin.
3. Existing save data is fine — budgets are stored on workshop refs as before.

## Warning

Settlement build limits exist for performance and stability. Exceeding them is at your own risk.

## Source

https://github.com/NOBOBYoO/BuildingBudgetExtender (branch: `f4se`)

---

## Suggested Nexus metadata

- **Category:** Gameplay / Settlements (or Modder's Resource if you prefer)
- **Tags:** F4SE, Settlements, Building
- **Version:** 1.0.0
- **Main file:** `BuildingBudgetExtender_F4SE_v1.0.0.zip`

## Optional follow-ups

1. **Separate Nexus page** vs updating the existing Papyrus mod page — recommend a **new page** titled "Building Budget Extender - F4SE" and add a note on the old page pointing here.
2. **GitHub Release** on the `f4se` branch with the same zip attached.
3. **FOMOD** — only worth it if you want a one-click "disable ESP" helper; the zip is small enough for manual install.
4. **Compatibility note** on Nexus: compatible with Workshop Framework; do not run alongside the Papyrus BBE ESP.
