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

		bool redraw_needed = true;

		void updateCursorPos()
		{
			if (m_text.getText().size() > 0)
			{
				auto setPos = [&]()
				{
					m_cursor_pos = m_text.getSymbolPos(m_cursor_pos_in_txt);

					m_cursor_pos.y -= m_text.getLine();
				};

				setPos();

				if (m_cursor_pos.y >= getSize().y)
				{
					int c = m_cursor_pos.y - (getSize().y-1);

					m_text.goToLine(m_text.getLine()+c);
					setPos();
				}
				else if (m_cursor_pos.y < 0)
				{
					int c = m_cursor_pos.y * -1;

					m_text.goToLine(m_text.getLine()-c);
					setPos();
				}

			}
			else { m_cursor_pos = { 0,0 }; }
		}

		void updateText()
		{
			console_string s = m_str;
			s += " ";

			m_text.setText(s);
		}
		
		void moveCursorRight(bool set_str)
		{
			if (set_str) { updateText(); }

			if (m_cursor_pos_in_txt < m_str.size()) { m_cursor_pos_in_txt++; }

			updateCursorPos();

		}
		void moveCursorLeft(bool set_str)
		{
			if (set_str) { updateText(); }

			if (m_cursor_pos_in_txt > 0) { m_cursor_pos_in_txt--; }

			updateCursorPos();
		}

		void moveCursorUp()
		{
			if (m_str.size() > 0)
			{
				vec2i c_pos = m_text.getSymbolPos(m_cursor_pos_in_txt);

				for (int l_pos = m_cursor_pos_in_txt; l_pos >= 0; --l_pos)
				{
					if (m_text.getSymbolPos(l_pos).y == c_pos.y - 1 && m_text.getSymbolPos(l_pos).x <= c_pos.x)
					{
						m_cursor_pos_in_txt = l_pos;
						break;
					}
				}

				updateCursorPos();
			}
		}

		void moveCursorDown()
		{
			if (m_str.size() > 0)
			{
				vec2i c_pos = m_text.getSymbolPos(m_cursor_pos_in_txt);

				for (int l_pos = m_cursor_pos_in_txt; l_pos <= m_str.size(); ++l_pos)
				{
					if (m_text.getSymbolPos(l_pos).y == c_pos.y + 1 && (m_text.getSymbolPos(l_pos).x == c_pos.x || l_pos == m_str.size()))
					{
						m_cursor_pos_in_txt = l_pos;
						break;
					}
					else if (m_text.getSymbolPos(l_pos).y > c_pos.y + 1) 
					{
						m_cursor_pos_in_txt = l_pos - 1;
						break; 
					}
				}

				updateCursorPos();
			}
		}

		int getTextWidth()
		{
			int w = m_text.getSize().x;
			if (m_text.isDisplayingScroll()) { w--; }
			return w;
		}

		void fill()
		{
			if (redraw_needed)
			{
				makeTransparent();

				insertSurface(m_text);
			}

			static bool blink = true;

			if (m_cursor_blink.isEnd(true)) { blink = !blink; }
		
			if (blink)
			{
				updateCursorPos();
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
			bool update_text = false;

			std::string input = KEYBOARD::getInputAsString();

			
			for (int i = 0; i < input.size(); i++)
			{
				m_str.insert(m_str.begin() + m_cursor_pos_in_txt, input[i]);
				moveCursorRight(true);

				redraw_needed = true;
				update_text = true;
			}
			
			for(int i = 0;(i< KEYBOARD::isKeyPressed(KEYBOARD::KEY::BACKSPACE) && m_cursor_pos_in_txt >0);i++)
			{ 
				m_str.erase(m_str.begin() + m_cursor_pos_in_txt - 1);
				moveCursorLeft(true);

				redraw_needed = true;
				update_text = true;
			}

			if (KEYBOARD::isKeyPressed(keyLeft)) 
			{
				redraw_needed = true;
				moveCursorLeft(false); 
			}
			if (KEYBOARD::isKeyPressed(keyRight)) 
			{
				redraw_needed = true;
				moveCursorRight(false); 
			}
			if (KEYBOARD::isKeyPressed(keyUp))
			{
				redraw_needed = true;
				moveCursorUp();
			}
			if (KEYBOARD::isKeyPressed(keyDown))
			{
				redraw_needed = true;
				moveCursorDown();
			}

			if (update_text) { updateText(); }
		
			fill(); 
		}

		console_string getText() { return m_text.getText(); }

		void resize_action() override { redraw_needed = true; }
		void draw_action() override { update(); }

		void activation_action() override {}
		void disactivation_action() override {}

		void setAppearance_action() override { fill(); }

	};

}
