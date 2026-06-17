#pragma once

#include "RE/T/TESFormID.hpp"

#include <unordered_map>
#include <unordered_set>

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
		void TryResetBudget();

	private:
		BudgetManager() = default;

		struct DefaultBudget
		{
			float maxDraws{ 0.0F };
			float maxTriangles{ 0.0F };
		};

		[[nodiscard]] DefaultBudget GetOrCacheDefaults(RE::TESObjectREFR& a_workshop);
		void NotifyExtended(RE::TESObjectREFR& a_workshop, float a_newMaxDraws, float a_newMaxTriangles) const;
		void NotifyCapReached(RE::TESObjectREFR& a_workshop);

		std::unordered_map<RE::TESFormID, DefaultBudget> _defaults;
		std::unordered_set<RE::TESFormID> _capNotified;
	};
}
