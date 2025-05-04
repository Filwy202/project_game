#include <array>
#include <cmath>
#include <SFML/Graphics.hpp>

#include "Headers/Global.hpp"
#include "Headers/Pacman.hpp"
#include "Headers/Ghost.hpp"
#include "Headers/GhostManager.hpp"

GhostManager::GhostManager() :
	current_wave(0),
	wave_timer(LONG_SCATTER_DURATION),
	ghosts({Ghost(0), Ghost(1), Ghost(2), Ghost(3)})
{

}

void GhostManager::draw(bool i_flash, sf::RenderWindow& i_window)
{
	// Lặp qua từng bóng ma và vẽ nó
	for (Ghost& ghost : ghosts)
	{
		ghost.draw(i_flash, i_window);
	}
}

// Hàm reset trạng thái của bóng ma khi bắt đầu màn chơi mới hoặc sau khi reset
void GhostManager::reset(unsigned char i_level, const std::array<Position, 4>& i_ghost_positions)
{
	current_wave = 0;

	// Tính lại thời gian wave dựa trên level
	wave_timer = static_cast<unsigned short>(LONG_SCATTER_DURATION / pow(2, i_level));

	// Cập nhật lại vị trí ban đầu của các bóng ma
	for (unsigned char a = 0; a < 4; a++)
	{
		ghosts[a].set_position(i_ghost_positions[a].x, i_ghost_positions[a].y);
	}

	// Reset lại trạng thái của các bóng ma (có thể reset các giá trị trạng thái như hướng đi, chế độ...)
	for (Ghost& ghost : ghosts)
	{
		// Dùng vị trí của ghost[2] và ghost[0] để reset lại các ghost (có thể là để căn chỉnh lại hướng đi)
		ghost.reset(ghosts[2].get_position(), ghosts[0].get_position());
	}
}

// Hàm cập nhật trạng thái của các bóng ma
void GhostManager::update(unsigned char i_level, std::array<std::array<Cell, MAP_HEIGHT>, MAP_WIDTH>& i_map, Pacman& i_pacman)
{
	// Nếu Pac-Man không còn hiệu lực của energizer
	if (0 == i_pacman.get_energizer_timer()) 
	{
		// Nếu thời gian wave đã hết, chuyển sang sóng bóng ma tiếp theo
		if (0 == wave_timer)
		{
			if (7 > current_wave)
			{
				current_wave++;

				for (Ghost& ghost : ghosts)
				{
					ghost.switch_mode();
				}
			}

			
			if (1 == current_wave % 2)
			{
				wave_timer = CHASE_DURATION;
			}
			else if (2 == current_wave)
			{
				wave_timer = static_cast<unsigned short>(LONG_SCATTER_DURATION / pow(2, i_level));
			}
			else
			{
				wave_timer = static_cast<unsigned short>(SHORT_SCATTER_DURATION / pow(2, i_level));
			}
		}
		else
		{
			// Nếu chưa hết thời gian của wave, giảm thời gian wave xuống
			wave_timer--;
		}
	}

	for (Ghost& ghost : ghosts)
	{
		ghost.update(i_level, i_map, ghosts[0], i_pacman);
	}
}