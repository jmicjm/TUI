#pragma once
#include "tui_console.h"
#include "tui_appearance.h"
#include "tui_text_utils.h"

namespace tui
{
	struct input_text_appearance : appearance
	{
	protected:
		symbol m_cursor;
	public:
		input_text_appearance() : input_text_appearance('_') {}
		input_text_appearance(symbol Cursor) : m_cursor(Cursor) {}

		void setColor(color Color) override
		{
			m_cursor.setColor(Color);
			setAppearance_action();
		}

		void setCursorSymbol(symbol Cursor)
		{
			m_cursor = Cursor;
			setAppearance_action();
		}
		symbol getCursorSymbol() { return m_cursor; }
	};

	struct input_text : surface, active_element, input_text_appearance
	{
	private:
		text m_text;
		console_string m_str;
		vec2i m_cursor_pos = { 0,0 };
		int m_cursor_pos_in_txt = 0;
		time_frame m_cursor_blink;

		bool m_redraw_needed = true;

		void updateCursorPos()
		{
			vec2i old_pos = m_cursor_pos;

			if (m_text.getText().size() > 0)
			{
				auto getRelativePos = [&]()
				{
					vec2i pos = m_text.getSymbolPos(m_cursor_pos_in_txt);
					pos.y -= m_text.getLine();

					return pos;
				};

				m_cursor_pos = getRelativePos();

				if (m_cursor_pos.y >= getSize().y)
				{
					int c = m_cursor_pos.y - (getSize().y-1);

					m_text.goToLine(m_text.getLine()+c);
					m_cursor_pos = getRelativePos();
				}
				else if (m_cursor_pos.y < 0)
				{
					int c = m_cursor_pos.y * -1;

					m_text.goToLine(m_text.getLine()-c);
					m_cursor_pos = getRelativePos();
				}

			}
			else { m_cursor_pos = { 0,0 }; }

			if (m_cursor_pos != old_pos) { m_redraw_needed = true; }
		}

		void updateText()
		{
			console_string s = m_str;
			s += " ";

			if (m_cursor_pos_in_txt > s.size()) { m_cursor_pos_in_txt = s.size(); }

			m_text.setText(s);
		}
		
		void moveCursorRight(unsigned int n)
		{
			if (m_cursor_pos_in_txt + n <= m_str.size()) 
			{ 
				m_cursor_pos_in_txt += n; 
				m_redraw_needed = true;
			}
			else
			{
				m_cursor_pos_in_txt = m_str.size();
				m_redraw_needed = true;
			}
		}
		void moveCursorRight() { moveCursorRight(1); }

		void moveCursorLeft(unsigned int n)
		{
			if (m_cursor_pos_in_txt - n >= 0) 
			{ 
				m_cursor_pos_in_txt -= n; 
				m_redraw_needed = true;
			}
			else
			{
				m_cursor_pos_in_txt = 0;
				m_redraw_needed = true;
			}
		}
		void moveCursorLeft() { moveCursorLeft(1); }

		void moveCursorUp(unsigned int n)
		{
			if (m_str.size() > 0)
			{
				vec2i c_pos = m_text.getSymbolPos(m_cursor_pos_in_txt);

				if (c_pos.y > 0)
				{
					if (c_pos.y < n)
					{
						n = c_pos.y;
					}

					for (int l_pos = m_cursor_pos_in_txt - 1; l_pos >= 0; --l_pos)
					{
						if (m_text.getSymbolPos(l_pos).y == c_pos.y - n && m_text.getSymbolPos(l_pos).x <= c_pos.x)
						{
							m_cursor_pos_in_txt = l_pos;
							m_redraw_needed = true;
							break;
						}
					}
				}
			}
		}
		void moveCursorUp() { moveCursorUp(1); }

		void moveCursorDown(unsigned int n)
		{
			if (m_str.size() > 0)
			{
				vec2i c_pos = m_text.getSymbolPos(m_cursor_pos_in_txt);

				if (c_pos.y < m_text.getNumberOfLines()-1)
				{
					if (c_pos.y >= m_text.getNumberOfLines() - n)
					{
						n = m_text.getNumberOfLines() - c_pos.y - 1;
					}

					for (int l_pos = m_cursor_pos_in_txt + 1; l_pos <= m_str.size(); ++l_pos)
					{
						if (m_text.getSymbolPos(l_pos).y == c_pos.y + n && (m_text.getSymbolPos(l_pos).x == c_pos.x || l_pos == m_str.size()))
						{
							m_cursor_pos_in_txt = l_pos;
							m_redraw_needed = true;
							break;
						}
						else if (m_text.getSymbolPos(l_pos).y > c_pos.y + n)
						{
							m_cursor_pos_in_txt = l_pos - 1;
							m_redraw_needed = true;
							break;
						}
					}
				}
			}
		}
		void moveCursorDown() { moveCursorDown(1); }

		void fill()
		{
			updateCursorPos();

			if (m_redraw_needed)
			{
				makeTransparent();
				insertSurface(m_text);
			}

			static bool blink = true;

			if (m_cursor_blink.isEnd(true)) { blink = !blink; }
		
			if (isActive() && blink)
			{
				setSymbolAt(m_cursor, m_cursor_pos);
			}
		}
	public:
		int keyUp = KEYBOARD::KEY::UP;
		int keyDown = KEYBOARD::KEY::DOWN;
		int keyLeft = KEYBOARD::KEY::LEFT;
		int keyRight = KEYBOARD::KEY::RIGHT;

		input_text() : m_cursor_blink(std::chrono::milliseconds(500))
		{
			m_text.setSize({ {0,0},{100,100} });
			m_text.useDensePunctuation(false);
			m_text.usePreparedText(false);

		}

		void update()
		{
			if (isActive())
			{
				std::string input = KEYBOARD::getInputAsString();

				if (input.size() > 0)
				{
					for (int i = 0; i < input.size(); i++)
					{
						m_str.insert(m_str.begin() + m_cursor_pos_in_txt + i, input[i]);
					}
					moveCursorRight(input.size());
					updateText();
				}


				if (KEYBOARD::isKeyPressed(KEYBOARD::KEY::BACKSPACE))
				{
					int erase_c = KEYBOARD::isKeyPressed(KEYBOARD::KEY::BACKSPACE);
					if (erase_c > m_cursor_pos_in_txt) { erase_c = m_cursor_pos_in_txt; }

					m_str.erase(m_str.begin() + m_cursor_pos_in_txt - erase_c, m_str.begin() + m_cursor_pos_in_txt);

					moveCursorLeft(erase_c);
					updateText();
				}

				if (KEYBOARD::isKeyPressed(keyLeft)) { moveCursorLeft(KEYBOARD::isKeyPressed(keyLeft)); }
				if (KEYBOARD::isKeyPressed(keyRight)) { moveCursorRight(KEYBOARD::isKeyPressed(keyRight)); }
				if (KEYBOARD::isKeyPressed(keyUp)) { moveCursorUp(KEYBOARD::isKeyPressed(keyUp)); }
				if (KEYBOARD::isKeyPressed(keyDown)) { moveCursorDown(KEYBOARD::isKeyPressed(keyDown)); }
			}
		}

		console_string getText() { return m_str; }

		void resize_action() override { m_redraw_needed = true; }
		void draw_action() override 
		{ 
			update();
			fill();
		}

		void activation_action() override 
		{
			m_text.activate();
			m_redraw_needed = true;
		}
		void disactivation_action() override 
		{
			m_text.disactivate();
			m_redraw_needed = true;
		}

		void setAppearance_action() override { m_redraw_needed = true; }

	};

}
