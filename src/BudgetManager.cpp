#include "BudgetManager.hpp"

#include "Config.hpp"
#include "WorkshopUIRefresh.hpp"

#include "RE/A/ActorValue.hpp"
#include "RE/S/SendHUDMessage.hpp"
#include "RE/T/TESObjectREFR.hpp"

namespace BBE
{
	namespace
	{
		[[nodiscard]] float SanitizeDefault(float a_value, float a_fallback)
		{
			return a_value > 0.0F ? a_value : a_fallback;
		}

		[[nodiscard]] float FloorPositive(float a_value)
		{
			return a_value > 0.0F ? std::floor(a_value) : 0.0F;
		}

		[[nodiscard]] RE::ActorValue* GetActorValues()
		{
			return RE::ActorValue::GetSingleton();
		}
	}

	BudgetManager& BudgetManager::Get() noexcept
	{
		static BudgetManager manager;
		return manager;
	}

	BudgetManager::DefaultBudget BudgetManager::GetOrCacheDefaults(RE::TESObjectREFR& a_workshop)
	{
		const auto formID = a_workshop.GetFormID();
		if (const auto it = _defaults.find(formID); it != _defaults.end()) {
			return it->second;
		}

		const auto& config = Config::Get();
		const auto* actorValues = GetActorValues();
		if (!actorValues) {
			return DefaultBudget{
				.maxDraws = config.defaultMaxDrawsFallback,
				.maxTriangles = config.defaultMaxTrianglesFallback,
			};
		}

		auto defaults = DefaultBudget{
			.maxDraws = a_workshop.GetBaseActorValue(*actorValues->workshopMaxDraws),
			.maxTriangles = a_workshop.GetBaseActorValue(*actorValues->workshopMaxTriangles),
		};

		if (defaults.maxDraws <= 0.0F) {
			defaults.maxDraws = a_workshop.GetActorValue(*actorValues->workshopMaxDraws);
		}
		if (defaults.maxTriangles <= 0.0F) {
			defaults.maxTriangles = a_workshop.GetActorValue(*actorValues->workshopMaxTriangles);
		}

		defaults.maxDraws = SanitizeDefault(defaults.maxDraws, config.defaultMaxDrawsFallback);
		defaults.maxTriangles = SanitizeDefault(defaults.maxTriangles, config.defaultMaxTrianglesFallback);

		_defaults.emplace(formID, defaults);
		return defaults;
	}

	void BudgetManager::NotifyExtended(RE::TESObjectREFR& a_workshop, float a_newMaxDraws, float a_newMaxTriangles) const
	{
		const auto& config = Config::Get();
		if (config.verboseLogging) {
			REX::LogInformation(
				"Extended settlement budget for {:08X}: max draws {:.0f}, max triangles {:.0f}",
				a_workshop.GetFormID(),
				a_newMaxDraws,
				a_newMaxTriangles);
		}

		if (config.showNotification) {
			RE::SendHUDMessage::ShowHUDMessage("[BuildingBudgetExtender] Settlement building budget extended!");
		}
	}

	void BudgetManager::HandleWorkshop(RE::TESObjectREFR* a_workshop)
	{
		if (!a_workshop) {
			return;
		}

		const auto* actorValues = GetActorValues();
		if (!actorValues) {
			return;
		}

		const auto& config = Config::Get();
		const auto defaults = GetOrCacheDefaults(*a_workshop);

		float currentMaxDraws = a_workshop->GetActorValue(*actorValues->workshopMaxDraws);
		float currentMaxTriangles = a_workshop->GetActorValue(*actorValues->workshopMaxTriangles);
		const float currentDraws = a_workshop->GetActorValue(*actorValues->workshopCurrentDraws);
		const float currentTriangles = a_workshop->GetActorValue(*actorValues->workshopCurrentTriangles);

		currentMaxDraws = SanitizeDefault(currentMaxDraws, defaults.maxDraws);
		currentMaxTriangles = SanitizeDefault(currentMaxTriangles, defaults.maxTriangles);

		const float drawsThreshold = currentMaxDraws * config.extendThreshold;
		const float trianglesThreshold = currentMaxTriangles * config.extendThreshold;

		if (currentDraws < drawsThreshold && currentTriangles < trianglesThreshold) {
			return;
		}

		const float drawIncrease = FloorPositive(defaults.maxDraws * config.budgetIncreaseFactor);
		const float triangleIncrease = FloorPositive(defaults.maxTriangles * config.budgetIncreaseFactor);

		const float newMaxDraws = currentMaxDraws + drawIncrease;
		const float newMaxTriangles = currentMaxTriangles + triangleIncrease;

		if (newMaxDraws <= currentMaxDraws && newMaxTriangles <= currentMaxTriangles) {
			return;
		}

		ApplyWorkshopBudgetWithUIRefresh(*a_workshop, newMaxDraws, newMaxTriangles);

		NotifyExtended(*a_workshop, newMaxDraws, newMaxTriangles);
	}
}
