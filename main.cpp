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
		rect.setPosition(tui::position(tui::vec2i(0, 0), { 0,0 }, tui::vec2i(tui::POSITION::HORIZONTAL::CENTER, tui::POSITION::VERTICAL::CENTER)));
		rect.setChar(tui::console_char(219, tui::console_color(tui::COLOR::CYAN, tui::COLOR::BLACK)));

		tui::box mainBox({ 100, 100 }, tui::SIZE::PERCENTAGE, tui::THICKNESS::MEDIUM);

		tui::box box1(tui::vec2i(20, 100), tui::SIZE::PERCENTAGE_Y, tui::THICKNESS::THIN);
		box1.setPosition(tui::position(tui::vec2i(0, 0), { 0,0 }, tui::vec2i(tui::POSITION::HORIZONTAL::RIGHT, tui::POSITION::VERTICAL::TOP)));
		box1.setColor({ tui::COLOR::MAGENTA, tui::COLOR::BLACK });

		//std::string  ipsum = 
		//"Lorem ipsum dolor sit amet, consectetur adipiscing elit. Vestibulum feugiat egestas urna non euismod. Maecenas magna mauris, dictum non egestas eu, rhoncus sed sem. Cras egestas massa eget nulla cursus venenatis. Nulla id ultricies arcu, id sollicitudin augue. Orci varius natoque penatibus et magnis dis parturient montes, nascetur ridiculus mus.";

		tui::console_string ipsum;
		ipsum << tui::COLOR::CYAN;
		ipsum << "Lorem ipsum dolor sit amet, consectetur adipiscing elit. ";
		ipsum << tui::COLOR::MAGENTA;
		ipsum << "Vestibulum feugiat egestas urna non euismod. "; 
		ipsum << tui::console_color(tui::COLOR::BLACK, tui::COLOR::YELLOW);
		ipsum << "Maecenas "; 
		ipsum << tui::COLOR::MAGENTA;
		ipsum << "magna mauris, dictum non egestas eu, rhoncus sed sem. Cras egestas massa eget nulla cursus venenatis. Nulla id ultricies arcu, id sollicitudin augue. Orci varius natoque penatibus et magnis dis parturient montes, nascetur ridiculus mus.";

		tui::text text(tui::vec2i(18, 50), tui::SIZE::PERCENTAGE_Y, ipsum);
		text.setPosition(tui::position(tui::vec2i(-1, 1), { 0,0 }, tui::vec2i(tui::POSITION::HORIZONTAL::RIGHT, tui::POSITION::VERTICAL::TOP)));

		text.disactivate();

		tui::text input_text(tui::vec2i(50, 30), tui::SIZE::PERCENTAGE, "xxxxx ");
		input_text.setPosition(tui::position(tui::vec2i(0, 0), { 0,0 }, tui::vec2i(tui::POSITION::HORIZONTAL::LEFT, tui::POSITION::VERTICAL::BOTTOM)));


		tui::box bx0(tui::vec2i(25, 100), tui::SIZE::PERCENTAGE, tui::THICKNESS::THIN);
		bx0.setPosition(tui::position(tui::vec2i(0, 0), { 0,0 }));

		tui::box bx1(tui::vec2i(25, 100), tui::SIZE::PERCENTAGE, tui::THICKNESS::THIN);
		bx1.setPosition(tui::position(tui::vec2i(0, 0), { 25,0 }));

		tui::box bx2(tui::vec2i(25, 100), tui::SIZE::PERCENTAGE, tui::THICKNESS::THIN);
		bx2.setPosition(tui::position(tui::vec2i(0, 0), { 50,0 }));

		tui::box bx3(tui::vec2i(25, 100), tui::SIZE::PERCENTAGE, tui::THICKNESS::THIN);
		bx3.setPosition(tui::position(tui::vec2i(0, 0), { 75,0 }));


		tui::group grp(tui::vec2i(50, 50), tui::SIZE::PERCENTAGE);
		grp.setPosition(tui::position(tui::vec2i(0, 0), { 0,0 }, tui::vec2i(tui::POSITION::HORIZONTAL::CENTER, tui::POSITION::VERTICAL::CENTER)));

		grp.addSurface(bx0);
		grp.addSurface(bx1);
		grp.addSurface(bx2);
		grp.addSurface(bx3);

		//grp.removeSurface(bx1);

		bx0.setColor(tui::console_color(tui::COLOR::BLUE, tui::COLOR::DARKGRAY));

		std::string t_s;

		tui::console_string con_str("test");

		//con_str << tui::COLOR::BLUE;
		con_str = "test";

		tui::rectangle r0;
		r0.setSize(tui::vec2i(1, 1), tui::SIZE::CONSTANT);
		r0.setPosition(tui::position(tui::vec2i(20, 0)));
		r0.setChar(tui::console_char(219));
		tui::rectangle r1;
		r1.setSize(tui::vec2i(1, 1), tui::SIZE::CONSTANT);
		r1.setPosition(tui::position(tui::vec2i(22, 0)));
		r1.setChar(tui::console_char(219));
		tui::rectangle r2;
		r2.setSize(tui::vec2i(1, 1), tui::SIZE::CONSTANT);
		r2.setPosition(tui::position(tui::vec2i(24, 0)));
		r2.setChar(tui::console_char(219));

		tui::navigation_group act_group;
		act_group.activate();
		act_group.setKeyComboNext({ tui::KEYBOARD::CTRL, tui::KEYBOARD::RIGHT });
		//act_group.addElement(r0);
		//act_group.addElement(r1);
		//act_group.addElement(r2);
		act_group.addElement(text);
		act_group.addElement(input_text);



		for (;;)
		{

			if (tui::KEYBOARD::isKeyPressedBuffered(tui::KEYBOARD::CTRL)) {
				rect.move(tui::vec2i(1, 0));
			}
			if (tui::KEYBOARD::isKeyPressedBuffered(tui::KEYBOARD::LEFT)) {
				rect.move(tui::vec2i(-1, 0));
			}


			auto start = std::chrono::steady_clock::now();
			con.clear();

			act_group.update();

			con.draw(rect);

			con.draw(box1);
			con.draw(mainBox);
			

			con.draw(text);

			t_s += tui::KEYBOARD::getInputAsString(TUI_BUFFERED_INPUT);
		
			input_text.setText(t_s);
			con.draw(input_text);

			con.draw(r0);
			con.draw(r1);
			con.draw(r2);

			con.draw(grp);

			con.display();
			
			auto end = std::chrono::steady_clock::now();
			auto diff = end - start;
	
			 
			std::cout << std::endl << std::chrono::duration <double, std::milli>(diff).count() << " ms" << std::endl;
			std::cout << "X: " << con.getSize().x << " Y: " << con.getSize().y << std::endl;
			std::cout << "Xb: " << mainBox.getSize().x << " Yb: " << mainBox.getSize().y << std::endl;
			std::cout << text.isActive();
	
		}
	
	return 0;
}