#include "GameManager.h"
#include <cstdlib>
#include <ctime>
#include <iostream>

using namespace std;

GameManager::GameManager()
    : window(sf::VideoMode(1280, 720), "Turn-Based Strategy Game"),
      currentState(GameState::MainMenu),
      selectedMenuOption(0),
      selectedBattleAction(0),
      currentEnemy(nullptr),
      currentDoorIndex(-1) {
  // Seed random number generator
  srand(static_cast<unsigned int>(time(nullptr)));

  // Configure window
  window.setFramerateLimit(60);

  // Set up menu options
  menuOptions = {"Play Game", "Exit"};

  // Load resources
  loadResources();

  // Create player
  player = new Player("Hero", 100);

  // Create and initialize map manager
  mapManager = new MapManager(this, &window, &font);
  mapManager->initialize();
}

GameManager::~GameManager() {
  // Clean up allocated objects
  delete mapManager;
  delete player;

  if (currentEnemy) {
    delete currentEnemy;
    currentEnemy = nullptr;
  }
}

void GameManager::loadResources() {
  if (!font.loadFromFile(
          "/home/nguyenvu343/sfml/Image and fonts/Roboto-Black.ttf")) {
    cerr << "Failed to load font from backup path!" << std::endl;
    throw std::runtime_error("Font loading failed");
  }

  if (!playerBattleTexture.loadFromFile("/home/nguyenvu343/sfml/Image and "
                                        "fonts/0_Fallen_Angels_Idle_008.png")) {
    cerr << "Failed to load player battle texture" << std::endl;
  } else {
    cout << "Successfully loaded player battle texture" << std::endl;
  }

  // Load enemy battle texture - using one texture for all enemies
  if (!enemyBattleTexture.loadFromFile(
          "Image and fonts/0_Dark_Oracle_Idle_012.png")) {
    cerr << "Failed to load enemy battle texture" << std::endl;
  } else {
    cout << "Successfully loaded enemy battle texture" << std::endl;
  }
}

void GameManager::run() {
  sf::Clock clock;

  // Main game loop
  while (window.isOpen()) {
    float deltaTime = clock.restart().asSeconds();

    processEvents();
    update(deltaTime);
    render();
  }
}

void GameManager::processEvents() {
  sf::Event event;
  while (window.pollEvent(event)) {
    if (event.type == sf::Event::Closed) {
      window.close();
    }

    if (event.type == sf::Event::KeyPressed) {
      // Handle input based on current game state
      switch (currentState) {
        case GameState::MainMenu:
          handleMainMenuInput(event);
          break;

        case GameState::HubWorld:
          handleHubWorldInput(event);
          break;

        case GameState::BeforeFight:
          handleBeforeFightInput(event);
          break;

        case GameState::Battle:
          battleSystem.handleInput(event);
          break;

        case GameState::Inventory:
          handleInventoryInput(event);
          break;

        default:
          break;
      }
    }
  }

  if (currentState == GameState::HubWorld) {
    const float moveSpeed = 5.0f;  // Movement in pixels per frame

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) {
      mapManager->movePlayer(0, -moveSpeed);
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) {
      mapManager->movePlayer(0, moveSpeed);
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
      mapManager->movePlayer(-moveSpeed, 0);
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
      mapManager->movePlayer(moveSpeed, 0);
    }
  }
}

void GameManager::handleMainMenuInput(const sf::Event& event) {
  if (event.key.code == sf::Keyboard::Up) {
    if (selectedMenuOption > 0) {
      selectedMenuOption--;
      cout << "Selected: " << menuOptions[selectedMenuOption] << std::endl;
    }
  } else if (event.key.code == sf::Keyboard::Down) {
    if (selectedMenuOption < static_cast<int>(menuOptions.size() - 1)) {
      selectedMenuOption++;
      cout << "Selected: " << menuOptions[selectedMenuOption] << std::endl;
    }
  } else if (event.key.code == sf::Keyboard::Return) {
    if (selectedMenuOption == 0) {
      // Play Game
      cout << "Play selected!" << std::endl;
      startTransition(GameState::HubWorld);
    } else {
      // Exit
      cout << "Exit selected!" << std::endl;
      window.close();
    }
  }
}

