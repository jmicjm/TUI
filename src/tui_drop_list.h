#pragma once
#include "tui_surface.h"
#include "tui_active_element.h"
#include "tui_appearance.h"
#include "tui_active_element.h"
#include "tui_input.h"
#include "tui_scroll.h"
#include "tui_list.h"

namespace tui
{
	struct drop_list_appearance_a
	{
		list_appearance_a dlist_list_appearance_a;

		drop_list_appearance_a(list_appearance_a dlist_list_appearance_a = list_appearance_a()) :
			dlist_list_appearance_a(dlist_list_appearance_a) {}

		void setColor(color Color)
		{
			dlist_list_appearance_a.setColor(Color);
		}
	};

	struct drop_list_appearance : appearance
	{
	protected:
		drop_list_appearance_a active_appearance;
		drop_list_appearance_a inactive_appearance;
	public:
		drop_list_appearance()
		{
			inactive_appearance.setColor(tui::COLOR::DARKGRAY);
		}
		drop_list_appearance(drop_list_appearance_a active, drop_list_appearance_a inactive) 
			: active_appearance(active), inactive_appearance(inactive) {}

		void setColor(color Color)
		{
			active_appearance.setColor(Color);
			inactive_appearance.setColor(Color);
			setAppearanceAction();
		}

		void setAppearance(drop_list_appearance appearance) { setElement(*this, appearance); }
		drop_list_appearance getAppearance() const { return *this; }

		void setActiveAppearance(drop_list_appearance_a active) { setElement(active_appearance, active); }
		drop_list_appearance_a getActiveAppearance() const { return active_appearance; }

		void setInactiveAppearance(drop_list_appearance_a inactive) { setElement(inactive_appearance, inactive); }
		drop_list_appearance_a getInactiveAppearance() const { return inactive_appearance; }
	};


	struct drop_list : surface1D<tui::DIRECTION::HORIZONTAL>, active_element, drop_list_appearance
	{
	private:
		symbol_string m_title;
		list m_list;
		bool m_dropped = false;
		bool m_drop_halt = false;

		bool m_redraw_needed = true;

		void fill(surface::action_proxy proxy)
		{
			if (m_redraw_needed)
			{
				makeBlank();
				for (int i = 0; i < m_title.size() && i < getSize(); i++)
				{
					setSymbolAt(m_title[i], i);
				}
				if (isActive()) { invert(); }
			}

			if (m_dropped)
			{
				proxy.insertSurface(m_list, !m_drop_halt);
				m_drop_halt = false;
			}
		}

		void resizeAction() override { m_redraw_needed = true; }
		void updateAction() override { update(); }
		void drawAction(action_proxy proxy) override
		{
			fill(proxy);
			m_redraw_needed = false;
		}

		void activationAction() override
		{
			m_list.activate();
			m_redraw_needed = true;
		}
		void disactivationAction() override
		{
			m_list.disactivate();
			m_redraw_needed = true;
		}

		void setAppearanceAction() override
		{
			m_list.setAppearance({ active_appearance.dlist_list_appearance_a, inactive_appearance.dlist_list_appearance_a });
			m_redraw_needed = true;
		}
	public:
		short& key_up = m_list.key_up;
		short& key_down = m_list.key_down;
		short& key_pgup = m_list.key_pgup;
		short& key_pgdn = m_list.key_pgdn;
		short& key_check = m_list.key_check;
		short& key_right = m_list.key_right;
		short& key_left = m_list.key_left;
		short& key_select = m_list.key_select;

		drop_list(surface1D_size size = surface1D_size())
		{
			m_list.setAnchor(this);
			m_list.setAnchorPositionInfo({ tui::SIDE::BOTTOM, tui::POSITION::BEGIN });
			m_list.setClearSymbol(' ');

			setSizeInfo(size);
		}

		void setTitle(const symbol_string& title)
		{
			m_title = title;
			m_redraw_needed = true;
		}
		symbol_string getTitle() const { return m_title; }

		void setListSizeInfo(surface_size size)
		{
			m_list.setSizeInfo(size);
		}
		surface_size getListSizeInfo() const { return m_list.getSizeInfo(); }

		void setEntries(const std::vector<list_entry>& entries)
		{
			m_list.setEntries(entries);
			m_redraw_needed = true;
		}
		std::vector<list_entry> getEntries() const { return m_list.getEntries(); }

		size_t size() const { return m_list.size(); }

		std::vector<size_t> getHighlighted() const
		{
			switch (m_dropped)
			{
			case true:
				return m_list.getHighlighted();
			case false:
				return std::vector<size_t>();
			}
		}

		void setEntryAt(const list_entry& entry, size_t i)
		{
			m_list.setEntryAt(entry, i);
			m_redraw_needed = true;
		}
		list_entry getEntryAt(size_t i) const
		{
			return m_list.getEntryAt(i);
		}

		void removeEntryAt(size_t i)
		{
			m_list.removeEntryAt(i);
			m_redraw_needed = true;
		}
		void insertEntryAt(const list_entry& entry, size_t i)
		{
			m_list.insertEntryAt(entry, i);
			m_redraw_needed = true;
		}
		void addEntry(const list_entry& entry)
		{
			m_list.addEntry(entry);
			m_redraw_needed = true;
		}

		void retract()
		{
			m_list.retract(true);
			m_dropped = false;
		}

		void update()
		{
			if (isActive())
			{
				if (input::isKeyPressed(key_down))
				{
					if (!m_dropped && m_list.size() > 0)
					{
						m_dropped = true;
						m_drop_halt = true;
					}
				}
				if (input::isKeyPressed(key_up))
				{
					if (m_dropped && m_list.size() > 0)
					{
						if (m_list.getHighlighted()[0] == 0 && m_list.getHighlighted().size() == 1)
						{
							m_dropped = false;
							m_drop_halt = false;
						}
					}
				}
			}
		}
	};
}
