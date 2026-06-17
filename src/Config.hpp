#pragma once

namespace BBE
{
	struct Config
	{
		// How much of each settlement's original budget is added when the limit is hit.
		// 1.5 matches the original Papyrus mod (v4.3.1).
		float budgetIncreaseFactor{ 1.5F };

		// Fallback defaults when a workshop reports non-positive base limits
		// (helps custom settlement mods, same as the Papyrus version).
		float defaultMaxDrawsFallback{ 100000.0F };
		float defaultMaxTrianglesFallback{ 100000.0F };

		// Extend when usage reaches this fraction of the current max (0.95 = at 95%, before the vanilla limit message).
		float extendThreshold{ 0.95F };

		// Cap each settlement's max budget at originalDefault × this value (e.g. 10 = up to 10× vanilla).
		// 0 = unlimited (keep extending indefinitely).
		float maxBudgetMultiplier{ 0.0F };

		// Show an on-screen message when the budget is extended.
		bool showExtendNotification{ true };

		// Show an on-screen message when a capped settlement reaches its max budget.
		bool showCapNotification{ true };

		// Write a log line whenever a settlement budget is extended.
		bool verboseLogging{ true };

		static Config& Get() noexcept;
		void LoadFromFile(const std::filesystem::path& a_path);
		void LoadFromMcmIfPresent();
		void Reload();

	private:
		std::filesystem::path _configPath;
	};
}
