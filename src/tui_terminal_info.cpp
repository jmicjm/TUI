#include "tui_terminal_info.h"

#include <array>
#include <cstdio>
#include <cmath>
#include <algorithm>
#include <iostream>

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
		std::string infocmp = getCommandOutput("infocmp");

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


		std::string colorterm = getCommandOutput("echo $COLORTERM");

		rgb_color = colorterm.find("truecolor") != std::string::npos || colorterm.find("24bit") != std::string::npos;

#endif
#if defined(_WIN32)
		std::vector<int> keys =
		{
			73, 81, 83,82, 79, 71, 59, 60, 61, 62, 63, 64, 65, 66, 67, 68, 133, 134, 72, 80, 75, 77
		};

		for (int i = 0; i < sequences.size(); i++)
		{
			sequences[i].seq = { keys[i] };
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