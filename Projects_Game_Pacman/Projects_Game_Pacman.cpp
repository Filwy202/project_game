#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include <vector>

const int CELL_SIZE = 20;
const int PACMAN_SIZE = 18;
const int MAP_WIDTH = 21;
const int MAP_HEIGHT = 21;

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
        shape.setPosition(10 * CELL_SIZE + CELL_SIZE / 2, 15 * CELL_SIZE + CELL_SIZE / 2);
        direction = { 1, 0 };
        speed = 100.0f;
    }

    void handleInput() {
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) direction = { 0, -1 };
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) direction = { 0, 1 };
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) direction = { -1, 0 };
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) direction = { 1, 0 };
    }

    bool canMove(sf::Vector2f newPos) {
        int x = static_cast<int>(newPos.x / CELL_SIZE);
        int y = static_cast<int>(newPos.y / CELL_SIZE);
        if (x < 0 || x >= MAP_WIDTH || y < 0 || y >= MAP_HEIGHT) return false;
        return map_sketch[y][x] != '#';
    }

    void update(float deltaTime) {
        sf::Vector2f newPos = shape.getPosition() + direction * speed * deltaTime;
        if (canMove(newPos)) {
            shape.setPosition(newPos);
        }
    }
};

struct Coin {
    sf::CircleShape shape;
    int x, y;

    Coin(int x, int y) : x(x), y(y) {
        shape.setRadius(4);
        shape.setFillColor(sf::Color::White);
        shape.setOrigin(4, 4);
        shape.setPosition(x * CELL_SIZE + CELL_SIZE / 2, y * CELL_SIZE + CELL_SIZE / 2);
    }
};

void drawMap(sf::RenderWindow& window, std::vector<Coin>& coins) {
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

    for (auto& coin : coins) {
        window.draw(coin.shape);
    }
}

int main() {
    sf::RenderWindow window(sf::VideoMode(MAP_WIDTH * CELL_SIZE, MAP_HEIGHT * CELL_SIZE), "Pac-Man", sf::Style::Close);
    window.setFramerateLimit(60);

    Pacman pacman;
    sf::Clock clock;
    std::vector<Coin> coins;

    for (int y = 0; y < MAP_HEIGHT; ++y) {
        for (int x = 0; x < MAP_WIDTH; ++x) {
            if (map_sketch[y][x] == '.' || map_sketch[y][x] == 'o') {
                coins.emplace_back(x, y);
            }
        }
    }

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
        }

        pacman.handleInput();
        float delta_time = clock.restart().asSeconds();
        pacman.update(delta_time);

        // Xử lý ăn coin
        sf::Vector2f pacPos = pacman.shape.getPosition();
        coins.erase(std::remove_if(coins.begin(), coins.end(), [&](Coin& coin) {
            return (std::abs(coin.shape.getPosition().x - pacPos.x) < 10 &&
                std::abs(coin.shape.getPosition().y - pacPos.y) < 10);
            }), coins.end());

        window.clear();
        drawMap(window, coins);
        window.draw(pacman.shape);
        window.display();
    }
}
