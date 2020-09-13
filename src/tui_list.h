#pragma once
#include "tui_surface.h"
#include "tui_active_element.h"
#include "tui_appearance.h"
#include "tui_active_element.h"
#include "tui_input.h"
#include "tui_scroll.h"

#include <vector>
#include <functional>

namespace tui
{
	struct list_appearance_a
	{
		symbol checked;
		symbol not_checked;
		scroll_appearance_a list_scroll_appearance_a;

		list_appearance_a() : list_appearance_a(U'\x25AA', U'\x25AB') {}
		list_appearance_a(symbol checked, symbol not_checked) : list_appearance_a(checked, not_checked, scroll_appearance_a()) {}
		list_appearance_a(symbol checked, symbol not_checked, scroll_appearance_a list_scroll_appearance_a)
			 : checked(checked), not_checked(not_checked),  list_scroll_appearance_a(list_scroll_appearance_a) {}

		void setColor(color Color)
		{
			checked.setColor(Color);
			not_checked.setColor(Color);
			list_scroll_appearance_a.setColor(Color);
		}
	};

	struct list_appearance : appearance
	{
	protected:
		list_appearance_a active_appearance;
		list_appearance_a inactive_appearance;
	public:
		list_appearance()
		{
			inactive_appearance.setColor(tui::COLOR::DARKGRAY);
		}
		list_appearance(list_appearance_a active, list_appearance_a inactive) : active_appearance(active), inactive_appearance(inactive) {}

		void setColor(color Color) override
		{
			active_appearance.setColor(Color);
			inactive_appearance.setColor(Color);
			setAppearanceAction();
		}

		void setAppearance(list_appearance appearance) { setElement(*this, appearance); }
		list_appearance getAppearance() const { return *this; }

		void setActiveAppearance(list_appearance_a active) { setElement(active_appearance, active); }
		list_appearance_a getActiveAppearance() const { return active_appearance; }

		void setInactiveAppearance(list_appearance_a inactive) { setElement(inactive_appearance, inactive); }
		list_appearance_a getInactiveAppearance() const { return inactive_appearance; }
	};

	enum class CHECK_STATE : uint8_t
	{
		NOT_CHECKED = 0, CHECKED = 1, NONCHECKABLE = 2
	};

	struct list_entry
	{
		symbol_string name;
		CHECK_STATE checked;
		std::function<void()> check_function;
		std::function<void()> uncheck_function;

		list_entry(symbol_string name = "", CHECK_STATE checked = CHECK_STATE::NONCHECKABLE, std::function<void()> check_function = nullptr, std::function<void()> uncheck_function = nullptr)
			: name(name), checked(checked), check_function(check_function), uncheck_function(uncheck_function) {}
	};

	struct list : surface, active_element, list_appearance
	{
	private:
		std::vector<list_entry> m_entries;
		scroll<DIRECTION::VERTICAL> m_scroll;

		bool m_redraw_needed = true;

		list_appearance_a gca()
		{
			if (isActive()) { return active_appearance; }
			else { return inactive_appearance; }
		}

		void fill()
		{
			clear();

			m_scroll.setContentLength(m_entries.size());

			for (int i = m_scroll.getTopPosition(), y = 0; y < getSize().y && i < m_entries.size(); y++, i++)
			{
				switch (m_entries[i].checked)
				{
				case CHECK_STATE::NOT_CHECKED:
					setSymbolAt(gca().not_checked, { 0, y });
					break;
				case CHECK_STATE::CHECKED:
					setSymbolAt(gca().checked, { 0, y });
				}

				bool checkable = m_entries[i].checked != CHECK_STATE::NONCHECKABLE;

				if (checkable && getSize().x > 1) { setSymbolAt(' ', { 1, y }); }

				for (int x = checkable*2, j = 0; x < getSize().x && j < m_entries[i].name.size(); x++, j++)
				{
					setSymbolAt(m_entries[i].name[j], { x, y });
				}

				if (i == m_scroll.getCurrentPosition())
				{
					for (int x = 0; x < getSize().x; x++)
					{
						if ((*this)[x][y].getWidth() == 0)
						{
							(*this)[x][y] = ' ';
						}
						(*this)[x][y].invert();
					}
				}
			}

			insertSurface(m_scroll, false);
		}

		void resizeAction() override
		{
			updateSurfaceSize(m_scroll);
			m_redraw_needed = true;
		}
		void updateAction() override { update(); }
		void drawAction() override
		{
			if (m_redraw_needed) { fill(); }
			m_redraw_needed = false;
		}

		void activationAction() override
		{
			m_scroll.activate();
			m_redraw_needed = true;
		}
		void disactivationAction() override
		{
			m_scroll.disactivate();
			m_redraw_needed = true;
		}

		void setAppearanceAction() override
		{
			m_scroll.setAppearance({ active_appearance.list_scroll_appearance_a, inactive_appearance.list_scroll_appearance_a });
			m_redraw_needed = true;
		}

	public:
		short& key_up = m_scroll.key_up;
		short& key_down = m_scroll.key_down;
		short& key_pgup = m_scroll.key_pgup;
		short& key_pgdn = m_scroll.key_pgdn;
		short key_check = ' ';

		list()
		{
			m_scroll.setPositionInfo({ { 0,0 }, { 0,0 }, { POSITION::END, POSITION::BEGIN } });
			m_scroll.setSizeInfo({ 0, 100 });

			m_scroll.useFreeMode(true);
		}

		void setEntries(const std::vector<list_entry>& entries)
		{
			m_entries = entries;
			m_redraw_needed = true;
		}
		std::vector<list_entry> getEntries() { return m_entries; }

		size_t size() { return m_entries.size(); }

		size_t getHighlighted() { return m_scroll.getCurrentPosition(); }

		void setEntryAt(const list_entry& entry, size_t i) 
		{
			m_entries[i] = entry; 
			m_redraw_needed = true;
		}
		list_entry getEntryAt(size_t i) { return m_entries[i]; }

		void removeEntryAt(size_t i)
		{
			m_entries.erase(m_entries.begin() + i);
			m_redraw_needed = true;
		}
		void insertEntryAt(const list_entry& entry, size_t i)
		{
			m_entries.insert(m_entries.begin() + i, entry);
			m_redraw_needed = true;
		}
		void addEntry(const list_entry& entry)
		{
			m_entries.push_back(entry);
			m_redraw_needed = true;
		}

		void update()
		{
			if (isActive())
			{
				int pos = m_scroll.getCurrentPosition();
				m_scroll.update();
				if (pos != m_scroll.getCurrentPosition()) { m_redraw_needed = true; }

				if (input::isKeyPressed(key_check) && m_entries.size() > 0)
				{
					if (m_entries[pos].checked != CHECK_STATE::NONCHECKABLE)
					{
						switch (m_entries[pos].checked)
						{
						case CHECK_STATE::CHECKED:
							m_entries[pos].checked = CHECK_STATE::NOT_CHECKED;
							if (m_entries[pos].uncheck_function) { m_entries[pos].uncheck_function(); }
							break;
						case CHECK_STATE::NOT_CHECKED:
							m_entries[pos].checked = CHECK_STATE::CHECKED;
							if (m_entries[pos].check_function) { m_entries[pos].check_function(); }
						}
						m_redraw_needed = true;
					}
				}
			}
		}

	};
}
