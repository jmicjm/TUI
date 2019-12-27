#pragma once
#include <string>
#include <vector>
#include <array>
#include <cstdio>
#include <cmath>

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

				int longest = 0;
				int shortest = pow(2, sizeof(int) * 8) - 1;

				for (int i = 0; i < sequences.size(); i++)
				{
					if (infocmp.find(sequences[i].name) != std::string::npos)
					{
						int s = infocmp.find(sequences[i].name);
						s += sequences[i].name.size();
						s += 3; // =\E

						sequences[i].seq = { 27 }; //ESC

						while (infocmp[s] != ',')
						{
							sequences[i].seq.push_back(infocmp[s]);
							s++;
						}
					}
					else
					{
						sequences[i].seq = { -2 };
					}

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


				if (infocmp.find("smkx") != std::string::npos)
				{
					int s = infocmp.find("smkx");
					s += 7; //smkx=\E
					smkx = '\033';

					while (infocmp[s] != ',')
					{
						smkx += infocmp[s];
						s++;
					}
				}
				if (infocmp.find("rmkx") != std::string::npos)
				{
					int s = infocmp.find("rmkx");
					s += 7; //rmkx=\E
					rmkx = '\033';

					while (infocmp[s] != ',')
					{
						rmkx += infocmp[s];
						s++;
					}
				}

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

			int longest_seq;
			int shortest_seq;

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

		};
	}
}
