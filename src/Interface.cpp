#include "../header/Interface.hpp"

void showBlockAt(std::vector<std::string> &lineBlock, int y, int x) {
	for(int j = 0; j < lineBlock.size(); j++) {
		mvaddstr(y + j, x, lineBlock[j].c_str());
	}
}

void showWave(std::vector<int> bit_levels, int y, int x, int wave_start, int width, int level_width, bool repeat) {

	// Map of the bits transitions
	// Accessed with [A][B] where
	//	A is the current bit level value
	//	B is the next bit level value
	static std::vector<std::string> transitions[3][3] = 
		{
			{
				{
					" ",
					" ",
					" ",
					" ",
					" ",
					" ",
					"─"
				}, {
					" ",
					" ",
					" ",
					"┌",
					"│",
					"│",
					"┘"
				}, {
					"┌",
					"│",
					"│",
					"│",
					"│",
					"│",
					"┘"
				}
			}, {
				{
					" ",
					" ",
					" ",
					"┐",
					"│",
					"│",
					"└"
				}, {
					" ",
					" ",
					" ",
					"─",
					" ",
					" ",
					" "
				}, {
					"┌",
					"│",
					"│",
					"┘",
					" ",
					" ",
					" "
				}
			}, {
				{
					"┐",
					"│",
					"│",
					"│",
					"│",
					"│",
					"└"
				}, {
					"┐",
					"│",
					"│",
					"└",
					" ",
					" ",
					" "
				}, {
					"─",
					" ",
					" ",
					" ",
					" ",
					" ",
					" "
				}
			}
		};

	int cur_x = 0;
	int first_bit = wave_start / (level_width + 1);
	int pos_in_first_bit = wave_start % (level_width + 1);

	// If must repeat and is already past the bits provided
	// go back.
	if(repeat && first_bit >= bit_levels.size()) {
		first_bit %= bit_levels.size();
	}


	for (int i = first_bit; i < bit_levels.size(); ++i) {
		int cur_level = bit_levels[i];

		// Prints the bit level
		for(int j = pos_in_first_bit; j < level_width; j++, cur_x++) {
			if(cur_x >= width)
				return;

			showBlockAt(transitions[cur_level + 1][cur_level + 1], y, x + cur_x);
		}

		// Resets the position in the first bit after
		// the first bit is shown
		pos_in_first_bit = 0;

		int next_level = cur_level;

		// Prints the transition from one bit level to another
		if(i < bit_levels.size() - 1)
			// If it's not at the end of the bits, take the next
			// level for the transition
			next_level = bit_levels[i + 1];
		else if(repeat) {
			// If it is at the last bit and must repeat
			// take the first bit as the next transition and
			// reset i to 0 (set to -1 due to increment in for).
			next_level = bit_levels[0];
			i = -1;
		}
		
		if(cur_x >= width)
			return;

		showBlockAt(transitions[cur_level + 1][next_level + 1], y, x + cur_x);
		cur_x++;
	}
}