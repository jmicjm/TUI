//this file contains group that could be used to simplyfy content displaying/positioning
#pragma once
#include "tui_surface.h"

#include <vector>


namespace tui
{
	struct group_entry
	{
		surface* surf;
		surface::color_override c_override;
		surface::color_transparency_override co_override;

		group_entry(surface* surf) : surf(surf) {}
		group_entry(
			surface* surf,
			surface::color_transparency_override co_override
		)
			: surf(surf),
			co_override(co_override) {}
		group_entry(
			surface* surf,
			surface::color_override c_override,
			surface::color_transparency_override co_override = surface::color_transparency_override()
		)
			: surf(surf),
			c_override(c_override),
			co_override(co_override) {}
	};

	struct group : surface, private std::vector<group_entry>
	{
	private:
		void drawAction(action_proxy proxy) override
		{
			surface::clear();
			for (auto i : *this)
			{
				insertSurface(*i.surf, i.c_override, i.co_override);
			}
		}
	public:
		group(){}
		group(std::vector<group_entry> entries) : std::vector<group_entry>(entries) {}

		using std::vector<group_entry>::size;
		using std::vector<group_entry>::operator[];
		using std::vector<group_entry>::push_back;
		using std::vector<group_entry>::pop_back;
		using std::vector<group_entry>::insert;
		using std::vector<group_entry>::begin;
		using std::vector<group_entry>::end;
		using std::vector<group_entry>::clear;
		using std::vector<group_entry>::erase;
	};
}