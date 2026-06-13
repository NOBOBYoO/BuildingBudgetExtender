# BuildingBudgetExtender (F4SE)

**Version 1.0.0** — native F4SE rewrite of [Building Budget Extender](https://github.com/NOBOBYoO/BuildingBudgetExtender). This is a separate release line from the Papyrus mod (v4.x).

Automatically raises a settlement's build budget when you approach the vanilla limit. No ESP, no Papyrus scripts.

## Requirements

- [Fallout 4 Script Extender (F4SE)](https://f4se.silverlock.org/)
- [Address Library for F4SE Plugins](https://www.nexusmods.com/fallout4/mods/47327)
- Fallout 4 **1.10.980+** / Anniversary Edition (tested on **1.11.221.0**)

## What it does

Fallout 4 limits how much you can build in each settlement using two hidden counters:

- **Draw calls** (`WorkshopMaxDraws` / `WorkshopCurrentDraws`)
- **Triangles** (`WorkshopMaxTriangles` / `WorkshopCurrentTriangles`)

When either counter reaches **95%** of the settlement's maximum (configurable), the plugin increases the maximum by `floor(originalDefault × BudgetIncreaseFactor)` — the same formula as the Papyrus mod (default factor **1.5**).

It listens for:

1. **Workshop item placed** — extends when placing pushes usage past the threshold
2. **Workshop menu opened** — extends when entering build mode at an already-full settlement

The workshop UI refreshes automatically so you can keep building without manually exiting workshop mode.

## Installation

1. Install **F4SE** and **Address Library**.
2. Extract the release archive so `Data` merges with your Fallout 4 folder, or copy manually:
   - `BuildingBudgetExtender.dll` → `Fallout 4\Data\F4SE\Plugins\`
   - `BuildingBudgetExtender.ini` → `Fallout 4\Data\F4SE\Plugins\` (optional)
3. **Disable or remove the Papyrus version** (`BuildingBudgetExtender.esp`) — do not run both.
4. Launch via **`f4se_loader.exe`**.

## Configuration

Edit `Fallout 4\Data\F4SE\Plugins\BuildingBudgetExtender.ini`:

| Setting | Default | Description |
|---------|---------|-------------|
| `BudgetIncreaseFactor` | `1.5` | Multiplier on the settlement's original budget |
| `DefaultMaxDrawsFallback` | `100000` | Fallback when a workshop reports invalid draw limits |
| `DefaultMaxTrianglesFallback` | `100000` | Fallback when a workshop reports invalid triangle limits |
| `ExtendThreshold` | `0.95` | Extend at this fraction of max (before the vanilla limit message) |
| `ShowNotification` | `true` | On-screen HUD message when extended |
| `VerboseLogging` | `true` | Write details to the F4SE log |

## Improvements over the Papyrus version

| Papyrus mod (v4.x) | F4SE plugin (v1.x) |
|--------------------|--------------------|
| Requires `BuildingBudgetExtender.esp` + scripts | Standalone `.dll` only |
| Registers events on every workshop individually | One native event hook for all settlements |
| Runs on the Papyrus VM | Native C++ |
| Hard-coded settings | Optional INI config beside the DLL |

## Important note

Bethesda's build limits exist to protect performance and save stability. Extending them is **at your own risk**, same as the original mod.

## Building from source

Requires Visual Studio 2026 (or matching generator in `CMakePresets.json`), CMake 4.3+, vcpkg, and the `VCPKG_ROOT` and `XSE_FO4_GAME_PATH` environment variables set.

```powershell
git submodule update --init --recursive
cmake --preset build-release-msvc-msvc
cmake --build --preset release-msvc-msvc
```

Build output is copied to your game folder automatically when `XSE_FO4_GAME_PATH` is set.

## License

MIT License — see [LICENSE](LICENSE).
