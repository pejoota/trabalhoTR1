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

	int curX = 0;
	int firstBit = wave_start / (level_width + 1);
	int posInFirstBit = wave_start % (level_width + 1);

	// If must repeat and is already past the bits provided
	// go back.
	if(repeat && firstBit >= bit_levels.size()) {
		firstBit %= bit_levels.size();
	}


	for (int i = firstBit; i < bit_levels.size(); ++i) {
		int cur_level = bit_levels[i];

		// Prints the bit level
		for(int j = posInFirstBit; j < level_width; j++, curX++) {
			if(curX >= width)
				return;

			showBlockAt(transitions[cur_level + 1][cur_level + 1], y, x + curX);
		}

		// Resets the position in the first bit after
		// the first bit is shown
		posInFirstBit = 0;

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
		
		if(curX >= width)
			return;

		showBlockAt(transitions[cur_level + 1][next_level + 1], y, x + curX);
		curX++;
	}
}