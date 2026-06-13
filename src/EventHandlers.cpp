#include "EventHandlers.hpp"

#include "BudgetManager.hpp"

#include "F4SE/Events.hpp"
#include "F4SE/MessagingInterface.hpp"

#include "RE/M/MenuOpenCloseEvent.hpp"
#include "RE/P/PlayerCharacter.hpp"
#include "RE/U/UI.hpp"
#include "RE/W/Workshop.hpp"
#include "RE/W/Workshop_ItemPlacedEvent.hpp"

using namespace std::string_view_literals;

namespace BBE
{
	namespace
	{
		void OnWorkshopMenuOpened()
		{
			const auto* player = RE::PlayerCharacter::GetSingleton();
			if (!player) {
				return;
			}

			auto* workshop = RE::Workshop::FindNearestValidWorkshop(*player);
			BudgetManager::Get().HandleWorkshop(workshop);
		}

		void RegisterGameEvents()
		{
			F4SE::Events::RegisterForInlineEvent<RE::Workshop::ItemPlacedEvent>(
				[](const RE::Workshop::ItemPlacedEvent& a_event) {
					BudgetManager::Get().HandleWorkshop(a_event.workshopRef.get());
					return RE::BSContainer::ForEachResult::kContinue;
				});

			auto* ui = RE::UI::GetSingleton();
			if (ui) {
				auto* menuEvents = static_cast<RE::BSTEventSource<RE::MenuOpenCloseEvent>*>(ui);
				F4SE::Events::RegisterForInlineEvent<RE::MenuOpenCloseEvent>(
					menuEvents,
					[](const RE::MenuOpenCloseEvent& a_event) {
						if (a_event.opening && a_event.menuName == "WorkshopMenu"sv) {
							OnWorkshopMenuOpened();
						}
						return RE::BSContainer::ForEachResult::kContinue;
					});
			}

			REX::LogInformation("BuildingBudgetExtender event handlers registered.");
		}

		void F4SE_API MessageHandler(F4SE::MessagingInterface::Message* a_message)
		{
			if (!a_message) {
				return;
			}

			if (a_message->GetType() != F4SE::MessagingInterface::MessageType::kGameDataReady) {
				return;
			}

			RegisterGameEvents();
		}
	}

	void RegisterEventHandlers()
	{
		const auto messaging = F4SE::GetMessagingInterface();
		messaging->RegisterListener(REX::NotNull<F4SE::MessagingInterface::EventCallback*>{ MessageHandler });
	}
}
