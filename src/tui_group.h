#pragma once
#include "tui_surface.h"

#include <vector>


namespace tui
{
	struct group : surface, private std::vector<surface*>
	{
	private:
		void drawAction() override
		{
			clear();

			for (auto i : *(std::vector<surface*>*)this)
			{
				insertSurface(*i);
			}
		}
	public:
		group(){}
		group(surface* surf) : std::vector<surface*>({ surf }) {}
		group(std::vector<surface*> surfaces) : std::vector<surface*>(surfaces) {}

		using std::vector<surface*>::size;
		using std::vector<surface*>::operator[];
		using std::vector<surface*>::push_back;
		using std::vector<surface*>::pop_back;
		using std::vector<surface*>::insert;
		using std::vector<surface*>::begin;
		using std::vector<surface*>::end;
		using std::vector<surface*>::clear;
		using std::vector<surface*>::erase;
	};
}