void GameManager::handleHubWorldInput(const sf::Event& event) {
  if (event.key.code == sf::Keyboard::Escape) {
    cout << "Returning to main menu..." << std::endl;
    startTransition(GameState::MainMenu);
  } else if (event.key.code == sf::Keyboard::I) {
    cout << "Opening inventory..." << std::endl;
    startTransition(GameState::Inventory);
  } else if (event.key.code == sf::Keyboard::Space) {
    // Check if player is near a door
    if (mapManager->isPlayerNearDoor()) {
      std::cout << "Entering gate..." << std::endl;
      createEnemyForDoor(currentDoorIndex);
      startTransition(GameState::BeforeFight);
    }
    // Check if player is near a heal orb
    else if (mapManager->isPlayerNearHealOrb()) {
      cout << "Found a healing orb!" << std::endl;

      // Heal the player
      int healAmount =
          player->getMaxHealth() / 4;   // Heal for 25% of max health
      player->takeDamage(-healAmount);  // Negative damage for healing
      std::cout << "Healed for " << healAmount << " health!" << std::endl;
      if (player->getHealth() > player->getMaxHealth()) {
        player->setHealth(player->getMaxHealth());  // Cap at max health
      }

      // Remove the orb
      mapManager->removeHealOrb();
    }
  }
}

void GameManager::handleBeforeFightInput(const sf::Event& event) {
  // In the demo version, any key press returns to hub world
  if (event.key.code == sf::Keyboard::F) {
    startBattle(currentDoorIndex);
    startTransition(GameState::Battle);
    cout << "Starting battle..." << std::endl;
    // Reset the selected battle action
    selectedBattleAction = 0;
    startBattle(currentDoorIndex);

    // Print enemy stats for debugging
    cout << "Enemy HP: " << currentEnemy->getHealth() << "/"
              << currentEnemy->getMaxHealth() << std::endl;
    cout << "Enemy Damage: " << currentEnemy->getBaseDamage() << std::endl;

  } else if (event.key.code == sf::Keyboard::E ||
             event.key.code == sf::Keyboard::Escape) {
    cout << "Retreating from battle..." << std::endl;
    startTransition(GameState::HubWorld);
  }
}

void GameManager::handleInventoryInput(const sf::Event& event) {
  if (event.key.code == sf::Keyboard::Escape ||
      event.key.code == sf::Keyboard::I) {
    cout << "Closing inventory..." << std::endl;
    startTransition(GameState::HubWorld);
  }
}

void GameManager::update(float deltaTime) {
  // Handle battle logic
  if (currentState == GameState::Battle) {
    // Check if battle is over
    if (battleSystem.isBattleOver()) {
      if (battleSystem.isPlayerVictorious()) {
        // Player won the battle
        cout << "Enemy defeated!" << std::endl;

        // Award experience
        Enemy* enemy = battleSystem.getEnemy();
        if (enemy) {
          int expGained = enemy->getRewardExp();
          player->gainExp(expGained);
          cout << "Gained " << expGained << " experience points!"
                    << std::endl;

          // Check for level up
          player->checkLevelUp();
        }

        // End battle with victory and return to hub world
        endBattle(true);
        startTransition(GameState::HubWorld);
        return;
      } else {
        // Player was defeated
        cout << "Player defeated!" << std::endl;

        // Restore player's health
        player->takeDamage(-player->getMaxHealth());  // Full heal

        // End battle and return to hub world
        endBattle(false);
        startTransition(GameState::HubWorld);
        return;
      }
    }

    // Run the battle system turn if it's not the player's turn
    if (!battleSystem.isPlayerTurn()) {
      battleSystem.runTurn();
    }
  }
}

