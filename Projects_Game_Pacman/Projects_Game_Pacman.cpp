#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include <vector>
#include <iostream>
#include <algorithm> 
#include <random>
const int CELL_SIZE = 20;
const int PACMAN_SIZE = 16;
const int MAP_WIDTH = 21;
const int MAP_HEIGHT = 21;

std::vector<std::string> map_sketch = {
    "#####################",
    "#.........#.........#",
    "#o###.###.#.###.###o#",
    "#...................#",
    "#.###.#.#####.#.###.#",
    "#.....#...#...#.....#",
    "#####.### # ###.#####",
    "    #.#   0   #.#    ",
    "#####.# ##=## #.#####",
    "#.....  #123#  .....#",
    "#####.# ##### #.#####",
    "    #.#       #.#    ",
    "#####.# ##### #.#####",
    "#.........#.........#",
    "#.###.###.#.###.###.#",
    "#o..#.....P.....#..o#",
    "###.#.#.#####.#.#.###",
    "#.....#...#...#.....#",
    "#.#######.#.#######.#",
    "#...................#",
    "#####################"
};

// Lớp nhân vật chung (Pac-Man và ma)
struct Entity {
    sf::CircleShape shape;
    sf::Vector2f direction;
    float speed;

    // Hàm kiểm tra xem nhân vật có thể di chuyển đến vị trí mới không
    bool canMove(sf::Vector2f newPos) {
        // Kiểm tra 4 góc của Pac-Man để tránh đi vào tường
        int x1 = static_cast<int>((newPos.x - PACMAN_SIZE / 2) / CELL_SIZE);
        int y1 = static_cast<int>((newPos.y - PACMAN_SIZE / 2) / CELL_SIZE);
        int x2 = static_cast<int>((newPos.x + PACMAN_SIZE / 2) / CELL_SIZE);
        int y2 = static_cast<int>((newPos.y + PACMAN_SIZE / 2) / CELL_SIZE);

        return (map_sketch[y1][x1] != '#' && map_sketch[y1][x2] != '#' &&
            map_sketch[y2][x1] != '#' && map_sketch[y2][x2] != '#');
    }
};

// Lớp ma
class Ghost {
public:
    sf::CircleShape shape;
    sf::Vector2f direction;
    float speed;

    Ghost(float x, float y, sf::Vector2f dir) {
        shape.setRadius(PACMAN_SIZE / 2);
        shape.setFillColor(sf::Color::Red);
        shape.setOrigin(PACMAN_SIZE / 2, PACMAN_SIZE / 2);
        shape.setPosition(static_cast<float>(x * CELL_SIZE + CELL_SIZE / 2),
                          static_cast<float>(y * CELL_SIZE + CELL_SIZE / 2));

        direction = dir;
        speed = 60.0f;
    }

    bool canMove(sf::Vector2f newPos) {
        float left = newPos.x - shape.getRadius(); // Lấy cạnh trái
        float right = newPos.x + shape.getRadius();
        float top = newPos.y - shape.getRadius();
        float bottom = newPos.y + shape.getRadius();

        int x1 = static_cast<int>(left / CELL_SIZE);
        int y1 = static_cast<int>(top / CELL_SIZE);
        int x2 = static_cast<int>(right / CELL_SIZE);
        int y2 = static_cast<int>((newPos.y + shape.getRadius()) / CELL_SIZE);

        // Kiểm tra xem bốn góc có đụng tường không
        if (map_sketch[y1][x1] == '#' || map_sketch[y1][x2] == '#' ||
            map_sketch[y2][x1] == '#' || map_sketch[y2][x2] == '#') {
            return false;
        }
        return true;
    }

    void update(float deltaTime, const std::vector<Ghost>& ghosts) {
        sf::Vector2f newPos = shape.getPosition() + direction * speed * deltaTime;

        // Nếu không thể di chuyển, chọn hướng mới ngẫu nhiên
        if (!canMove(newPos) || isCollidingWithOtherGhosts(newPos, ghosts)) {
            std::vector<sf::Vector2f> possibleDirections;
            std::vector<sf::Vector2f> allDirections = {
                {1, 0}, {-1, 0}, {0, 1}, {0, -1}
            };

            // Kiểm tra các hướng có thể đi mà không va vào tường hoặc ma khác
            for (auto& dir : allDirections) {
                if (canMove(shape.getPosition() + dir * static_cast<float>(CELL_SIZE))) {
                    possibleDirections.push_back(dir);
                }
            }

            // Nếu có hướng đi hợp lệ, chọn ngẫu nhiên một hướng
            if (!possibleDirections.empty()) {
                static std::random_device rd;
                static std::mt19937 gen(rd());
                std::shuffle(possibleDirections.begin(), possibleDirections.end(), gen);

                for (const auto& dirOption : possibleDirections) {
                    if (dirOption != sf::Vector2f(-direction.x, -direction.y)) { // Không lùi lại
                        direction = dirOption;
                        break;
                    }
                }

            }

            // Cập nhật vị trí nếu có thể di chuyển
            sf::Vector2f newPos = shape.getPosition() + direction * speed * deltaTime;
            if (canMove(newPos)) {
                shape.setPosition(newPos);
            }
        }
    }

