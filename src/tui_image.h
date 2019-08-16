#pragma once
#include "tui_console.h"
#include "tui_text_utils.h"

namespace tui
{
	struct image : surface
	{
		void setImage(std::vector<symbol> image, unsigned int width)
		{
			if (width == 0) { return; }

			setSize({ { (int)width, (int)ceil(image.size() / width) } });

			for (int i = 0; i < image.size(); i++)
			{
				surface::setSymbolAt(image[i], {i%(int)width, (int)floor(i/width)});
			}
		}
		
		void setImageColorOnly(std::vector<COLOR> image, unsigned int width)
		{
			std::vector<symbol> temp;
			temp.resize(image.size());

			for (int i = 0; i < temp.size(); i++)
			{
				temp[i] = symbol(' ', color(COLOR::BLACK, image[i]));
			}

			setImage(temp, width);
		}

		void loadFromFile(std::string file)
		{

		}

	};
}
