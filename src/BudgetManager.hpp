#pragma once

#include "RE/T/TESFormID.hpp"

namespace RE
{
	class TESObjectREFR;
}

namespace BBE
{
	class BudgetManager
	{
	public:
		static BudgetManager& Get() noexcept;

		void HandleWorkshop(RE::TESObjectREFR* a_workshop);

	private:
		BudgetManager() = default;

		struct DefaultBudget
		{
			float maxDraws{ 0.0F };
			float maxTriangles{ 0.0F };
		};

		[[nodiscard]] DefaultBudget GetOrCacheDefaults(RE::TESObjectREFR& a_workshop);
		void NotifyExtended(RE::TESObjectREFR& a_workshop, float a_newMaxDraws, float a_newMaxTriangles) const;

		std::unordered_map<RE::TESFormID, DefaultBudget> _defaults;
	};
}
