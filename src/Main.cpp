#include "BudgetManager.hpp"
#include "Config.hpp"
#include "EventHandlers.hpp"
#include "PapyrusNatives.hpp"

F4SE_PLUGIN_LOAD(const F4SE::LoadInterface* a_f4se)
{
	F4SE::Init(a_f4se);

	const auto configPath =
		std::filesystem::path{ F4SE::ROOT_DIRECTORY_PATH } / F4SE::PLUGINS_DIRECTORY_PATH / "BuildingBudgetExtender.ini";
	BBE::Config::Get().LoadFromFile(configPath);

	BBE::RegisterEventHandlers();
	BBE::RegisterPapyrusNatives();

	const auto& config = BBE::Config::Get();
	if (config.maxBudgetMultiplier > 0.0F) {
		REX::LogInformation(
			"BuildingBudgetExtender loaded (factor {:.2f}, threshold {:.0f}%, max budget {:.1f}×). Config: {}",
			config.budgetIncreaseFactor,
			config.extendThreshold * 100.0F,
			config.maxBudgetMultiplier,
			configPath.string());
	} else {
		REX::LogInformation(
			"BuildingBudgetExtender loaded (factor {:.2f}, threshold {:.0f}%). Config: {}",
			config.budgetIncreaseFactor,
			config.extendThreshold * 100.0F,
			configPath.string());
	}

	return true;
}