    // Kiểm tra xem có con ma nào khác đang đứng ở vị trí mới không
    bool isCollidingWithOtherGhosts(sf::Vector2f newPos, const std::vector<Ghost>& ghosts) {
        for (const auto& ghost : ghosts) {
            if (&ghost != this && ghost.shape.getGlobalBounds().contains(newPos)) {
                return true; // Nếu có ma khác ở đó, không di chuyển
            }
        }
        return false;
    }

    // Hàm đổi hướng ngẫu nhiên khi đến ngã rẽ
    void changeDirection(const std::vector<Ghost>& ghosts) {
        std::vector<sf::Vector2f> possibleDirections;
        std::vector<sf::Vector2f> allDirections = {
            {1, 0}, {-1, 0}, {0, 1}, {0, -1}
        };

        // Loại bỏ hướng ngược lại để tránh đi lui
        sf::Vector2f oppositeDirection = { -direction.x, -direction.y };
        allDirections.erase(std::remove(allDirections.begin(), allDirections.end(), oppositeDirection), allDirections.end());

        // Kiểm tra và chọn hướng có thể đi
        for (auto& newDir : allDirections) {
            sf::Vector2f newPos = shape.getPosition() + newDir * static_cast<float>(CELL_SIZE);
            if (canMove(newPos) && !isCollidingWithOtherGhosts(newPos, ghosts)) {
                possibleDirections.push_back(newDir);
            }
        }

        // Nếu có hướng hợp lệ, chọn hướng ngẫu nhiên để đi
        if (!possibleDirections.empty()) {
            static std::random_device rd;
            static std::mt19937 g(rd());
            std::shuffle(possibleDirections.begin(), possibleDirections.end(), g);
            direction = possibleDirections[0];
        }
    }

};

bool checkCollision(const sf::CircleShape& pacman, const sf::CircleShape& ghost) {
    return pacman.getGlobalBounds().intersects(ghost.getGlobalBounds());
}

class Pacman {
public:
    sf::CircleShape shape;
    sf::Vector2f direction;
    float speed;

    Pacman() {
        shape.setRadius(PACMAN_SIZE / 2);
        shape.setFillColor(sf::Color::Yellow);
        shape.setOrigin(PACMAN_SIZE / 2, PACMAN_SIZE / 2);
        shape.setPosition(static_cast<float>(10 * CELL_SIZE + CELL_SIZE / 2),
                          static_cast<float>(15 * CELL_SIZE + CELL_SIZE / 2));
        direction = { 1, 0 };
        speed = 36.0f;
    }

