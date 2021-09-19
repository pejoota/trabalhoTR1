#include <vector>
#include <string>
#include <ncurses.h>

#define REPEAT_WAVE true
#define NOREPEAT_WAVE false

void showBlockAt(std::vector<std::string> &lineBlock, int y, int x);
void showWave(std::vector<int> quadro, int y, int x, int wave_start, int width, int level_width, bool repeat);