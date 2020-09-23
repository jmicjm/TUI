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
		virtual void deactivationAction() {}
	public:
		virtual ~active_element() {}
		void activate()
		{
			active = true;
			activationAction();
		}
		void deactivate()
		{
			active = false;
			deactivationAction();
		}
		bool isActive() const { return active; }
	};
}