    void handleInput() {
        sf::Vector2f newDir = direction;

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::W) || sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) {
            newDir = { 0, -1 };
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::S) || sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) {
            newDir = { 0, 1 };
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::A) || sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
            newDir = { -1, 0 };
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::D) || sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
            newDir = { 1, 0 };
        }

        // Nếu có thể di chuyển theo hướng mới, thì cập nhật hướng
        sf::Vector2f newPos = shape.getPosition() + newDir * speed * 0.5f; // giả sử deltaTime = 0.016 giây
        if (canMove(newPos)) {
            direction = newDir;
        }
    }

    // Hàm kiểm tra có thể di chuyển không (giống với ma)
    bool canMove(sf::Vector2f newPos) {
        int xLeft = static_cast<int>((newPos.x - PACMAN_SIZE / 2) / CELL_SIZE);
        int xRight = static_cast<int>((newPos.x + PACMAN_SIZE / 2 - 1) / CELL_SIZE);
        int yTop = static_cast<int>((newPos.y - PACMAN_SIZE / 2) / CELL_SIZE);
        int yBottom = static_cast<int>((newPos.y + PACMAN_SIZE / 2) / CELL_SIZE);

        if (map_sketch[yTop][xLeft] == '#' || map_sketch[yTop][xLeft] == ' ' ||
            map_sketch[yBottom][xLeft] == '#' || map_sketch[yBottom][xLeft] == ' ' ||
            map_sketch[yTop][xRight] == '#' || map_sketch[yTop][xRight] == ' ' ||
            map_sketch[yBottom][xRight] == '#' || map_sketch[yBottom][xRight] == ' ') {
            return false;
        }
        return true;
    }

    void update(float deltaTime) {
        sf::Vector2f newPos = shape.getPosition() + direction * speed * deltaTime;

        // Nếu có thể di chuyển, cập nhật vị trí
        if (canMove(newPos)) {
            shape.setPosition(newPos);
        }
        else {
            // Nếu gặp tường, tìm hướng mới để rẽ khi có thể
            std::vector<sf::Vector2f> possibleTurns;
            std::vector<sf::Vector2f> allDirections = {
                {1, 0}, {-1, 0}, {0, 1}, {0, -1}
            };

            for (auto& newDir : allDirections) {
                sf::Vector2f checkPos = shape.getPosition() + newDir * static_cast<float>(CELL_SIZE);
                if (canMove(checkPos)) {
                    possibleTurns.push_back(newDir);
                }
            }

            // Nếu có hướng rẽ hợp lệ, chọn hướng ngẫu nhiên
            if (!possibleTurns.empty()) {
                static std::random_device rd;
                static std::mt19937 g(rd());
                std::shuffle(possibleTurns.begin(), possibleTurns.end(), g);
                direction = possibleTurns[0];
            }
        }

        // Xử lý dịch chuyển qua biên bản đồ (wrap-around)
        sf::Vector2f pos = shape.getPosition();
        if (pos.x < 0) {
            shape.setPosition(MAP_WIDTH * CELL_SIZE, pos.y);  // Đi qua trái -> xuất hiện bên phải
        }
        else if (pos.x > MAP_WIDTH * CELL_SIZE) {
            shape.setPosition(0, pos.y);  // Đi qua phải -> xuất hiện bên trái
        }

        if (pos.y < 0) {
            shape.setPosition(pos.x, MAP_HEIGHT * CELL_SIZE);  // Đi qua trên -> xuất hiện bên dưới
        }
        else if (pos.y > MAP_HEIGHT * CELL_SIZE) {
            shape.setPosition(pos.x, 0);  // Đi qua dưới -> xuất hiện bên trên
        }

        // Tiếp tục di chuyển nếu có thể
        sf::Vector2f checkNewPos = shape.getPosition() + direction * speed * deltaTime;
        if (canMove(checkNewPos)) {
            shape.setPosition(checkNewPos);
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
        shape.setPosition(static_cast<float>(x * CELL_SIZE + CELL_SIZE / 3),
            static_cast<float>(y * CELL_SIZE + CELL_SIZE / 3));
    }
};

void drawMap(sf::RenderWindow& window, std::vector<Coin>& coins) {

    sf::RectangleShape wall(sf::Vector2f(CELL_SIZE, CELL_SIZE));
    wall.setFillColor(sf::Color::Blue);

    for (int y = 0; y < MAP_HEIGHT; ++y) {
        for (int x = 0; x < MAP_WIDTH; ++x) {
            if (map_sketch[y][x] == '#') {
                wall.setPosition(static_cast<float>(x * CELL_SIZE), static_cast<float>(y * CELL_SIZE));
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

    // Tạo ma
    std::vector<Ghost> ghosts = {
        Ghost(9, 9, {1, 0}),
        Ghost(11, 9, {-1, 0}),
        Ghost(9, 11, {0, 1}),
        Ghost(11, 11, {0, -1})
    };

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
        }
   
        float delta_time = clock.restart().asSeconds();
        pacman.handleInput();
        pacman.update(delta_time);
 
        // Cập nhật vị trí ma
        for (auto& ghost : ghosts) {
            ghost.update(delta_time, ghosts); 

            // Kiểm tra va chạm
            if (checkCollision(pacman.shape, ghost.shape)) {
                std::cout << "Game Over!" << std::endl;
                window.close();  // Thoát game khi Pac-Man bị bắt
            }
        }

        // Xử lý ăn coin
        sf::Vector2f pacPos = pacman.shape.getPosition();
        auto it = std::remove_if(coins.begin(), coins.end(), [&](Coin& coin) {
            return pacman.shape.getGlobalBounds().intersects(coin.shape.getGlobalBounds());
            });
        coins.erase(it, coins.end());

        window.clear();
        drawMap(window, coins);
        window.draw(pacman.shape);
        for (auto& ghost : ghosts) {
            window.draw(ghost.shape);
        }
        window.display();
    }
}