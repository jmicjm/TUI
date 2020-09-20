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
		symbol extend;
		scroll_appearance_a list_scroll_appearance_a;

		list_appearance_a() : list_appearance_a(U'\x25AA', U'\x25AB', '>') {}
		list_appearance_a(symbol checked, symbol not_checked, symbol extend) : list_appearance_a(checked, not_checked, extend, scroll_appearance_a()) {}
		list_appearance_a(symbol checked, symbol not_checked, symbol extend, scroll_appearance_a list_scroll_appearance_a)
			 : checked(checked), not_checked(not_checked), extend(extend), list_scroll_appearance_a(list_scroll_appearance_a) {}

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
	private:
		friend struct list;
		unsigned int highlighted = 0;
		unsigned int top = 0;
		bool extended = false;
		bool ext_halt = false;
		void reset()
		{
			highlighted = 0;
			top = 0;
			extended = false;
			ext_halt = false;
			for (int i = 0; i < nested_entries.size(); i++)
			{
				nested_entries[i].reset();
			}
		}
	public:
		symbol_string name;
		CHECK_STATE checked;
		std::function<void()> check_function;
		std::function<void()> uncheck_function;
		std::function<void()> select_function;

		std::vector<list_entry> nested_entries;

		list_entry
		(
			symbol_string name = "",
			CHECK_STATE checked = CHECK_STATE::NONCHECKABLE,
			std::function<void()> check_function = nullptr,
			std::function<void()> uncheck_function = nullptr,
			std::function<void()> select_function = nullptr,
			std::vector<list_entry> nested_entries = {}
		)	
			: name(name), checked(checked),
			check_function(check_function),
			uncheck_function(uncheck_function),
			select_function(select_function),
			nested_entries(nested_entries) {}
	};

	struct list : surface, active_element, list_appearance
	{
	private:
		std::vector<list_entry> m_entries;
		scroll<DIRECTION::VERTICAL> m_scroll;

		bool m_display_scroll = true;

		bool m_redraw_needed = true;

		list_appearance_a gca() const
		{
			if (isActive()) { return active_appearance; }
			else { return inactive_appearance; }
		}

		void fill(action_proxy proxy)
		{
			if (m_redraw_needed)
			{
				clear();
			}

			m_scroll.setContentLength(m_entries.size());

			for (int i = m_scroll.getTopPosition(), y = 0; y < getSize().y && i < m_entries.size(); y++, i++)
			{
				if (m_redraw_needed)
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

					symbol_string w_str = getFullWidthString(m_entries[i].name);
					for (int x = checkable * 2, j = 0; x < getSize().x && j < w_str.size(); x++, j++)
					{
						setSymbolAt(w_str[j], { x, y });
					}

					if (m_entries[i].nested_entries.size() > 0)
					{
						bool scr = m_scroll.isNeeded();
						int ext_p = getSize().x - 1 - scr;
						if (ext_p >= 0)
						{
							setSymbolAt(gca().extend, { ext_p, y });
						}
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

				if (m_entries[i].nested_entries.size() > 0 && m_entries[i].extended)
				{
					list l;
					l.setSizeInfo({ getSize() });		
					l.setPositionInfo({ getPosition() + vec2i(getSize().x, y) });
					l.setEntries(m_entries[i].nested_entries);
					
					l.key_up = key_up;
					l.key_down = key_down;
					l.key_pgup = key_pgup;
					l.key_pgdn = key_pgdn;
					l.key_check = key_check;
					l.key_right = key_right;
					l.key_left = key_left;
					l.key_select = key_select;

					l.m_scroll.setContentLength(m_entries[i].nested_entries.size());
					l.m_scroll.setSizeInfo({ getSize().y });
					l.m_scroll.setTopPosition(m_entries[i].top);
					l.m_scroll.setCurrentPosition(m_entries[i].highlighted);
					l.displayScroll(isDisplayingScroll());

					l.setClearSymbol(getClearSymbol());

					if (isActive() && m_entries[i].ext_halt == 0) { l.activate(); }

					proxy.insertSurface(l);
			
					m_entries[i].ext_halt = 0;
					m_entries[i].nested_entries = l.getEntries();
					m_entries[i].highlighted = l.m_scroll.getCurrentPosition();
					m_entries[i].top = l.m_scroll.getTopPosition();
				}

			}

			if (m_display_scroll && m_scroll.isNeeded()) { insertSurface(m_scroll, false); }
		}

		void resizeAction() override
		{
			updateSurfaceSize(m_scroll);
			m_redraw_needed = true;
		}
		void updateAction() override { update(); }
		void drawAction(action_proxy proxy) override
		{		
			fill(proxy);
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
		short key_right = input::KEY::RIGHT;
		short key_left = input::KEY::LEFT;
		short key_select = input::KEY::ENTER;

		list(surface_size size = surface_size())
		{
			m_scroll.setPositionInfo({ { 0,0 }, { 0,0 }, { POSITION::END, POSITION::BEGIN } });
			m_scroll.setSizeInfo({ 0, 100 });
			m_scroll.useFreeMode(true);

			setSizeInfo(size);
		}

		void setEntries(const std::vector<list_entry>& entries)
		{
			m_entries = entries;
			m_redraw_needed = true;
		}
		std::vector<list_entry> getEntries() const { return m_entries; }

		size_t size() const { return m_entries.size(); }

		std::vector<size_t> getHighlighted() const 
		{
			std::vector<size_t> vec;
			if (m_entries.size() > 0)
			{
				vec.push_back(m_scroll.getCurrentPosition());

				const list_entry* entry = &m_entries[vec.back()];

				while (entry->nested_entries.size() > 0 && entry->extended)
				{
					vec.push_back(entry->highlighted);
					entry = &entry->nested_entries[entry->highlighted];
				}
			}
			return vec;
		}

		void setEntryAt(const list_entry& entry, size_t i) 
		{
			m_entries[i] = entry; 
			m_redraw_needed = true;
		}
		list_entry getEntryAt(size_t i) const 
		{
			list_entry entry = m_entries[i];
			entry.reset();
			return entry; 
		}

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

		void resizeToEntries()
		{
			surface_size c_size = getSizeInfo();
			c_size.percentage.y = 0;
			c_size.fixed.y = m_entries.size();
			setSizeInfo(c_size);
		}

		void retract(bool reset_top_level = false)
		{
			if (m_entries.size() > 0)
			{
				list_entry* entry = &m_entries[m_scroll.getCurrentPosition()];

				if (reset_top_level)
				{
					m_scroll.setCurrentPosition(0);
				}

				while (entry->nested_entries.size() > 0 && entry->extended)
				{
					unsigned int highlighted = entry->highlighted;
					entry->top = 0;
					entry->highlighted = 0;
					entry->extended = false;
					entry->ext_halt = false;
					entry = &entry->nested_entries[highlighted];
				}
			}
			m_redraw_needed = true;
		}

		void displayScroll(bool display)
		{
			m_display_scroll = display;
			m_redraw_needed = true;
		}
		//is displaying scroll if needed
		bool isDisplayingScroll() const { return m_display_scroll; }

		//is displaying scroll currently
		bool isDisplayingScrollNow() const { return m_display_scroll && m_scroll.isNeeded(); }

		void update()
		{
			if (isActive())
			{
				int pos = m_scroll.getCurrentPosition();
				if (m_entries.size() > 0 && !m_entries[pos].extended)
				{
					m_scroll.update();
				}
				if (pos != m_scroll.getCurrentPosition()) { m_redraw_needed = true; }

				if (input::isKeyPressed(key_check) && m_entries.size() > 0 && !m_entries[pos].extended)
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
				if (input::isKeyPressed(key_select) && m_entries.size() > 0 && !m_entries[pos].extended)
				{
					if (m_entries[pos].select_function)
					{
						m_entries[pos].select_function();
					}
				}

				if (input::isKeyPressed(key_right) && m_entries.size() > 0 && !m_entries[pos].extended)
				{
					if (m_entries[pos].nested_entries.size() > 0)
					{
						m_entries[pos].extended = true;
						m_entries[pos].ext_halt = true;
					}
				}
				if (input::isKeyPressed(key_left) && m_entries.size() > 0 && m_entries[pos].extended)
				{
					if (m_entries[pos].nested_entries.size() > 0 && !m_entries[pos].nested_entries[m_entries[pos].highlighted].extended)
					{
						m_entries[pos].extended = false;
						m_entries[pos].ext_halt = false;
						m_entries[pos].top = 0;
						m_entries[pos].highlighted = 0;
					}
				}
			}
		}

	};
}
