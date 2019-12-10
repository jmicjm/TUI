#pragma once
#include "tui_surface.h"
#include "tui_text_utils.h"

#include <fstream>

namespace tui
{
	struct image : surface
	{
		const std::array<rgb, 16> color16 =
		{
			COLOR::BLACK, COLOR::BLUE, COLOR::GREEN, COLOR::CYAN,
			COLOR::RED, COLOR::MAGENTA, COLOR::BROWN, COLOR::LIGHTGRAY,
			COLOR::DARKGRAY, COLOR::LIGHTBLUE, COLOR::LIGHTGREEN, COLOR::LIGHTCYAN,
			COLOR::LIGHTRED, COLOR::LIGHTMAGENTA, COLOR::YELLOW, COLOR::WHITE
		};

		void setImage(std::vector<symbol> image, unsigned int width)
		{
			if (width == 0) { return; }

			setSizeInfo({ { (int)width, (int)ceil(image.size() / (float)width) } });

			for (int i = 0; i < image.size(); i++)
			{
				surface::setSymbolAt(image[i], {i%(int)width, (int)floor(i/width)});
			}
		}
		
		void setImageColorOnly(std::vector<int> image, unsigned int width)
		{
			std::vector<symbol> temp;
			temp.resize(image.size());

			for (int i = 0; i < temp.size(); i++)
			{
				if (image[i] == -1)
				{
					temp[i] = symbol((char32_t)0);
					continue;
				}
				temp[i] = symbol(' ', color(COLOR::BLACK, color16[image[i]]));
			}

			setImage(temp, width);
		}

		void loadFromFile(std::string file, unsigned int width)
		{
			std::vector<symbol> temp;

			std::ifstream input(file);

			std::string input_str;

			for (std::string line; std::getline(input, line);)
			{
				input_str += line;
			}
			input.close();

			std::vector<int> symbol_vals;
			std::string val;
			for (int i = 0; i < input_str.size(); i++)
			{
				auto isValidChar = [&]()
				{
					return input_str[i] != ' ' && !IsControl(input_str[i]) && input_str[i] != ',';
				};

				if (isValidChar())
				{
					val += input_str[i];
				}
				if ((!isValidChar() && val.size() > 0) || i == input_str.size() -1)
				{
					symbol_vals.push_back(std::stoi(val, nullptr, 16));
					val.clear();
				}

				if (input_str[i] == ',' || i == input_str.size() - 1)
				{
					if (symbol_vals.size() == 3)
					{
						temp.push_back(symbol(symbol_vals[0], { color16[(uint8_t)symbol_vals[1]], color16[(uint8_t)symbol_vals[2]] }));
					}
					else
					{
						temp.push_back(symbol('?', color()));
					}
					symbol_vals.clear();
				}
			}
			setImage(temp, width);
		}

		void loadFromFileColorOnly(std::string file, unsigned int width)
		{
			std::vector<symbol> temp;

			std::ifstream input(file);

			std::string input_str;

			for (std::string line; std::getline(input, line);)
			{
				input_str += line;
			}
			input.close();

			for (int i = 0; i < input_str.size(); i++)
			{
				std::string val;
				val.resize(1);
				if (input_str[i] != ' ' && !IsControl(input_str[i]))
				{
					if (input_str[i] == '-')
					{
						temp.push_back(symbol((char32_t)0));
						continue;
					}
					val[0] = input_str[i];
					temp.push_back(symbol(' ', {tui::COLOR::BLACK, color16[(uint8_t)std::stoi(val, nullptr, 16)]}));
				}
			}
			setImage(temp, width);
		}
	};
}