void GameManager::render() {
  // Clear with appropriate background color
  if (currentState == GameState::MainMenu) {
    window.clear(sf::Color(33, 33, 33));  // Dark gray for main menu
  } else if (currentState == GameState::BeforeFight) {
    window.clear(sf::Color(20, 30, 50));  // Dark blue for before fight screen
  } else if (currentState == GameState::Battle) {
    window.clear(sf::Color(40, 20, 60));  // Purple for battle screen
  } else {
    window.clear(sf::Color(20, 20, 30));  // Dark blue for hub world
  }

  // Render based on current state
  switch (currentState) {
    case GameState::MainMenu:
      drawMainMenu();
      break;

    case GameState::HubWorld:
      mapManager->drawMap();                // Draw the hub world
      mapManager->drawInteractionPrompt();  // Draw interaction prompts
      break;

    case GameState::BeforeFight:
      drawBeforeFightScreen();  // Draw the enemy stats screen
      break;

    case GameState::Inventory:
      drawInventoryScreen();
      break;

    case GameState::Battle:
      drawBattleScreen();  // Draw the battle screen
      break;
    default:
      break;
  }

  // Display the state at the bottom of the screen for debugging
  sf::Text stateText;
  stateText.setFont(font);
  stateText.setCharacterSize(14);
  stateText.setFillColor(sf::Color::Yellow);

  std::string stateName;
  switch (currentState) {
    case GameState::MainMenu:
      stateName = "MAIN MENU";
      break;
    case GameState::HubWorld:
      stateName = "HUB WORLD";
      break;
    case GameState::BeforeFight:
      stateName = "BEFORE FIGHT";
      break;
    case GameState::Battle:
      stateName = "BATTLE";
      break;
    case GameState::Inventory:
      stateName = "INVENTORY";
      break;
    case GameState::GameOver:
      stateName = "GAME OVER";
      break;
    default:
      stateName = "UNKNOWN";
      break;
  }

  stateText.setString("CURRENT STATE: " + stateName);
  stateText.setPosition(
      window.getSize().x - stateText.getGlobalBounds().width - 10,
      window.getSize().y - 20);
  window.draw(stateText);

  window.display();
}

void GameManager::drawMainMenu() {
  // Title
  sf::Text titleText("TURN-BASED STRATEGY GAME", font, 60);
  titleText.setStyle(sf::Text::Bold);
  titleText.setFillColor(sf::Color::White);
  titleText.setPosition(
      (window.getSize().x - titleText.getGlobalBounds().width) / 2, 100);
  window.draw(titleText);

  // Menu option: Play Game
  sf::Text playText(menuOptions[0], font, 30);
  playText.setFillColor(selectedMenuOption == 0 ? sf::Color(255, 165, 0)
                                                : sf::Color::White);
  playText.setPosition(
      (window.getSize().x - playText.getGlobalBounds().width) / 2, 300);

  // Menu option: Exit
  sf::Text exitText(menuOptions[1], font, 30);
  exitText.setFillColor(selectedMenuOption == 1 ? sf::Color(255, 165, 0)
                                                : sf::Color::White);
  exitText.setPosition(
      (window.getSize().x - exitText.getGlobalBounds().width) / 2, 380);

  // Button backgrounds
  sf::RectangleShape playBg(
      sf::Vector2f(playText.getGlobalBounds().width + 40, 50));
  playBg.setFillColor(sf::Color(70, 70, 70, 200));
  playBg.setPosition(playText.getPosition().x - 20,
                     playText.getPosition().y - 10);

  sf::RectangleShape exitBg(
      sf::Vector2f(exitText.getGlobalBounds().width + 40, 50));
  exitBg.setFillColor(sf::Color(70, 70, 70, 200));
  exitBg.setPosition(exitText.getPosition().x - 20,
                     exitText.getPosition().y - 10);

  // Draw button backgrounds first
  window.draw(playBg);
  window.draw(exitBg);

  // Then draw text
  window.draw(playText);
  window.draw(exitText);

  // Navigation instructions
  sf::Text instructionsText("Use Arrow Keys to Navigate", font, 20);
  instructionsText.setFillColor(sf::Color(200, 200, 200));
  instructionsText.setPosition(
      (window.getSize().x - instructionsText.getGlobalBounds().width) / 2, 600);
  window.draw(instructionsText);

  sf::Text enterText("Press Enter to Select", font, 20);
  enterText.setFillColor(sf::Color(200, 200, 200));
  enterText.setPosition(
      (window.getSize().x - enterText.getGlobalBounds().width) / 2, 630);
  window.draw(enterText);
}

