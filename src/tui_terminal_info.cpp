#include "tui_terminal_info.h"

#include <array>
#include <cstdio>
#include <cmath>
#include <algorithm>
#include <iostream>
#include <utility>

namespace tui
{
	terminal_info term_info;

#if defined(__linux__) || defined(__unix__) 
	std::string getCommandOutput(const char* command)
	{
		std::array<char, 128> buffer;
		std::string output;
		FILE* pipe = popen(command, "r");
		if (pipe)
		{
			while (fgets(buffer.data(), buffer.size(), pipe) != NULL) {
				output += buffer.data();
			}
		}
		pclose(pipe);

		return output;
	}
#endif

	void terminal_info::set()
	{
#if defined(__linux__) || defined(__unix__) 
		std::string infocmp = getCommandOutput("infocmp") + getCommandOutput("infocmp -x");

		auto getSeq = [&](std::string seq_name)
		{
			//https://man7.org/linux/man-pages/man5/terminfo.5.html
			std::vector<int> seq;

			if (infocmp.find(seq_name) != std::string::npos)
			{
				int s = infocmp.find(seq_name);
				s += seq_name.size() + 1;

				while (s < infocmp.size() && infocmp[s] != ',')
				{
					if (infocmp[s] == '\\' && s + 1 < infocmp.size())
					{
						s++;
						switch (infocmp[s])
						{
						case 'E':
						case 'e':
							seq.push_back(27);
							break;
						case 'n':
						case 'l'://?
							seq.push_back('\n');
							break;
						case 'r':
							seq.push_back('\r');
							break;
						case 't':
							seq.push_back('\t');
							break;
						case 'b':
							seq.push_back('\b');
							break;
						case 'f':
							seq.push_back('\f');
							break;
						case 's':
							seq.push_back(' ');
							break;
						case '0':
							seq.push_back(0);
							break;
						default:
							seq.push_back(infocmp[s]); // eg \^ => ^
						}
						s++;
					}
					else if (infocmp[s] == '^' && s + 1 < infocmp.size())
					{
						s++;
						if (infocmp[s] == '?')
						{
							seq.push_back(127);
						}
						else
						{
							seq.push_back(infocmp[s] & 0x1F);
						}
						s++;
					}
					else
					{
						seq.push_back(infocmp[s]);
						s++;
					}
				}
			}

			return seq;
		};

		unsigned short longest = 0;
		unsigned short shortest = pow(2, sizeof(short) * 8) - 1;

		for (int i = 0; i < sequences.size(); i++)
		{
			sequences[i].seq = getSeq(sequences[i].name);

			longest = std::max(sequences[i].seq.size(), (size_t)longest);
			shortest = std::min(sequences[i].seq.size(), (size_t)shortest);
		}

		longest_seq = longest;
		shortest_seq = shortest;

		if (longest_seq <= 1)
		{
			std::cout << "There were some problems during retrieving information about terminal.\n"
				"Nonalphanumerical input(arrows F-keys etc) will not work.\n"
				"Possible reasons: no infocmp present or some problem with it, invalid terminal entry\n"
				"press 'q' to exit or any other key to continue\n";
			char k;
			std::cin >> k;

			if (k == 'q' || k == 'Q') { std::exit(0); }
		}

		auto vecToStr = [](std::vector<int> vec)
		{
			std::string str;
			for (int i = 0; i < vec.size(); i++)
			{
				str.push_back(vec[i]);
			}
			return str;
		};

		smkx = vecToStr(getSeq("smkx"));
		rmkx = vecToStr(getSeq("rmkx"));

		std::string t_home = vecToStr(getSeq("home"));
		home = t_home.size() > 0 ? t_home : "\033[H";//if no entry replace with ansi one

		std::string t_civis = vecToStr(getSeq("civis"));
		civis = t_civis.size() > 0 ? t_civis : "\033[?25l";

		std::string t_cvvis = vecToStr(getSeq("cvvis"));
		cvvis = t_cvvis.size() > 0 ? t_cvvis : "\033[?25h";


		std::string colorterm = getCommandOutput("echo $COLORTERM");

		rgb_color = colorterm.find("truecolor") != std::string::npos || colorterm.find("24bit") != std::string::npos;

#endif
#if defined(_WIN32)
		std::vector<std::pair<int,int>> keys =
		{
			{224, 73}, //PGUP
			{224, 81}, //PGDN
			{224, 83}, //DEL
			{224, 82}, //INS
			{224, 79}, //END
			{224, 71}, //HOME

			{0, 59}, //F1
			{0, 60},
			{0, 61},
			{0, 62},
			{0, 63},
			{0, 64},
			{0, 65},
			{0, 66},
			{0, 67},
			{0, 68},
			{224, 133},
			{224, 134}, //F12

			{224, 72}, //UP
			{224, 80}, //DOWN
			{224, 75}, //LEFT
			{224, 77}, //RIGHT

			//ALT KEYS
			{0, 153}, //ALT+PGUP
			{0, 161}, //ALT+PGDN
			{0, 163}, //ALT+DEL
			{0, 162}, //ALT+INS
			{0, 159}, //ALT+END
			{0, 151}, //ALT+HOME

			{0, 152}, //ALT+UP
			{0, 160}, //ALT+DOWN
			{0, 155}, //ALT+LEFT
			{0, 157},  //ALT+RIGHT

			//SHIFT KEYS
			{0, 84}, //SHIFT+F1
			{0, 85},
			{0, 86},
			{0, 87},
			{0, 88},
			{0, 89},
			{0, 90},
			{0, 91},
			{0, 92},
			{0, 93},
			{224, 135},
			{224, 136}, //SHIFT+F12

			{-1, -1}, //SHIFT+UP
			{-1, -1}, //SHIFT+DOWN
			{-1, -1}, //SHIFT+LEFT
			{-1, -1}, //SHIFT+RIGHT

			//CTRL KEYS
			{-1, -1}, //CTRL+PGUP
			{224, 118}, //CTRL+PGDN
			{224, 147}, //CTRL+DEL
			{224, 146}, //CTRL+INS
			{224, 117}, //CTRL+END
			{224, 119}, //CTRL+HOME

			{0, 94}, //CTRL+F1
			{0, 95},
			{0, 96},
			{0, 97},
			{0, 98},
			{0, 99},
			{0, 100},
			{0, 101},
			{0, 102},
			{0, 103},
			{224, 137},
			{224, 138}, //CTRL+F12

			{224, 141}, //CTRL+UP
			{224, 145}, //CTRL+DOWN
			{224, 115}, //CTRL+LEFT
			{224, 116}  //CTRL+RIGHT
		};

		for (int i = 0; i < sequences.size(); i++)
		{
			sequences[i].seq = { keys[i].first, keys[i].second };
		}
#endif
	}

	int terminal_info::getSeqNumber(const std::vector<int>& seq)
	{
		for (int i = 0; i < sequences.size(); i++)
		{
			if (sequences[i].seq == seq)
			{
				return i;
			}
		}
		return -1;
	}

	std::vector<int> terminal_info::getSeq(unsigned int seq)
	{
		return sequences[seq].seq;
	}
}