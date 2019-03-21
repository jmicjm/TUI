#include "tui.h"


#include <iostream>

#include <thread>
#include <chrono>



int main()
{
	
		tui::console con;
		con.setTitle("tytul");
		con.setFPSlimit(10);

		tui::rectangle rect;
		rect.setSize(tui::vec2i(10, 5), tui::SIZE::CONSTANT);
		rect.setPosition(tui::position(tui::vec2i(0, 0), tui::vec2i(tui::POSITION::HORIZONTAL::CENTER, tui::POSITION::VERTICAL::CENTER)));
		rect.setChar(tui::console_char(219, tui::console_color(tui::COLOR::CYAN, tui::COLOR::BLACK)));

		tui::box mainBox(tui::vec2i(100, 100), tui::SIZE::PERCENTAGE, tui::THICKNESS::MEDIUM);

		tui::box box1(tui::vec2i(20, 100), tui::SIZE::PERCENTAGE_Y, tui::THICKNESS::THIN);
		box1.setPosition(tui::position(tui::vec2i(0, 0), tui::vec2i(tui::POSITION::HORIZONTAL::RIGHT, tui::POSITION::VERTICAL::TOP)));
		box1.setColor(tui::console_color(tui::COLOR::MAGENTA, tui::COLOR::BLACK));

		std::string  ipsum = 
		"Lorem ipsum dolor sit amet, consectetur adipiscing elit. Vestibulum feugiat egestas urna non euismod. Maecenas magna mauris, dictum non egestas eu, rhoncus sed sem. Cras egestas massa eget nulla cursus venenatis. Nulla id ultricies arcu, id sollicitudin augue. Orci varius natoque penatibus et magnis dis parturient montes, nascetur ridiculus mus.";

		tui::text text(tui::vec2i(18, 50), tui::SIZE::PERCENTAGE_Y, ipsum);
		text.setPosition(tui::position(tui::vec2i(-1, 1), tui::vec2i(tui::POSITION::HORIZONTAL::RIGHT, tui::POSITION::VERTICAL::TOP)));

		tui::text input_text(tui::vec2i(50, 30), tui::SIZE::PERCENTAGE, "xxxxx ");
		input_text.setPosition(tui::position(tui::vec2i(0, 0), tui::vec2i(tui::POSITION::HORIZONTAL::LEFT, tui::POSITION::VERTICAL::BOTTOM)));


		tui::box bx0(tui::vec2i(50, 100), tui::SIZE::PERCENTAGE, tui::THICKNESS::THIN);
		bx0.setPosition(tui::position(tui::vec2i(0, 0), tui::vec2i(tui::POSITION::HORIZONTAL::LEFT, tui::POSITION::VERTICAL::TOP)));

		tui::box bx1(tui::vec2i(50, 100), tui::SIZE::PERCENTAGE, tui::THICKNESS::THIN);
		bx1.setPosition(tui::position(tui::vec2i(0, 0), tui::vec2i(tui::POSITION::HORIZONTAL::RIGHT, tui::POSITION::VERTICAL::TOP)));


		tui::group grp(tui::vec2i(50, 50), tui::SIZE::PERCENTAGE);
		grp.setPosition(tui::position(tui::vec2i(0, 0), tui::vec2i(tui::POSITION::HORIZONTAL::CENTER, tui::POSITION::VERTICAL::CENTER)));

		grp.addSurface(bx0);
		grp.addSurface(bx1);

		std::string t_s;

		for (;;)
		{

			if (tui::KEYBOARD::isKeyPressedBuffered(tui::KEYBOARD::RIGHT)) {
				rect.move(tui::vec2i(1, 0));
			}
			if (tui::KEYBOARD::isKeyPressedBuffered(tui::KEYBOARD::LEFT)) {
				rect.move(tui::vec2i(-1, 0));
			}


			auto start = std::chrono::steady_clock::now();
			con.clear();
			con.draw(rect);

			con.draw(box1);
			con.draw(mainBox);
			

			con.draw(text);

			t_s += tui::KEYBOARD::getInputAsString(TUI_BUFFERED_INPUT);
		
			input_text.setText(t_s);
			con.draw(input_text);


			con.draw(grp);

			con.display();
			
			auto end = std::chrono::steady_clock::now();
			auto diff = end - start;
			
			//tui::KEYBOARD::clearBuffer();

			 
			std::cout << std::endl << std::chrono::duration <double, std::milli>(diff).count() << " ms" << std::endl;
			std::cout << "X: " << con.getSize().x << " Y: " << con.getSize().y << std::endl;
			std::cout << "Xb: " << mainBox.getSize().x << " Yb: " << mainBox.getSize().y << std::endl;
		//	std::cout << ""
			
			//std::cout << "caps " << tui::KEYBOARD::isCapsLockEnabled() << std::endl;
			//std::cout << "resized: " << con.wasResized() << std::endl;
			//std::cout << t_s << std::endl;
		//	std::cout << tui::KEYBOARD::keyboardBuffer.size() << std::endl;
			//std::this_thread::sleep_for(std::chrono::milliseconds(100));
		}
	
	return 0;
}