void GameManager::createEnemyForDoor(int doorIndex) {
  // Clean up previous enemy if it exists
  if (currentEnemy != nullptr) {
    delete currentEnemy;
    currentEnemy = nullptr;
  }

  // Create enemy based on the door
  std::string enemyName;
  Difficulty diff;
  AbilityType ability = AbilityType::NormalAttack;
  ResistanceType resistance = ResistanceType::None;

  switch (doorIndex) {
    case 0:  // First door
      enemyName = "Goblin";
      diff = Difficulty::Easy;
      break;
    case 1:  // Second door
      enemyName = "Goblin";
      diff = Difficulty::Medium;
      ability = AbilityType::Poison;
      break;
    case 2:  // Third door
      enemyName = "Goblin";
      diff = Difficulty::Hard;
      ability = AbilityType::Stun;
      resistance = ResistanceType::Physical;
      break;
    default:
      enemyName = "Unknown Enemy";
      diff = Difficulty::Easy;
      ability = AbilityType::NormalAttack;
      resistance = ResistanceType::None;
  }

  // Create the new enemy object
  currentEnemy = new Enemy(enemyName, diff, ability, resistance, "Fire Blast",
                           20 + (doorIndex * 10));

  // Store the current door index
  currentDoorIndex = doorIndex;
}

