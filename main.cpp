#include "tui.h"

#include <iostream>

#include <thread>
#include <chrono>

int main()
{

		tui::console con;
		con.setTitle("tytul");

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


		tui::scroll<tui::SCROLL::DIRECTION::HORIZONTAL> scroll(tui::vec2i(50, 1), tui::SIZE::PERCENTAGE_X);
		scroll.setPosition(tui::position(tui::vec2i(0, 0), tui::vec2i(tui::POSITION::HORIZONTAL::CENTER, tui::POSITION::VERTICAL::CENTER)));
		scroll.setLenght(20);
		scroll.setHandlePosition(5);

		for (;;)
		{

			if (GetKeyState(VK_RIGHT) & 0x8000) {
				//rect.move(tui::vec2i(1, 0));
				scroll.setHandlePosition(scroll.getHandlePosition() + 1);
			}
			if (GetKeyState(VK_LEFT) & 0x8000) {
				//rect.move(tui::vec2i(-1, 0));
				scroll.setHandlePosition(scroll.getHandlePosition() - 1);
			}
			if (GetKeyState(VK_UP) & 0x8000) {
				//rect.move(tui::vec2i(0, -1));
				//text.setText(text.getText() + "O");
				scroll.setLenght(scroll.getLenght() + 1);

			}
			if (GetKeyState(VK_DOWN) & 0x8000) {
				//rect.move(tui::vec2i(0, 1));
				//text.setText("X");
				scroll.setLenght(scroll.getLenght() - 1);
			}

			auto start = std::chrono::steady_clock::now();
			con.clear();
			con.draw(rect);

			con.draw(box1);
			con.draw(mainBox);
			con.draw(text);


		//	scroll.update();
			con.draw(scroll);
			con.display();
			

			

			auto end = std::chrono::steady_clock::now();
			auto diff = end - start;
			std::cout << std::endl << std::chrono::duration <double, std::milli>(diff).count() << " ms" << std::endl;
			
			std::cout << "caps " << tui::isCapsLockEnabled() << std::endl;
			std::cout << "resized: " << con.wasResized() << std::endl;
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
		}
	
	return 0;
}