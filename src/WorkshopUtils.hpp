#pragma once

namespace RE
{
	class TESObjectREFR;
}

namespace BBE
{
	[[nodiscard]] RE::TESObjectREFR* ResolveWorkshop(RE::TESObjectREFR* a_consoleRef);
	[[nodiscard]] std::string GetSettlementLabel(const RE::TESObjectREFR& a_workshop);
}
