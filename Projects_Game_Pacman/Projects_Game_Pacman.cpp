#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include <vector>

const int CELL_SIZE = 20;
const int PACMAN_SIZE = 18;
const int MAP_WIDTH = 21;
const int MAP_HEIGHT = 21;
const int FRAME_DURATION = 16667;

std::vector<std::string> map_sketch = {
    " ################### ",
    " #........#........# ",
    " #o##.###.#.###.##o# ",
    " #.................# ",
    " #.##.#.#####.#.##.# ",
    " #....#...#...#....# ",
    " ####.### # ###.#### ",
    "    #.#   0   #.#    ",
    "#####.# ##=## #.#####",
    "     .  #123#  .     ",
    "#####.# ##### #.#####",
    "    #.#       #.#    ",
    " ####.# ##### #.#### ",
    " #........#........# ",
    " #.##.###.#.###.##.# ",
    " #o.#.....P.....#.o# ",
    " ##.#.#.#####.#.#.## ",
    " #....#...#...#....# ",
    " #.######.#.######.# ",
    " #.................# ",
    " ################### "
};

class Pacman {
public:
    sf::CircleShape shape;
    sf::Vector2f direction;
    float speed;

    Pacman() {
        shape.setRadius(PACMAN_SIZE / 2);
        shape.setFillColor(sf::Color::Yellow);
        shape.setOrigin(PACMAN_SIZE / 2, PACMAN_SIZE / 2);
        // Đặt Pacman vào vị trí hợp lệ trong bản đồ (ở giữa màn hình)
        shape.setPosition(10 * CELL_SIZE + CELL_SIZE / 2, 15 * CELL_SIZE + CELL_SIZE / 2);
        direction = { 1, 0 }; // Mặc định di chuyển sang phải
        speed = 100.0f;
    }

    void handleInput() {
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) direction = { 0, -1 };
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) direction = { 0, 1 };
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) direction = { -1, 0 };
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) direction = { 1, 0 };
    }

    bool canMove(sf::Vector2f newPos) {
        int x = static_cast<int>((newPos.x) / CELL_SIZE);
        int y = static_cast<int>((newPos.y) / CELL_SIZE);
        if (x < 0) return true; // Đi từ trái qua phải
        if (x >= MAP_WIDTH) return true; // Đi từ phải qua trái
        if (y < 0 || y >= MAP_HEIGHT) return false;
        return map_sketch[y][x] != '#'; // Không cho phép di chuyển vào tường
    }

    void update(float deltaTime) {
        sf::Vector2f newPos = shape.getPosition() + direction * speed * deltaTime;
        if (canMove(newPos)) {
            if (newPos.x < 0) {
                shape.setPosition((MAP_WIDTH - 1) * CELL_SIZE, shape.getPosition().y);
            }
            else if (newPos.x >= MAP_WIDTH * CELL_SIZE) {
                shape.setPosition(0, shape.getPosition().y);
            }
            else {
                shape.setPosition(newPos);
            }
        }
    }

    void draw(sf::RenderWindow& window) {
        window.draw(shape);
    }
};

void drawMap(sf::RenderWindow& window) {
    sf::RectangleShape wall(sf::Vector2f(CELL_SIZE, CELL_SIZE));
    wall.setFillColor(sf::Color::Blue);

    for (int y = 0; y < MAP_HEIGHT; ++y) {
        for (int x = 0; x < MAP_WIDTH; ++x) {
            if (map_sketch[y][x] == '#') {
                wall.setPosition(x * CELL_SIZE, y * CELL_SIZE);
                window.draw(wall);
            }
        }
    }
}

int main()
{
    sf::Clock clock;
    Pacman pacman;

    sf::RenderWindow window(sf::VideoMode(MAP_WIDTH * CELL_SIZE, MAP_HEIGHT * CELL_SIZE), "Pac-Man", sf::Style::Close);
    window.setFramerateLimit(60);

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
            {
                window.close();
            }
        }

        pacman.handleInput();

        float delta_time = clock.restart().asSeconds();
        pacman.update(delta_time);

        window.clear();
        drawMap(window);
        pacman.draw(window);
        window.display();
    }
}