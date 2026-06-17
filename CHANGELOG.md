# Changelog

All notable changes to the **F4SE** release line are documented here. This is separate from the Papyrus mod changelog (v4.x).

## [1.1] - 2026-06-17

### Added

- Optional in-game configuration through [Mod Configuration Menu (MCM)](https://www.nexusmods.com/fallout4/mods/21497) (Pause menu → Mods → Mod Configuration Menu → Building Budget Extender)
- `MaxBudgetMultiplier` — optional cap on how far each settlement's budget can grow (`0` = unlimited; e.g. `10` = up to 10× the original vanilla limit)
- HUD notification when a capped settlement reaches its maximum budget, including the settlement name (once per settlement)
- `ShowExtendNotification` and `ShowCapNotification` INI options — control extend and cap messages separately (`ShowNotification` still sets both)
- MCM **Reset Budget** tool — restores the nearest settlement's max budget to its original value when usage allows (on-screen confirmation)

### Changed

- Extend HUD notifications now include the settlement name

## [1.0.0] - 2026-06-14

### Added

- Initial F4SE release — native C++ rewrite of Building Budget Extender
- Automatic budget extension when draw calls or triangles reach the configured threshold
- Workshop UI refresh without manually closing workshop mode
- Optional `BuildingBudgetExtender.ini` config in `Data\F4SE\Plugins\`
- Support for OG, NG, and AE game runtimes via CommonLibF4

[1.1]: https://github.com/NOBOBYoO/BuildingBudgetExtender/releases/tag/f4se-v1.1
[1.0.0]: https://github.com/NOBOBYoO/BuildingBudgetExtender/releases/tag/f4se-v1.0.0
