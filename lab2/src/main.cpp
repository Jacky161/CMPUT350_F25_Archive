#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <algorithm>
#include <cstdint>
#include <iostream>
#include <memory>
#include <random>
#include <vector>

// Constants
const uint32_t WINDOW_WIDTH = 800;
const uint32_t WINDOW_HEIGHT = 800;
const uint32_t FPS_LIMIT = 60;

// Tube generation
const float TUBE_WIDTH = 80.0f;
const float TUBE_SPACING = 200.0f;
const float GAP_HEIGHT = 200.0f;

// Game physics
const float INITIAL_BIRD_VELOCITY_Y = 0.0f;
const float GRAVITY = 0.3f;
const float JUMP_SPEED = -5.0f; // (set velocity of bird in the Y direction upon jump)
const float TUBE_SPEED = 3.0f;

// TODO: (Q1)
//  Initial Bird Attributes
//  Initialize the global (constant) variables for it here (radius, position, color)
constexpr sf::Vector2f INITIAL_BIRD_POS { 100.0, 400.0 };
constexpr float BIRD_RADIUS { 15.0 };
constexpr sf::Color BIRD_COLOR { sf::Color::Yellow };

// ResourceManager just owns all the resources/assets you'd want in your game.
// In an engine, you'd probably want to make this more flexible than what we have here
//  (e.g., by setting up a mapping from resourceName->resource which can be modified at runtime).
// Since our lab is simple, we'll just hardcode it:
struct ResourceManager {
    std::unique_ptr<sf::SoundBuffer> jumpSoundBuffer;
    std::unique_ptr<sf::Sound> jumpSound;
};

struct TubePair {
    sf::RectangleShape topTube;
    sf::RectangleShape bottomTube;
    float x;

    TubePair(float xPos, float gapY, float gapHeight)
        : x(xPos)
    {
        // Top tube
        topTube.setPosition({ xPos, 0 });
        topTube.setSize({ TUBE_WIDTH, gapY });
        topTube.setFillColor(sf::Color::Green);

        // Bottom tube
        float bottomTubeHeight = WINDOW_HEIGHT - (gapY + gapHeight);
        bottomTube.setPosition({ xPos, gapY + gapHeight });
        bottomTube.setSize({ TUBE_WIDTH, bottomTubeHeight });
        bottomTube.setFillColor(sf::Color::Green);
    }

    void update(float speed)
    {
        x -= speed;
        topTube.setPosition({ x, topTube.getPosition().y });
        bottomTube.setPosition({ x, bottomTube.getPosition().y });
    }

    bool isOffScreen() const { return x + topTube.getSize().x < 0; }
};

bool isTubeOffScreen(const TubePair& tube) { return tube.isOffScreen(); }

struct BirdState {
    BirdState()
        : velocityY { INITIAL_BIRD_VELOCITY_Y }
        , circle { BIRD_RADIUS }
        , pos { INITIAL_BIRD_POS }
    {
        // ====== ====== ======
        // TODO: (Q1)
        //  - initialize the bird's shape (see below) to have
        //    appropriate size, color, and initial position.
        //  Note: consider using member initializer list to set the radius via ctor call.
        // ====== ====== ======
        circle.setFillColor(BIRD_COLOR);

        // Set coordinates to be relative to the centre
        circle.setOrigin({ circle.getRadius(), circle.getRadius() });

        circle.setPosition(pos);
    }

    void jump()
    {
        velocityY = JUMP_SPEED - GRAVITY; // GRAVITY gets added later, so we cancel it out here
    }

    void updatePosition()
    {
        pos.y += velocityY;
        circle.setPosition(pos);
    }

    void reset()
    {
        velocityY = 0;
        pos = INITIAL_BIRD_POS;
        circle.setPosition(pos);
    }

    // ====== ====== ======
    // TODO: (Q1)
    //  - add a field for the bird's shape.
    // ====== ====== ======
    float velocityY;
    sf::CircleShape circle;
    sf::Vector2f pos;
};

struct GameState {
    GameState()
        : rng(0)
        , gapDistribution(100.0f, 500.0f)
    {
        // Note: bird is automatically initialized by BirdState's default ctor,
        // which you implemented above.
        generateInitialTubes();
    }

    void updateState()
    {
        applyPhysicsToBird();
        updateTubes();
        checkCollisions();
    }

private:
    void generateInitialTubes()
    {
        // Generate tubes starting from the right edge
        for (float x = WINDOW_WIDTH + 100.f; x < WINDOW_WIDTH + 1200.f; x += TUBE_SPACING) {
            float gapY = gapDistribution(rng);
            tubes.emplace_back(x, gapY, GAP_HEIGHT);
        }
    }

    void reset()
    {
        resetTubes();
        bird.reset();
    }

    void resetTubes()
    {
        tubes.clear();
        generateInitialTubes();
    }

    void applyPhysicsToBird()
    {
        // Apply gravity to bird
        bird.velocityY += GRAVITY;

        // ====== ====== ======
        // TODO: (Q3)
        //  - Update bird position according to the rule that bird's y-position
        //    should have bird's y-velocity added to it every frame (assume dt = 1).
        //    Should be equivalent to: bird.positionY += bird.velocityY;
        //  - Note: bird's x-coordinate will alway be exactly 100.f
        // ====== ====== ======
        bird.updatePosition();

        // ====== ====== ======
        // TODO: (Q3)
        //  - Check if the bird has exceeded the bounds of the screen
        //    (i.e., if it's no longer visible). If not, game should reset by clearing
        //    the tubes and restarting the game (setting the bird back to original initial position)
        // ====== ====== ======
        if (bird.pos.y + BIRD_RADIUS < 0 || bird.pos.y - BIRD_RADIUS > WINDOW_HEIGHT) {
            reset();
        }
    }

