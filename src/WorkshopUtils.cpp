#include "WorkshopUtils.hpp"

#include "RE/P/PlayerCharacter.hpp"
#include "RE/T/TESFullName.hpp"
#include "RE/T/TESObjectREFR.hpp"
#include "RE/W/Workshop.hpp"

#include <format>

namespace BBE
{
	RE::TESObjectREFR* ResolveWorkshop(RE::TESObjectREFR* a_consoleRef)
	{
		if (a_consoleRef) {
			return a_consoleRef;
		}

		const auto* player = RE::PlayerCharacter::GetSingleton();
		if (!player) {
			return nullptr;
		}

		return RE::Workshop::FindNearestValidWorkshop(*player);
	}

	std::string GetSettlementLabel(const RE::TESObjectREFR& a_workshop)
	{
		if (const auto* location = a_workshop.GetCurrentLocation()) {
			if (const auto name = RE::TESFullName::GetFormFullName(location)) {
				if (!name->empty()) {
					return std::string{ name->c_str() };
				}
			}
		}

		if (const auto name = RE::TESFullName::GetFormFullOrDisplayName(&a_workshop)) {
			if (!name->empty()) {
				return std::string{ name->c_str() };
			}
		}

		return std::format("{:08X}", a_workshop.GetFormID());
	}
}
