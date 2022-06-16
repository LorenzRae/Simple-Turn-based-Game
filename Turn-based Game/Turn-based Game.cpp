#include <iostream>
#include <string>
#include <array>
#include <random>//for namespace Random
#include <stdlib.h>//for system("CLS")
#include <cstdlib>//for rand()
#include <ctime>//for time()


class Creature
{
protected:
	std::string m_name{};
	char m_symbol{};
	int m_health{};
	int m_attack{};
	int m_gold{};
public:
	//const here
	Creature(const std::string& name, char symbol, int health, int attack, int gold)
		: m_name{ name }, m_symbol{ symbol }, m_health{ health }, m_attack{ attack }, m_gold{ gold }
	{}
	//getters
	const std::string& getName() const { return m_name; }
	char getSymbol() const { return m_symbol; }
	int getHealth() const { return m_health; }
	int getAttack() const { return m_attack; }
	int getGold() const { return m_gold; }

	void reduceHealth(int amount) { m_health -= amount; }
	bool isDead() const { return m_health <= 0; }
	void addGold(int amount) { m_gold += amount; }
};

class Player : public Creature
{
private:
	int m_level{ 1 };
public:
	//const here
	Player(const std::string& name = " ") :
		//name, symbol, hp, attack, gold
		Creature{ name, '@', 10, 1, 0 }
	{}
	//getter
	int getLevel() const { return m_level; }
	bool hasWon() const { return m_level >= 20; }

	void levelUp()
	{
		++m_level;
		++m_attack;
		++m_health;
	}
};

class Monster : public Creature
{
public://monster types
	enum class Type
	{
		dragon,
		orc,
		slime,
		max_type
	};

private: //lookup table
	//example: getDefaultCreater(Type::dragon).getName() returns "dragon" etc.
	static const Creature& getDefaultCreature(Type type)
	{
		static const std::array<Creature, static_cast<std::size_t>(Type::max_type)> monsterData{
		  { { "Dragon", 'D', 20, 4, 100 },
			{ "Orc", 'o', 4, 2, 25 },
			{ "Slime", 's', 1, 1, 10 } }
		};

		return monsterData.at(static_cast<std::size_t>(type));
	}

public:
	//const here
	Monster(Type type) :
		Creature{ getDefaultCreature(type) }
	{

	}

	static Monster getRandomMonster();
};

namespace Random
{
	std::random_device rd{};
	std::seed_seq ss{ rd(), rd(), rd(), rd(), rd(), rd(), rd(), rd() };
	std::mt19937 mt{ ss };
	std::uniform_int_distribution<> num{ static_cast<int>(Monster::Type::dragon), static_cast<int>(Monster::Type::max_type) - 1 };

	int getRandMons() { return num(mt); }
}

Monster Monster::getRandomMonster()
{
	return Monster{ static_cast<Type>(Random::getRandMons()) };
}

void creatureStats(Monster& m, Player& p)
{
	std::cout << "---------------------\n";
	//monster stats
	std::cout << '\t' << '\t' << m.getName() << " HP:" << m.getHealth() << '\n';
	//your stats
	std::cout << "You Lv:" << p.getLevel() << '\n';
	std::cout << "    HP:" << p.getHealth() << '\n';
	std::cout << "---------------------\n" << '\n';
}

void monsterAttack(Player& p1, Monster& m)
{
	std::cout << "The " << m.getName() << " hit you for " << m.getAttack() << " damage!\n";
	p1.reduceHealth(m.getAttack());
}

void playerAttack(Player& p1, Monster& m)
{
	std::cout << "You hit the " << m.getName() << " for " << p1.getAttack() << " damage!\n";
	m.reduceHealth(p1.getAttack());
}

void enterPress()
{
	if (std::cin.get() == '\n')
	{
		std::cin.ignore();
		std::system("CLS");
	}
}

//Fight == true
bool fightMonster(Player& p1, Monster& m)
{
	std::cout << "=====================\n";
	//fight or run
	while (true)
	{
		//show fight stat UI
		creatureStats(m, p1);
		std::cout << "(F)ight  /  (R)un  : ";
		char x{};
		std::cin >> x;
		//rand == true ; escaped succesfully
		bool rand{ std::rand() < (RAND_MAX / 3) };
		switch (x)
		{
		case 'F':	return true;
		case 'R':
		{
			if (rand)
			{
				std::cout << "Run away safely!\n" << '\n';
				return false;
			}
			else
			{
				std::cout << "Can't escape!\n";
				monsterAttack(p1, m);
				if (p1.isDead())
					return false;
				std::cout << '\n';
				break;
			}
		}
		default:
		{
			std::cout << "Invalid selection!\n";
			break;
		}
		}
	}
}


int main()
{
	std::cout << "=====================\n";
	std::cout << "Enter your Name : ";
	std::string name{};
	std::cin >> name;
	Player p1{ name };
	std::cout << "Welcome, " << p1.getName() << '\n';
	std::cout << "=====================\n";

	//seed rand()
	std::srand(static_cast<unsigned int>(std::time(nullptr)));

	while (true)
	{
		//create a Monster instance
		Monster m{ Monster::getRandomMonster() };
		//Monster encountered
		std::cout << "=====================\n";
		std::cout << "You encountered a " << m.getName() << '\n';

		//do the loop if player choose to fight
		//break if monster/player was killed
		//or if player run
		for (bool i{ fightMonster(p1, m) }; i == true; i = fightMonster(p1, m))
		{
			//fight happens; 
			//player turn 
			playerAttack(p1, m);
			//monster killed
			if (m.isDead())
			{
				std::cout << "You killed the " << m.getName() << ".\n";
				std::cout << "Obtained " << m.getGold() << " gold!\n";
				p1.addGold(m.getGold());
				p1.levelUp();
				std::cout << "You leveled up to lv." << p1.getLevel() << "!\n" << '\n';
				break;
			}

			//monster turn
			monsterAttack(p1, m);
			//if monster killed you
			if (p1.isDead())
				//break the loop
				break;
		}

		if (p1.isDead())
		{
			std::cout << '\n';
			std::cout << "You died at level " << p1.getLevel() << " with " << p1.getGold() << " gold!\n";
			break;
		}

		if (p1.hasWon())
		{
			std::cout << "Congratulations!\n";
			std::cout << "You beat the game!\n";
		}

		enterPress();
		//break;
	}

	return 0;
}