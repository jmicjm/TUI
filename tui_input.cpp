#include "tui_input.h"

namespace tui
{
	namespace KEYBOARD
	{
		keyboard_buffer buffer;

#ifdef  TUI_TARGET_SYSTEM_LINUX
		terminal_info term_info;
#endif
	}
}