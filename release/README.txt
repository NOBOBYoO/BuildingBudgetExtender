Building Budget Extender (F4SE) v1.0.0
========================================

Automatically extends settlement build limits when you approach the vanilla cap.
Native F4SE plugin — no ESP required.

REQUIREMENTS
------------
- Fallout 4 Script Extender (F4SE)
- Address Library for F4SE Plugins
- Fallout 4 1.10.980+ / Anniversary Edition

INSTALLATION
------------
1. Install F4SE and Address Library first.
2. Extract so the Data folder merges with your Fallout 4 install.
3. REMOVE or DISABLE the old Papyrus mod (BuildingBudgetExtender.esp).
   Do not run both versions at the same time.
4. Launch the game with f4se_loader.exe.

CONFIGURATION (optional)
------------------------
Edit Data\F4SE\Plugins\BuildingBudgetExtender.ini

  ExtendThreshold=0.95     Extend before hitting 100% (avoids vanilla limit message)
  BudgetIncreaseFactor=1.5 How much each extension adds (based on original budget)
  ShowNotification=true    HUD message when extended

WARNING
-------
Build limits exist to protect performance and save stability.
Use at your own risk, especially on large settlements.

SOURCE & UPDATES
----------------
https://github.com/NOBOBYoO/BuildingBudgetExtender (f4se branch)
