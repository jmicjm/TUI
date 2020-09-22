//this file contains active_element that is inherited by input handling widgets
#pragma once

namespace tui
{
	struct active_element
	{
	private:
		bool active = false;
	protected:
		virtual void activationAction() {}
		virtual void disactivationAction() {}
	public:
		virtual ~active_element() {}
		void activate()
		{
			active = true;
			activationAction();
		}
		void disactivate()
		{
			active = false;
			disactivationAction();
		}
		bool isActive() const { return active; }
	};
}