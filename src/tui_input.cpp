#include "tui_input.h"

namespace tui
{
	namespace input
	{
		keyboard_buffer buffer;

#ifdef  TUI_TARGET_SYSTEM_LINUX
		terminal_info term_info;
#endif
	}
}