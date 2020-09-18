#pragma once
#include "tui_surface.h"
#include "tui_appearance.h"
#include "tui_active_element.h"
#include "tui_input.h"
#include "tui_time_frame.h"

#include <vector>
#include <algorithm>

namespace tui
{
	struct line_input_appearance_a
	{
		symbol insert_cursor;
		symbol overtype_cursor;
		color text_color;

		line_input_appearance_a() : line_input_appearance_a('_', U'\x2584') {}
		line_input_appearance_a(symbol insert_cursor, symbol overtype_cursor, color text_color = color())
			: insert_cursor(insert_cursor), overtype_cursor(overtype_cursor), text_color(text_color) {}

		void setColor(color Color)
		{
			insert_cursor.setColor(Color);
			overtype_cursor.setColor(Color);
			text_color = Color;
		}
	};

	struct line_input_appearance : appearance
	{
	protected:
		line_input_appearance_a active_appearance;
		line_input_appearance_a inactive_appearance;
	public:
		line_input_appearance(){}
		line_input_appearance(line_input_appearance_a active, line_input_appearance_a inactive) : active_appearance(active), inactive_appearance(inactive) {}

		void setColor(color Color) override
		{
			active_appearance.setColor(Color);
			inactive_appearance.setColor(Color);
			setAppearanceAction();
		}

		void setAppearance(line_input_appearance appearance) { setElement(*this, appearance); }
		line_input_appearance getAppearance() const { return *this; }

		void setActiveAppearance(line_input_appearance_a active) { setElement(active_appearance, active); }
		line_input_appearance_a getActiveAppearance() const { return active_appearance; }

		void setInactiveAppearance(line_input_appearance_a inactive) { setElement(inactive_appearance, inactive); }
		line_input_appearance_a getInactiveAppearance() const { return inactive_appearance; }
	};

	struct line_input : surface1D<tui::DIRECTION::HORIZONTAL>, active_element, line_input_appearance
	{
	private:
		symbol_string m_str = " ";
		std::vector<unsigned int> m_symbol_pos = {0};
		unsigned int m_cursor_sym_idx = 0;
		unsigned int m_first_sym_idx = 0;
		time_frame m_cursor_blink;

		bool m_redraw_needed = true;
		bool m_insert_mode = true;
		bool m_confidential_mode = false;
		bool m_blink = true;

		line_input_appearance_a gca() const
		{
			if (isActive()) { return active_appearance; }
			else { return inactive_appearance; }
		}

		void updateSymbolPos()
		{
			m_symbol_pos.resize(m_str.size());

			int pos = 0;
			for (int i = 0; i < m_str.size(); i++)
			{
				m_symbol_pos[i] = pos;
				pos += m_str[i].getWidth();
				
			}
		}

		void updateFirstSymIdx()
		{
			if (m_symbol_pos.back() < getSize())
			{
				m_first_sym_idx = 0;
			}
			if (m_first_sym_idx > m_cursor_sym_idx)
			{
				m_first_sym_idx = m_cursor_sym_idx;
			}
			if (m_symbol_pos.back() >= getSize() && m_symbol_pos.back() < m_symbol_pos[m_first_sym_idx] + getSize())
			{
				auto f_it = std::lower_bound(
					m_symbol_pos.begin(),
					m_symbol_pos.end(),
					m_symbol_pos.back() - getSize() + 1
				);

				m_first_sym_idx = f_it - m_symbol_pos.begin();
			}
			if (m_symbol_pos[m_cursor_sym_idx] >= m_symbol_pos[m_first_sym_idx] + getSize())
			{
				auto f_it = std::lower_bound(
					m_symbol_pos.begin(),
					m_symbol_pos.end(),
					m_symbol_pos[m_cursor_sym_idx] - getSize() + 1
				);

				m_first_sym_idx = f_it - m_symbol_pos.begin();
			}
		}