void GameManager::drawBeforeFightScreen() {
  // Draw a dark background
  sf::RectangleShape background(
      sf::Vector2f(window.getSize().x, window.getSize().y));
  background.setFillColor(
      sf::Color(20, 30, 50, 255));  // Dark blue for before fight screen
  window.draw(background);

  // Draw title
  sf::Text titleText("Enemy Encountered!", font, 40);
  titleText.setStyle(sf::Text::Bold);
  titleText.setFillColor(sf::Color::White);
  titleText.setPosition(
      (window.getSize().x - titleText.getGlobalBounds().width) / 2, 80);
  window.draw(titleText);

  // Create a placeholder enemy if we don't have one yet
  if (!currentEnemy) {
    // For demo, just create a simple enemy
    currentEnemy =
        new Enemy("Goblin", Difficulty::Easy, AbilityType::NormalAttack,
                  ResistanceType::None, "Fireball", 10);
    currentDoorIndex = 0;  // Default to first door
  }

  // Draw enemy stats in a box
  sf::RectangleShape statsBox(sf::Vector2f(500, 300));
  statsBox.setFillColor(sf::Color(50, 50, 80, 230));
  statsBox.setOutlineColor(sf::Color::White);
  statsBox.setOutlineThickness(2);
  statsBox.setPosition((window.getSize().x - 500) / 2, 150);
  window.draw(statsBox);

  // Draw enemy image
  sf::Sprite enemySprite(enemyBattleTexture);

  // Scale enemy sprite to fit nicely in the box
  float enemyScale = 100.0f / std::max(enemyBattleTexture.getSize().x,
                                       enemyBattleTexture.getSize().y);
  enemySprite.setScale(enemyScale, enemyScale);

  // Position the sprite in the top portion of the box
  enemySprite.setPosition(
      statsBox.getPosition().x +
          (statsBox.getSize().x - enemySprite.getGlobalBounds().width) / 2,
      statsBox.getPosition().y + 30);

  window.draw(enemySprite);

  // Enemy difficulty color
  sf::Color difficultyColor;
  string difficultyText;

  switch (currentEnemy->getDifficulty()) {
    case Difficulty::Easy:
      difficultyColor = sf::Color::Green;
      difficultyText = "Easy";
      break;
    case Difficulty::Medium:
      difficultyColor = sf::Color::Yellow;
      difficultyText = "Medium";
      break;
    case Difficulty::Hard:
      difficultyColor = sf::Color::Red;
      difficultyText = "Hard";
      break;
    default:
      difficultyColor = sf::Color::White;
      difficultyText = "Unknown";
      break;
  }

  // Draw enemy info
  string enemyInfo = "Enemy: " + currentEnemy->getName() + "\n";
  enemyInfo += "Difficulty: " + difficultyText + "\n";
  enemyInfo += "HP: " + std::to_string(currentEnemy->getHealth()) + "/" +
               std::to_string(currentEnemy->getMaxHealth()) + "\n";
  enemyInfo +=
      "Damage: " + std::to_string(currentEnemy->getBaseDamage()) + "\n";

  // Convert resistance type to string
  string resistanceText;
  switch (currentEnemy->getResistance()) {
    case ResistanceType::Physical:
      resistanceText = "Physical";
      break;
    case ResistanceType::Magical:
      resistanceText = "Magical";
      break;
    case ResistanceType::None:
      resistanceText = "None";
      break;
    default:
      resistanceText = "Unknown";
      break;
  }
  enemyInfo += "Resistance: " + resistanceText + "\n";


  sf::Text demoInfo(enemyInfo, font, 22);
  demoInfo.setFillColor(sf::Color::White);
  demoInfo.setPosition(
      statsBox.getPosition().x +
          (statsBox.getSize().x - demoInfo.getGlobalBounds().width) / 2,
      statsBox.getPosition().y + 150);
  window.draw(demoInfo);

  // Draw choice buttons
  sf::RectangleShape fightButton(sf::Vector2f(150, 60));
  fightButton.setFillColor(sf::Color(200, 50, 50, 200));
  fightButton.setPosition((window.getSize().x / 2) - 170, 500);
  window.draw(fightButton);

  sf::Text fightText("FIGHT", font, 30);
  fightText.setFillColor(sf::Color::White);
  fightText.setPosition(
      fightButton.getPosition().x +
          (fightButton.getSize().x - fightText.getGlobalBounds().width) / 2,
      fightButton.getPosition().y +
          (fightButton.getSize().y - fightText.getGlobalBounds().height) / 2 -
          5);
  window.draw(fightText);

  sf::RectangleShape fleeButton(sf::Vector2f(150, 60));
  fleeButton.setFillColor(sf::Color(50, 50, 200, 200));
  fleeButton.setPosition((window.getSize().x / 2) + 20, 500);
  window.draw(fleeButton);

  sf::Text fleeText("Exit", font, 30);
  fleeText.setFillColor(sf::Color::White);
  fleeText.setPosition(
      fleeButton.getPosition().x +
          (fleeButton.getSize().x - fleeText.getGlobalBounds().width) / 2,
      fleeButton.getPosition().y +
          (fleeButton.getSize().y - fleeText.getGlobalBounds().height) / 2 - 5);
  window.draw(fleeText);

  // Draw instructions
  sf::Text instructions("Press F to FIGHT or R to RETREAT", font, 20);
  instructions.setFillColor(sf::Color(200, 200, 200));
  instructions.setPosition(
      (window.getSize().x - instructions.getGlobalBounds().width) / 2, 580);
  window.draw(instructions);
}

