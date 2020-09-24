//template of widget that handles user input
//probably the simplest example is tui::radio_button
#include "tui_surface.h"
#include "tui_appearance.h"
#include "tui_active_element.h"
#include "tui_input.h"

struct template_appearance_a
{
	tui::symbol xyz;
	tui::color abc;
	//etc

	template_appearance_a() : template_appearance_a(/*default, default*/) {}
	template_appearance_a(/*symbol xyz, color abc*/) : xyz(xyz), abc(abc) {}

	void setColor(tui::color Color)
	{
		//set colors of members
	}
};

struct template_appearance : tui::appearance
{
protected:
	template_appearance_a active_appearance;
	template_appearance_a inactive_appearance;
public:
	template_appearance()
	{
		inactive_appearance.setColor(COLOR::DARKGRAY);//or something other, depending on your tastes
	}
	template_appearance(template_appearance_a active, template_appearance_a inactive) : active_appearance(active), inactive_appearance(inactive) {}

	void setColor(tui::color Color) override
	{
		active_appearance.setColor(Color);
		inactive_appearance.setColor(Color);
		setAppearanceAction();
	}

	void setAppearance(template_appearance appearance) { setElement(*this, appearance); }//setElement() beside assigning variable calls setAppearanceAction()
	template_appearance getAppearance() const { return *this; }

	void setActiveAppearance(template_appearance_a active) { setElement(active_appearance, active); }
	template_appearance_a getActiveAppearance() const { return active_appearance; }

	void setInactiveAppearance(template_appearance_a inactive) { setElement(inactive_appearance, inactive); }
	template_appearance_a getInactiveAppearance() const { return inactive_appearance; }
};

struct template_widget : tui::surface, tui::active_element, template_appearance
{
private:
	abc x;// eg some data to display, other widgets like scroll etc

	template_appearance_a getCurrentAppearance() const
	{
		if (isActive()) { return active_appearance; }
		else { return inactive_appearance; }
	}

	/*
	after calling output::draw(*this) or other.insertSurface(*this)
	virtual functions are called in this order:
		resizeAction()
		updateAction()
		drawAction()
	*/

	//called if inherited surface changed its size
	//for example by output::draw(*this) or other.insertSurface(*this) or other.updateSurfaceSize(*this)
	void resizeAction() override 
	{
		//resize members
	}
	//called by output::draw(*this) or other.insertSurface(*this)
	void updateAction() override 
	{
		update();
	}
	//called by output::draw(*this) or other.insertSurface(*this)
	void drawAction(surface::action_proxy proxy) override
	{
		//redraw
	}

	//caled by everything from iherited appearance
	void setAppearanceAction() override 
	{ 
		//set appearance of members
	}

	//called by *this.activate()
	void activationAction() override 
	{
		//activate members
	}
	//called by *this.deactivate()
	void deactivationAction() override 
	{
		//deactivate members
	}

public:
	short key_xxx = /**/; //eg key_up, key_down, key_next, key_prev
	short key_yyy = /**/; //etc

	void update()
	{
		if (isActive())
		{
			if (input::isKeyPressed(key_xxx)) 
			{
				//action on key_xxx
			}
			if (input::isKeyPressed(key_yyy)) 
			{
				//action on key_yyy
			}
		}
	}
};