// IconPicker.h
#pragma once

#include "imgui.h"
#include "../Assets/Fonts/IconsFontAwesome7.h"
#include "../Assets/Fonts/IconsFontAwesome7Brands.h"

// The icon definition type used by the generated lists
struct ImGuiIconDef
{
	const char* name;  // e.g. "ICON_FA_ADDRESS_BOOK"
	const char* icon;  // the actual glyph string
};

// Helper: big icon button with tooltip
bool ToolTipButtonBig(const char* icon, const char* name, ImVec2 size = ImVec2(32, 32));

namespace gear
{
	/// Shows an icon picker window with a filter:
	/// - First: ImGuiIconDefs
	/// - Separator
	/// - Then: ImGuiIconBrandsDefs
	///
	/// Clicking an icon copies its macro name to the clipboard.
	/// Pass p_open if you want a closeable window (like ImGui demo windows).
	void ShowIconListView(bool* p_open = nullptr);
}