void GameManager::drawInventoryScreen() {
  // Draw inventory screen (to be implemented)
  window.clear(sf::Color(40, 40, 60));  // Dark bluish for inventory

  sf::Text inventoryTitle("INVENTORY", font, 40);
  inventoryTitle.setFillColor(sf::Color::White);
  inventoryTitle.setPosition(
      (window.getSize().x - inventoryTitle.getGlobalBounds().width) / 2, 50);
  window.draw(inventoryTitle);

  // Demo inventory content
  sf::Text demoText("DEMO VERSION", font, 30);
  demoText.setFillColor(sf::Color::Yellow);
  demoText.setPosition(
      (window.getSize().x - demoText.getGlobalBounds().width) / 2, 150);
  window.draw(demoText);

  sf::Text demoInfo("Inventory system will be implemented later", font, 22);
  demoInfo.setFillColor(sf::Color::White);
  demoInfo.setPosition(
      (window.getSize().x - demoInfo.getGlobalBounds().width) / 2, 250);
  window.draw(demoInfo);

  sf::Text instructions("Press ESC or I to return to game", font, 20);
  instructions.setFillColor(sf::Color::White);
  instructions.setPosition(
      (window.getSize().x - instructions.getGlobalBounds().width) / 2, 600);
  window.draw(instructions);
}

