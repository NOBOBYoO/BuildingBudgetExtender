#include "BudgetManager.hpp"

#include "Config.hpp"
#include "WorkshopUIRefresh.hpp"
#include "WorkshopUtils.hpp"

#include "RE/A/ActorValue.hpp"
#include "RE/S/SendHUDMessage.hpp"
#include "RE/T/TESObjectREFR.hpp"

#include <format>

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

		void ShowHudMessage(std::string_view a_message)
		{
			RE::SendHUDMessage::ShowHUDMessage(std::string{ a_message }.c_str());
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
		const auto settlement = GetSettlementLabel(a_workshop);

		if (config.verboseLogging) {
			REX::LogInformation(
				"Extended settlement budget for {} ({:08X}): max draws {:.0f}, max triangles {:.0f}",
				settlement,
				a_workshop.GetFormID(),
				a_newMaxDraws,
				a_newMaxTriangles);
		}

		if (config.showExtendNotification) {
			const auto message = std::format(
				"[BuildingBudgetExtender] {}: building budget extended!",
				settlement);
			RE::SendHUDMessage::ShowHUDMessage(message.c_str());
		}
	}

	void BudgetManager::NotifyCapReached(RE::TESObjectREFR& a_workshop)
	{
		const auto formID = a_workshop.GetFormID();
		if (_capNotified.contains(formID)) {
			return;
		}
		_capNotified.insert(formID);

		const auto& config = Config::Get();
		const auto settlement = GetSettlementLabel(a_workshop);

		if (config.verboseLogging) {
			REX::LogInformation(
				"Settlement budget reached max cap ({:.1f}×) for {} ({:08X})",
				config.maxBudgetMultiplier,
				settlement,
				formID);
		}

		if (config.showCapNotification) {
			const auto message = std::format(
				"[BuildingBudgetExtender] {}: building budget at maximum ({:.0f}x)!",
				settlement,
				config.maxBudgetMultiplier);
			RE::SendHUDMessage::ShowHUDMessage(message.c_str());
		}
	}

	void BudgetManager::TryResetBudget()
	{
		auto* workshop = ResolveWorkshop(nullptr);
		if (!workshop) {
			ShowHudMessage("[BuildingBudgetExtender] No workshop found. Stand in a settlement.");
			return;
		}

		const auto* actorValues = GetActorValues();
		if (!actorValues) {
			ShowHudMessage("[BuildingBudgetExtender] Could not read workshop budget.");
			return;
		}

		const auto defaults = GetOrCacheDefaults(*workshop);
		const auto settlement = GetSettlementLabel(*workshop);
		const float currentDraws = workshop->GetActorValue(*actorValues->workshopCurrentDraws);
		const float currentTriangles = workshop->GetActorValue(*actorValues->workshopCurrentTriangles);

		if (currentDraws > defaults.maxDraws || currentTriangles > defaults.maxTriangles) {
			ShowHudMessage(std::format(
				"[BuildingBudgetExtender] Cannot reset {} — usage exceeds original budget.",
				settlement));
			return;
		}

		ApplyWorkshopBudgetWithUIRefresh(*workshop, defaults.maxDraws, defaults.maxTriangles);
		_capNotified.erase(workshop->GetFormID());

		ShowHudMessage(std::format(
			"[BuildingBudgetExtender] {} reset to original budget.",
			settlement));
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

		float maxBudgetDraws = 0.0F;
		float maxBudgetTriangles = 0.0F;
		if (config.maxBudgetMultiplier > 0.0F) {
			maxBudgetDraws = defaults.maxDraws * config.maxBudgetMultiplier;
			maxBudgetTriangles = defaults.maxTriangles * config.maxBudgetMultiplier;

			if (currentMaxDraws >= maxBudgetDraws && currentMaxTriangles >= maxBudgetTriangles) {
				NotifyCapReached(*a_workshop);
				return;
			}
		}

		const float drawIncrease = FloorPositive(defaults.maxDraws * config.budgetIncreaseFactor);
		const float triangleIncrease = FloorPositive(defaults.maxTriangles * config.budgetIncreaseFactor);

		float newMaxDraws = currentMaxDraws + drawIncrease;
		float newMaxTriangles = currentMaxTriangles + triangleIncrease;

		if (config.maxBudgetMultiplier > 0.0F) {
			newMaxDraws = std::min(newMaxDraws, maxBudgetDraws);
			newMaxTriangles = std::min(newMaxTriangles, maxBudgetTriangles);
		}

		if (newMaxDraws <= currentMaxDraws && newMaxTriangles <= currentMaxTriangles) {
			return;
		}

		ApplyWorkshopBudgetWithUIRefresh(*a_workshop, newMaxDraws, newMaxTriangles);

		const bool atCap = config.maxBudgetMultiplier > 0.0F &&
			newMaxDraws >= maxBudgetDraws && newMaxTriangles >= maxBudgetTriangles;

		if (atCap) {
			NotifyCapReached(*a_workshop);
		} else {
			NotifyExtended(*a_workshop, newMaxDraws, newMaxTriangles);
		}
	}
}