		void fill()
		{
			clear();
			updateFirstSymIdx();

			const unsigned int f_pos = m_symbol_pos[m_first_sym_idx];

			for (int i = m_first_sym_idx; i < m_str.size() && m_symbol_pos[i] - f_pos < getSize(); i++)
			{
				if (m_str[i].getWidth() > 0)
				{
					if (!m_confidential_mode)
					{
						setSymbolAt(m_str[i], m_symbol_pos[i] - f_pos);
					}
					else
					{
						setSymbolAt('*', m_symbol_pos[i] - f_pos);
					}
					(*this)[m_symbol_pos[i] - f_pos].setColor(gca().text_color);
				}
			}

			if (isActive() && (m_cursor_blink.isEnd(false) || m_redraw_needed))
			{
				unsigned int c_pos = m_symbol_pos[m_cursor_sym_idx] - f_pos;

				if (c_pos < getSize())
				{
					if (m_blink)
					{
						if (m_insert_mode) { setSymbolAt(gca().insert_cursor, c_pos); }
						else { setSymbolAt(gca().overtype_cursor, c_pos); }
					}
					else
					{
						symbol sym = !m_confidential_mode ? m_str[m_cursor_sym_idx] : '*';
						sym.setColor(gca().text_color);
						setSymbolAt(sym, c_pos);
					}
				}

				if (m_cursor_blink.isEnd(true)) { m_blink = !m_blink; }
			}
		}

		void moveCursorRight(unsigned int n = 1)
		{
			m_cursor_sym_idx = std::min(m_cursor_sym_idx + n, (unsigned int)m_str.size() - 1);
			m_redraw_needed = true;
		}
		void moveCursorLeft(unsigned int n = 1)
		{
			m_cursor_sym_idx = std::max((long long)m_cursor_sym_idx - (long long)n, (long long)0);
			m_redraw_needed = true;
		}

		void resizeAction() override { m_redraw_needed = true; }
		void updateAction() override { update(); }
		void drawAction(surface::action_proxy proxy) override 
		{
			fill(); 
			m_redraw_needed = false;
		}

		void activationAction() override { m_redraw_needed = true; }
		void disactivationAction() override { m_redraw_needed = true; }

		void setAppearanceAction() override { m_redraw_needed = true; }

	public:
		short key_left = input::KEY::LEFT;
		short key_right = input::KEY::RIGHT;
		short key_insert = input::KEY::INS;
		short key_backspace = input::KEY::BACKSPACE;


		line_input(surface1D_size size = surface1D_size()) : m_cursor_blink(std::chrono::milliseconds(500))
		{
			setSizeInfo(size);
		}

		symbol_string getText()
		{
			symbol_string str = m_str;
			str.pop_back();
			return str;
		}

		void useConfidentialMode(bool use)
		{
			m_confidential_mode = use;
			m_redraw_needed = true;
		}
		bool isUsingConfidentialMode() const { return m_confidential_mode; }

		void update()
		{
			if (isActive())
			{
				std::vector<short> input = input::getInput();
				std::string buffer;

				auto addText = [&](symbol_string str)
				{
					if (m_insert_mode)
					{
						for (int i = 0; i < str.size(); i++)
						{
							m_str.insert(m_str.begin() + m_cursor_sym_idx + i, str[i]);
						}
					}
					else//overtype
					{
						for (int i = 0; i < str.size(); i++)
						{
							if (i + m_cursor_sym_idx < m_str.size()-1)
							{
								m_str[i + m_cursor_sym_idx] = str[i];
							}
							else
							{
								m_str.insert(m_str.end()-1,str[i]);
							}
						}
					}
					moveCursorRight(str.size());
				};
				auto isSpecialKey = [&](short key)
				{
					return key == key_backspace
						|| key == key_insert
						|| key == key_left
						|| key == key_right;
				};

				bool update_needed = false;
				auto update = [&]()
				{
					if (buffer.size() > 0 || update_needed) { updateSymbolPos(); }
					update_needed = false;
				};

				for (int i = 0; i < input.size(); i++)
				{
					if (isSpecialKey(input[i]))
					{
						addText((symbol_string)buffer);

						if (input[i] == key_backspace)
						{
							if (m_cursor_sym_idx > 0)
							{
								m_str.erase(m_str.begin() + m_cursor_sym_idx - 1);
								moveCursorLeft();

								update_needed = true;
							}
						}
						else if (input[i] == key_left)
						{
							moveCursorLeft();
						}
						else if (input[i] == key_right)
						{
							moveCursorRight();
						}
						else if (input[i] == key_insert)
						{
							m_insert_mode = !m_insert_mode;
						}

						buffer.clear();
					}
					else if (input[i] >= 32 && input[i] <= 255)
					{
						buffer.push_back(input[i]);
					}
				}

				addText((symbol_string)buffer);
				update();

			}
		}
	};
}