void GameManager::drawBattleScreen() {
  // Get window dimensions
  float windowWidth = window.getSize().x;
  float windowHeight = window.getSize().y;

  // Main background
  sf::RectangleShape background(sf::Vector2f(windowWidth, windowHeight));
  background.setFillColor(sf::Color(40, 20, 60));  // Dark purple for battle
  window.draw(background);

  // Battle title
  sf::Text battleTitle("BATTLE", font, 50);
  battleTitle.setStyle(sf::Text::Bold);
  battleTitle.setFillColor(sf::Color::White);
  battleTitle.setPosition(
      (windowWidth - battleTitle.getGlobalBounds().width) / 2,
      windowHeight * 0.05f  // 5% from top
  );
  window.draw(battleTitle);

  // Calculate box dimensions relative to screen size
  float boxWidth = windowWidth * 0.35f;   // 35% of screen width
  float boxHeight = windowHeight * 0.4f;  // 40% of screen height
  float leftBoxX = windowWidth * 0.05f;   // 5% from left
  float rightBoxX = windowWidth * 0.6f;   // 60% from left
  float boxY = windowHeight * 0.15f;      // 15% from top

  // Draw player box on the left
  sf::RectangleShape playerBox(sf::Vector2f(boxWidth, boxHeight));
  playerBox.setFillColor(sf::Color(50, 80, 100, 230));
  playerBox.setOutlineColor(sf::Color::White);
  playerBox.setOutlineThickness(2);
  playerBox.setPosition(leftBoxX, boxY);
  window.draw(playerBox);

  // Draw player image using texture
  sf::Sprite playerSprite(playerBattleTexture);

  // Scale player sprite to fit nicely in the box
  float playerScale = boxHeight * 0.5f /
                      std::max(playerBattleTexture.getSize().x,
                               playerBattleTexture.getSize().y);
  playerSprite.setScale(playerScale, playerScale);

  // Position the sprite in the left portion of the box
  playerSprite.setPosition(
      playerBox.getPosition().x + boxWidth * 0.1f,  // 10% from left edge of box
      playerBox.getPosition().y +
          (boxHeight - playerSprite.getGlobalBounds().height) /
              2  // Vertical center
  );

  window.draw(playerSprite);

  // Draw player stats in the right portion of the box
  string playerStats = player->getName() + "\n";
  playerStats += "HP: " + std::to_string(player->getHealth()) + "/" +
                 std::to_string(player->getMaxHealth()) + "\n";
  playerStats += "Exp: " + std::to_string(player->getExp()) + "/35\n";
  playerStats += "Level: " + std::to_string(player->getLevel());

  sf::Text playerText(playerStats, font, 24);
  playerText.setFillColor(sf::Color::White);
  playerText.setPosition(
      playerBox.getPosition().x + boxWidth * 0.5f,   // 50% of box width
      playerBox.getPosition().y + boxHeight * 0.25f  // 25% from top of box
  );
  window.draw(playerText);

  // Draw enemy box on the right
  sf::RectangleShape enemyBox(sf::Vector2f(boxWidth, boxHeight));
  enemyBox.setFillColor(sf::Color(100, 50, 80, 230));
  enemyBox.setOutlineColor(sf::Color::White);
  enemyBox.setOutlineThickness(2);
  enemyBox.setPosition(rightBoxX, boxY);
  window.draw(enemyBox);

  // Get enemy from battle system
  Enemy* battleEnemy = battleSystem.getEnemy();

  // Draw enemy stats and image if there is an enemy
  if (battleEnemy) {
    // Draw enemy image
    sf::Sprite enemySprite(enemyBattleTexture);

    // Scale enemy sprite to fit nicely in the box
    float enemyScale = boxHeight * 0.5f /
                       std::max(enemyBattleTexture.getSize().x,
                                enemyBattleTexture.getSize().y);
    enemySprite.setScale(enemyScale, enemyScale);

    // Position the sprite in the left portion of the box
    enemySprite.setPosition(
        enemyBox.getPosition().x +
            boxWidth * 0.1f,  // 10% from left edge of box
        enemyBox.getPosition().y +
            (boxHeight - enemySprite.getGlobalBounds().height) /
                2  // Vertical center
    );

    window.draw(enemySprite);

    // Draw enemy stats
    std::string enemyStats = battleEnemy->getName() + "\n";
    enemyStats += "HP: " + std::to_string(battleEnemy->getHealth()) + "/" +
                  std::to_string(battleEnemy->getMaxHealth()) + "\n";
    enemyStats +=
        "Damage: " + std::to_string(battleEnemy->getBaseDamage()) + "\n";

    // Get difficulty as string
    string difficultyText;
    switch (battleEnemy->getDifficulty()) {
      case Difficulty::Easy:
        difficultyText = "Easy";
        break;
      case Difficulty::Medium:
        difficultyText = "Medium";
        break;
      case Difficulty::Hard:
        difficultyText = "Hard";
        break;
      default:
        difficultyText = "Unknown";
        break;
    }
    enemyStats += "Difficulty: " + difficultyText;

    sf::Text enemyText(enemyStats, font, 24);
    enemyText.setFillColor(sf::Color::White);
    enemyText.setPosition(
        enemyBox.getPosition().x + boxWidth * 0.5f,   // 50% of box width
        enemyBox.getPosition().y + boxHeight * 0.25f  // 25% from top of box
    );
    window.draw(enemyText);
  }

  // Draw action menu at the bottom
  float actionBoxWidth = windowWidth * 0.5f;    // 50% of screen width
  float actionBoxHeight = windowHeight * 0.3f;  // 30% of screen height

  sf::RectangleShape actionBox(sf::Vector2f(actionBoxWidth, actionBoxHeight));
  actionBox.setFillColor(sf::Color(60, 60, 80, 230));
  actionBox.setOutlineColor(sf::Color::White);
  actionBox.setOutlineThickness(2);
  actionBox.setPosition(
      (windowWidth - actionBoxWidth) / 2,  // Center horizontally
      windowHeight * 0.6f                  // 60% from top
  );
  window.draw(actionBox);

  // Draw action options - using action names from BattleSystem
  const vector<std::string>& actionNames = battleSystem.getActionNames();
  float actionTextX = actionBox.getPosition().x +
                      actionBoxWidth * 0.1f;  // 10% from left of action box
  float actionTextY = actionBox.getPosition().y +
                      actionBoxHeight * 0.1f;     // 10% from top of action box
  float actionSpacing = actionBoxHeight * 0.15f;  // Spacing between actions

  for (int i = 0; i < static_cast<int>(actionNames.size()); i++) {
    sf::Text actionText(std::to_string(i + 1) + ". " + actionNames[i], font,
                        24);

    // Highlight selected action
    if (i == battleSystem.getSelectedAction()) {
      actionText.setFillColor(sf::Color::Yellow);

      // Draw selection indicator
      sf::CircleShape selector(8);
      selector.setFillColor(sf::Color::Yellow);
      selector.setPosition(actionTextX - 20,
                           actionTextY + i * actionSpacing + 8);
      window.draw(selector);
    } else {
      actionText.setFillColor(sf::Color::White);
    }

    actionText.setPosition(actionTextX, actionTextY + i * actionSpacing);
    window.draw(actionText);
  }

  // Display turn information
  std::string turnText = battleSystem.isPlayerTurn()
                             ? "Your turn! Select an action."
                             : "Enemy's turn...";
  sf::Text turnInfo(turnText, font, 20);
  turnInfo.setFillColor(sf::Color::White);
  turnInfo.setPosition((windowWidth - turnInfo.getGlobalBounds().width) / 2,
                       actionBox.getPosition().y - 30);
  window.draw(turnInfo);

  // Instructions
  sf::Text instructions("Use arrow keys to select action, Enter to confirm",
                        font, 20);
  instructions.setFillColor(sf::Color(200, 200, 200));
  instructions.setPosition(
      (windowWidth - instructions.getGlobalBounds().width) / 2,
      windowHeight - 40);
  window.draw(instructions);
}