    void updateTubes()
    {
        // Update tube positions
        for (auto& tube : tubes) {
            tube.update(TUBE_SPEED);
        }

        // Remove tubes that are off screen
        tubes.erase(std::remove_if(tubes.begin(), tubes.end(), isTubeOffScreen), tubes.end());

        // Generate new tubes when needed
        if (!tubes.empty()) {
            float rightmostTubeX = tubes.back().x;
            if (rightmostTubeX < WINDOW_WIDTH + TUBE_SPACING) {
                float gapY = gapDistribution(rng);
                tubes.emplace_back(rightmostTubeX + TUBE_SPACING, gapY, GAP_HEIGHT);
            }
        }
    }

    void checkCollisions()
    {
        // ====== ====== ======
        // TODO: (Q4)
        //  Check for bird-tube collision using SFML Rect methods: getGlobalBounds() and
        //  findIntersection(). getGlobalBounds() will return a bounding box for the bird
        //  findIntersection() returns an sf::optional<sf::Rect>  (which can be
        //  implicitly converted to a boolean value) depending on whether a rectangle intersects
        //  with another
        // ====== ====== ======

        // ====== ====== ======
        // TODO: (Q4)
        //  If bird hits tube, game should reset by resetting the tubes and resetting the bird
        //  to its initial state (i.e., restarting the game)
        // ====== ====== ======

        const sf::FloatRect birdAABB { bird.circle.getGlobalBounds() };

        for (const TubePair& tube : tubes) {
            const sf::FloatRect tubeTopAABB { tube.topTube.getGlobalBounds() };
            const sf::FloatRect tubeBottomAABB { tube.bottomTube.getGlobalBounds() };

            if (birdAABB.findIntersection(tubeTopAABB)
                || birdAABB.findIntersection(tubeBottomAABB)) {
                reset();
            }
        }
    }

public:
    // game world objects
    std::vector<TubePair> tubes;
    BirdState bird;
    // rng state
    std::mt19937 rng;
    std::uniform_real_distribution<float> gapDistribution;
};

void handleInput(
    sf::Window& window, GameState& gameState, const ResourceManager& resources, bool& shouldQuit)
{
    while (const std::optional<sf::Event> event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) {
            window.close();
            shouldQuit = true;
        }

        // ====== ====== ======
        // TODO: (Q2)
        //  implement jump logic (the key press should be space) and play jump sound fx
        // ====== ====== ======
        if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()) {
            // We know a key was pressed, lets check if it was the space key!
            if (keyPressed->scancode == sf::Keyboard::Scan::Space) {
                gameState.bird.jump();
                resources.jumpSound->play();
            }
        }
    }
}

void render(sf::RenderWindow& window, const GameState& gameState)
{
    // Clear with blue background (sky)
    window.clear(sf::Color::Blue);
    // Draw tubes
    for (const auto& tube : gameState.tubes) {
        window.draw(tube.topTube);
        window.draw(tube.bottomTube);
    }
    // ====== ====== ======
    // TODO: (Q1) Draw bird
    // ====== ====== ======
    window.draw(gameState.bird.circle);

    window.display();
}

int main()
{
    sf::RenderWindow window;
    ResourceManager resources;
    GameState gameState; // game state initialization happens with ctor call

    // Prevent key repeats.
    window.setKeyRepeatEnabled(false);
    try {
        // Initialize window
        window.create(sf::VideoMode({ WINDOW_WIDTH, WINDOW_HEIGHT }), "Flappy Bird");
        window.setFramerateLimit(FPS_LIMIT);

        // ====== ====== ======
        // TODO: (Q2)
        //  - load jump sound into resources.jumpSoundBuffer
        //      - if fails, print to stderr: "Warning: Could not load jump.wav"
        //  - initialize an sf::Sound from resources.jumpSoundBuffer in resources.jumpSound
        // ====== ====== ======
        //  Note that a std::unique_ptr<T> is just a holder of a T* that automatically calls
        //  delete for you on the T* when it goes out of scope.
        //      e.g., std::unique_ptr<int> intPtr; // holds nullptr
        //            if (intPtr) {
        //                // Only runs when intPtr.get() != nullptr
        //            }
        //            intPtr.reset(new int(5)); // how to assign new ptr to a unique_ptr
        //            std::cout << "value is " << *intPtr << '\n';
        //            std::cout << "raw address is " << intPtr.get() << '\n';
        // ====== ====== ======
        resources.jumpSoundBuffer.reset(new sf::SoundBuffer);
        if (!resources.jumpSoundBuffer->loadFromFile("../assets/jump.wav")) {
            // failed to load sound file, handle here:
            std::cerr << "Warning: Could not load jump.wav\n";
        }
        resources.jumpSound.reset(new sf::Sound { *resources.jumpSoundBuffer });

        bool shouldQuit = false;
        // Main game loop
        while (window.isOpen()) {
            handleInput(window, gameState, resources, shouldQuit);
            if (shouldQuit)
                break;
            gameState.updateState();
            render(window, gameState);
        }

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return -1;
    }

    return 0;
}
