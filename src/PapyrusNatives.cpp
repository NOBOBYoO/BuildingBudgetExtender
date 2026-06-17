#include "PapyrusNatives.hpp"

#include "BudgetManager.hpp"
#include "Config.hpp"

#include "F4SE/API.hpp"
#include "F4SE/PapyrusInterface.hpp"

#include "RE/B/BSScriptUtil_External.hpp"

#include <string_view>

using namespace std::string_view_literals;

namespace BBE
{
	namespace
	{
		constexpr auto SCRIPT_NAME = "BuildingBudgetExtenderNative"sv;

		void ApplyFromMcm(RE::BSScript::StaticTag)
		{
			Config::Get().LoadFromMcmIfPresent();
		}

		void ResetBudget(RE::BSScript::StaticTag)
		{
			BudgetManager::Get().TryResetBudget();
		}

		bool Register(RE::BSScript::IVirtualMachine* a_vm)
		{
			if (!a_vm) {
				return false;
			}

			RE_REGISTER_VM_FUNCTION(*a_vm, SCRIPT_NAME, ApplyFromMcm);
			RE_REGISTER_VM_FUNCTION(*a_vm, SCRIPT_NAME, ResetBudget);
			return true;
		}
	}

	void RegisterPapyrusNatives()
	{
		const auto papyrus = F4SE::GetPapyrusInterface();
		papyrus->Register(REX::NotNull<F4SE::PapyrusInterface::RegisterFunctions*>{ Register });
	}
}