void GameManager::startTransition(GameState newState) {
  cout << "Transitioning from ";
  // Print the current state
  switch (currentState) {
    case GameState::MainMenu:
      std::cout << "MAIN_MENU";
      break;
    case GameState::HubWorld:
      std::cout << "HUB_WORLD";
      break;
    case GameState::BeforeFight:
      std::cout << "BEFORE_FIGHT";
      break;
    case GameState::Battle:
      std::cout << "BATTLE";
      break;
    case GameState::Inventory:
      std::cout << "INVENTORY";
      break;
    case GameState::GameOver:
      std::cout << "GAME_OVER";
      break;
    default:
      std::cout << "UNKNOWN";
      break;
  }

  cout << " to ";

  // Print the new state
  switch (newState) {
    case GameState::MainMenu:
      cout << "MAIN_MENU";
      break;
    case GameState::HubWorld:
      cout << "HUB_WORLD";
      break;
    case GameState::BeforeFight:
      cout << "BEFORE_FIGHT";
      break;
    case GameState::Battle:
      cout << "BATTLE";
      break;
    case GameState::Inventory:
      cout << "INVENTORY";
      break;
    case GameState::GameOver:
      cout << "GAME_OVER";
      break;
    default:
      cout << "UNKNOWN";
      break;
  }

  cout << std::endl;

  // State-specific setup when transitioning
  if (newState == GameState::HubWorld && currentState == GameState::MainMenu) {
    // Reset player position when entering hub world from main menu
    const float tileSize = mapManager->getTileSize();
    const float gridOffsetX = mapManager->getGridOffsetX();
    const float gridOffsetY = mapManager->getGridOffsetY();

    float centerX = gridOffsetX + 5 * tileSize + tileSize / 4;
    float centerY = gridOffsetY + 5 * tileSize + tileSize / 4;

    mapManager->setPlayerPosition(centerX, centerY);
  }

  // Change state
  currentState = newState;
}

Player* GameManager::getPlayer() { return player; }

void GameManager::startBattle(int doorIndex) {
  cout << "Starting battle with door: " << doorIndex << std::endl;

  // Initialize battle system with player and enemy
  battleSystem.fight(*player, *currentEnemy);

  // Keep local tracking variable in sync
  selectedBattleAction = battleSystem.getSelectedAction();
}

void GameManager::endBattle(bool playerWon) {
  // Set player out of battle
  player->setInBattle(false);

  // Reset battle system
  battleSystem.reset();

  // Clean up the enemy
  if (currentEnemy) {
    delete currentEnemy;
    currentEnemy = nullptr;
  }
}