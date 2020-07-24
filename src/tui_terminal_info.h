#pragma once
#include <string>
#include <vector>
#include <array>
#include <cstdio>
#include <cmath>
#include <algorithm>

namespace tui 
{
	namespace input
	{
		struct key_seq_pair
		{
			std::string name;
			std::vector<int> seq;
			key_seq_pair() {}
			key_seq_pair(std::string Name) : name(Name) {}
			key_seq_pair(std::string Name, std::vector<int> Seq) : name(Name), seq(Seq) {}
		};



		struct terminal_info
		{
			terminal_info()
			{
				set();
			}

			void set()
			{
#ifdef  TUI_TARGET_SYSTEM_LINUX
				std::array<char, 128> buffer;
				std::string infocmp;
				FILE* pipe = popen("infocmp", "r");
				if (pipe)
				{
					while (fgets(buffer.data(), buffer.size(), pipe) != NULL) {
						infocmp += buffer.data();
					}
				}
				pclose(pipe);

				auto getSeq = [&](std::string seq_name)
				{
					//https://man7.org/linux/man-pages/man5/terminfo.5.html
					std::vector<int> seq;

					if (infocmp.find(seq_name) != std::string::npos)
					{
						int s = infocmp.find(seq_name);
						s += seq_name.size() + 1;

						while (infocmp[s] != ',')
						{
							if (infocmp[s] == '\\')
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
							else if (infocmp[s] == '^')
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

					if (sequences[i].seq.size() > longest)
					{
						longest = sequences[i].seq.size();
					}
					if (sequences[i].seq.size() < shortest)
					{
						shortest = sequences[i].seq.size();
					}
				}

				longest_seq = longest;
				shortest_seq = shortest;

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
#endif
#ifdef  TUI_TARGET_SYSTEM_WINDOWS
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

			std::vector<key_seq_pair> sequences =
			{
				{"kpp"}, //PGUP
				{"knp"}, //PGDN

				{"kdch1"}, //DELETE
				{"kich1"}, //INSERT
				{"kend"}, //END
				{"khome"}, //HOME

				{"kf1"}, //F1
				{"kf2"},
				{"kf3"},
				{"kf4"},
				{"kf5"},
				{"kf6"},
				{"kf7"},
				{"kf8"},
				{"kf9"},
				{"kf10"},
				{"kf11"},
				{"kf12"}, //F12

				{"kcuu1"}, //UP
				{"kcud1"}, //DOWN
				{"kcub1"}, //LEFT
				{"kcuf1"} //RIGHT
			};


			std::string smkx;
			std::string rmkx;

			unsigned short longest_seq;
			unsigned short shortest_seq;

			//return position of sequence, if there is no given sequence return -1
			int getSeqNumber(std::vector<int> seq)
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

			std::vector<int> getSeq(unsigned int seq)
			{
				return sequences[seq].seq;
			}

		};
	}
}
