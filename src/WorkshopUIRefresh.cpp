#include "WorkshopUIRefresh.hpp"

#include "F4SE/API.hpp"
#include "F4SE/Menus.hpp"

#include "RE/A/ActorValue.hpp"
#include "RE/B/BSPointerHandle.hpp"
#include "RE/W/Workshop.hpp"
#include "RE/W/WorkshopMenu.hpp"
#include "RE/W/Workshop_PlacementStatusEvent.hpp"

using namespace std::string_view_literals;

namespace BBE
{
	namespace
	{
		void InvalidateWorkshopMenuCache()
		{
			const auto menuPtr = F4SE::Menus::GetMenuInstance(RE::WorkshopMenu::MENU_NAME);
			if (!menuPtr) {
				return;
			}

			auto* menu = static_cast<RE::WorkshopMenu*>(menuPtr.get());
			if (!menu) {
				return;
			}

			menu->lastBudget = -1.0L;
			menu->UpdateButtonText();
			menu->CheckAndSetItemForPlacement();

			if (auto* source = RE::Workshop::PlacementStatusEvent::GetEventSource()) {
				const RE::Workshop::PlacementStatusEvent event{};
				source->Notify(event);
			}
		}

		void ApplyBudgetValues(RE::TESObjectREFR& a_workshop, float a_newMaxDraws, float a_newMaxTriangles)
		{
			const auto* actorValues = RE::ActorValue::GetSingleton();
			if (!actorValues) {
				return;
			}

			a_workshop.SetActorValue(*actorValues->workshopMaxDraws, a_newMaxDraws);
			a_workshop.SetActorValue(*actorValues->workshopMaxTriangles, a_newMaxTriangles);
		}
	}

	void ApplyWorkshopBudgetWithUIRefresh(RE::TESObjectREFR& a_workshop, float a_newMaxDraws, float a_newMaxTriangles)
	{
		const bool inWorkshop = F4SE::Menus::IsMenuOpen(RE::WorkshopMenu::MENU_NAME);
		if (!inWorkshop) {
			ApplyBudgetValues(a_workshop, a_newMaxDraws, a_newMaxTriangles);
			return;
		}

		const auto workshopHandle = a_workshop.GetHandle();
		F4SE::GetTaskInterface()->AddUITask([workshopHandle, a_newMaxDraws, a_newMaxTriangles]() {
			const auto ref = workshopHandle.get();
			if (!ref) {
				return;
			}

			if (!F4SE::Menus::IsMenuOpen(RE::WorkshopMenu::MENU_NAME)) {
				ApplyBudgetValues(*ref, a_newMaxDraws, a_newMaxTriangles);
				return;
			}

			// Match the original Papyrus mod: close workshop, apply values, reopen.
			RE::Workshop::RequestExitWorkshop(true);
			ApplyBudgetValues(*ref, a_newMaxDraws, a_newMaxTriangles);
			RE::Workshop::StartWorkshop(ref.get());
			InvalidateWorkshopMenuCache();
		});
	}
}
