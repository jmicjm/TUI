//template of widget
//the simplest example is tui::rectangle
#include "tui_surface.h"
#include "tui_appearance.h"

struct template_appearance : tui::appearance
{
protected:
	tui::symbol xyz;
	tui::color abc;
	//etc
public:
	template_appearance() : rectangle_appearance(/*default, default*/) {}
	template_appearance(/*symbol xyz, color abc*/) : xyz(xyz), abc(abc) {}

	void setColor(tui::color Color) override
	{
		//set colors of members
		setAppearanceAction();
	}

	void setAppearance(template_appearance appearance) { setElement(*this, appearance); } //setElement() beside assigning variable calls setAppearanceAction()
	template_appearance getAppearance() const { return *this; }

	void setSymbolXyz(tui::symbol x) { setElement(xyz, x); }
	tui::symbol getSymbolXyz() const { return xyz; }

	void setColorAbc(tui::color a) { setElement(abc, a); }
	tui::color getColorAbc() const { return abc; }
	//etc
};

struct template_widget : tui::surface, template_appearance
{
private:
	abc x;// eg some data to display,

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
	void drawAction(action_proxy proxy) override
	{
		//redraw
	}

	//caled by everything from iherited appearance
	void setAppearanceAction() override 
	{
		//set members apperance
	}
public:

};