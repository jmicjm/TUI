#pragma once
#include "tui_text_utils.h"

namespace tui
{
	struct appearance
	{
	protected:
		virtual void setAppearanceAction() {}
	public:
		virtual void setColor(color Color) = 0;
	};
}