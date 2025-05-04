#include <array>
#include <cmath>

#include "Headers/Global.hpp"
#include "Headers/MapCollision.hpp"

// Hàm kiểm tra va chạm và xử lý các vật thể
bool map_collision(bool i_collect_pellets, bool i_use_door, short i_x, short i_y, std::array<std::array<Cell, MAP_HEIGHT>, MAP_WIDTH>& i_map)
{
	bool output = 0;

	// Chuyển đổi tọa độ pixel thành tọa độ ô trên bản đồ
	float cell_x = i_x / static_cast<float>(CELL_SIZE);
	float cell_y = i_y / static_cast<float>(CELL_SIZE);

	// Kiểm tra va chạm với các ô trong bản đồ (4 góc)
	for (unsigned char a = 0; a < 4; a++)
	{
		short x = 0;
		short y = 0;

		// Tính toán tọa độ ô dựa trên tọa độ pixel
		switch (a)
		{
			case 0: 
			{
				x = static_cast<short>(floor(cell_x));
				y = static_cast<short>(floor(cell_y));

				break;
			}
			case 1: 
			{
				x = static_cast<short>(ceil(cell_x));
				y = static_cast<short>(floor(cell_y));

				break;
			}
			case 2: 
			{
				x = static_cast<short>(floor(cell_x));
				y = static_cast<short>(ceil(cell_y));

				break;
			}
			case 3: 
			{
				x = static_cast<short>(ceil(cell_x));
				y = static_cast<short>(ceil(cell_y));
			}
		}

		// Kiểm tra nếu tọa độ nằm trong phạm vi bản đồ
		if (0 <= x && 0 <= y && MAP_HEIGHT > y && MAP_WIDTH > x)
		{
			if (0 == i_collect_pellets)
			{
				// Kiểm tra nếu va chạm với tường
				if (Cell::Wall == i_map[x][y])
				{
					output = 1;
				}
				// Kiểm tra nếu không sử dụng cửa và va chạm với cửa
				else if (0 == i_use_door && Cell::Door == i_map[x][y])
				{
					output = 1;
				}
			}
			else 
			{

				// Kiểm tra nếu va chạm với energizer
				if (Cell::Energizer == i_map[x][y])
				{
					output = 1;

					// Đặt ô đó thành ô trống sau khi thu thập energize
					i_map[x][y] = Cell::Empty;
				}
				
				else if (Cell::Pellet == i_map[x][y])
				{
					// Đặt ô đó thành ô trống sau khi thu thập pellet
					i_map[x][y] = Cell::Empty;
				}
			}
		}
	}

	return output;
}