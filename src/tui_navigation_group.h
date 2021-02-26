//this file contains navigation_group that could be used to simplyfy content navigation
#pragma once
#include "tui_active_element.h"
#include "tui_input.h"

#include <vector>
#include <algorithm>
#include <functional>
#include <cmath>
#include <float.h>

namespace tui
{
	struct navigation_group_entry
	{
		active_element* element;
		std::function<bool()> unblock_check;//should return true to unblock, if not present blocking wont be used
		std::function<bool()> block_check;//should return true to block

		navigation_group_entry(
			active_element* element,
			std::function<bool()> unblock_check = nullptr,
			std::function<bool()> block_check = nullptr
		)
			: element(element),
			unblock_check(unblock_check),
			block_check(block_check) {}
	};

	struct navigation_group : active_element, private std::vector<navigation_group_entry>
	{
	private:
		size_t m_selected = 0;
		bool m_blocked = false;
		bool m_wrap_around = true;
		bool m_index_based = true;

		void disable(int except = -1)
		{
			for (int i = 0; i < size(); i++)
			{
				if ((*this)[i].element != nullptr)
				{
					if ((*this)[i].element->isActive() && i != except)
					{
						(*this)[i].element->deactivate();
					}
				}
			}
		}
		void enable()
		{
			m_selected = size() > 0 ? std::min(m_selected, size()-1) : 0;
			disable(m_selected);

			if (size() > 0)
			{
				if ((*this)[m_selected].element != nullptr)
				{
					if (!(*this)[m_selected].element->isActive())
					{
						(*this)[m_selected].element->activate();
					}
				}
				m_blocked = (bool)((*this)[m_selected].unblock_check);//block only if there is way to unblock
			}
		}

		void updateBlocked()
		{
			m_selected = size() > 0 ? std::min(m_selected, size() - 1) : 0;
			if (size() > 0)
			{
				if ((*this)[m_selected].unblock_check)
				{
					if ((*this)[m_selected].unblock_check())
					{
						m_blocked = false;
					}
					if ((*this)[m_selected].block_check && (*this)[m_selected].block_check())
					{
						m_blocked = true;
					}
				}
				else
				{
					m_blocked = false;
				}
			}
		}

		unsigned int getClosestIdx(SIDE side)
		{
			unsigned int cl_idx = m_selected;
			float cl_dst = FLT_MAX;

			auto center = [](surface* s)
			{
				return vec2f(s->getGlobalPosition() + s->getSize() / 2.f);
			};

			//returns distance between center of 'a' and edge of 'b'
			//if center of 'a' intersects with 'b' return value is negative
			auto dst = [&](surface* a, surface* b)
			{
				const vec2f ca = center(a);
				const vec2f cb = center(b);
				const vec2i sb = b->getSize();

				vec2f d = { 
					std::fabs(ca.x - cb.x) - sb.x/2.f,
					std::fabs(ca.y - cb.y) - sb.y/2.f
				};
				const bool  intersect = d.x < 0 && d.y < 0;

				if (!intersect)
				{
					d.x = std::max(d.x, 0.f);
					d.y = std::max(d.y, 0.f);
					return std::sqrt(d.x * d.x + d.y * d.y);
				}
				else
				{
					return std::max(d.x, d.y);
				}		
			};

			surface* sel_ptr = dynamic_cast<surface*>((*this)[m_selected].element);

			if (sel_ptr != nullptr)
			{
				for (int i = 0; i < size(); i++)
				{
					if (i != m_selected)
					{
						surface* curr_ptr = dynamic_cast<surface*>((*this)[i].element);

						if (curr_ptr != nullptr)
						{
							switch (side)
							{
							case SIDE::TOP:
								if (center(sel_ptr).y <= center(curr_ptr).y) { continue; }
								break;
							case SIDE::BOTTOM:
								if (center(sel_ptr).y >= center(curr_ptr).y) { continue; }
								break;
							case SIDE::LEFT:
								if (center(sel_ptr).x <= center(curr_ptr).x) { continue; }
								break;
							case SIDE::RIGHT:
								if (center(sel_ptr).x >= center(curr_ptr).x) { continue; }
							}

							float curr_dst = dst(sel_ptr, curr_ptr);

							if (curr_dst < cl_dst)
							{
								cl_idx = i;
								cl_dst = curr_dst;
							}
						}
					}
				}
			}

			return cl_idx;
		}

		void dirMove(SIDE side)
		{
			m_selected = getClosestIdx(side);
			enable();
		}

		void deactivationAction() override { disable(); }
		void activationAction() override { enable(); }
	public:
		input::key_t key_next = input::KEY::RIGHT;
		input::key_t key_prev = input::KEY::LEFT;
		input::key_t key_up = input::KEY::UP;
		input::key_t key_down = input::KEY::DOWN;
		input::key_t key_left = input::KEY::LEFT;
		input::key_t key_right = input::KEY::RIGHT;

		navigation_group() {}
		navigation_group(navigation_group_entry element) : std::vector<navigation_group_entry>({ element }) {}
		navigation_group(std::vector<navigation_group_entry> elements) : std::vector<navigation_group_entry>(elements) {}

		using std::vector<navigation_group_entry>::size;
		using std::vector<navigation_group_entry>::operator[];
		using std::vector<navigation_group_entry>::push_back;
		using std::vector<navigation_group_entry>::pop_back;
		using std::vector<navigation_group_entry>::insert;
		using std::vector<navigation_group_entry>::begin;
		using std::vector<navigation_group_entry>::end;
		using std::vector<navigation_group_entry>::clear;
		using std::vector<navigation_group_entry>::erase;

		void useWrappingAround(bool use) { m_wrap_around = use; }
		bool isUsingWrappingAround() { return m_wrap_around; }

		/*
		index based: navigation relies on position in vector
		non index based: navigation relies on surface position 
		*/
		void useIndexBasedNavigation(bool use) { m_index_based = use; }
		bool isUsingIndexBasedNavigation() { return m_index_based; }

		void setSelected(unsigned int selected)
		{
			m_selected = selected;
			if (isActive()) { enable(); }
		}
		unsigned int getSelected() { return m_selected; }

		void next()
		{
			m_selected = m_selected < size() - 1 ? m_selected + 1 : (m_wrap_around ? 0 : m_selected);
			enable();
		}
		void prev()
		{
			m_selected = m_selected > 0 ? m_selected - 1 : (m_wrap_around ? size() - 1 : m_selected);
			enable();
		}

		void up()    { dirMove(SIDE::TOP);    }
		void down()  { dirMove(SIDE::BOTTOM); }
		void left()  { dirMove(SIDE::LEFT);   }
		void right() { dirMove(SIDE::RIGHT);  }

		void update()
		{
			if (isActive())
			{		
				updateBlocked();
				if (!m_blocked)
				{
					switch (m_index_based)
					{
					case true:
						if (input::isKeyPressed(key_next))
						{
							next();
						}
						if (input::isKeyPressed(key_prev))
						{
							prev();
						}
						break;
					case false:
						if (input::isKeyPressed(key_up))
						{
							up();
						}
						if (input::isKeyPressed(key_down))
						{
							down();
						}
						if (input::isKeyPressed(key_left))
						{
							left();
						}
						if (input::isKeyPressed(key_right))
						{
							right();
						}
					}
				}
			}
		}
	};
}