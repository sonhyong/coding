using System;

namespace TextRpgGame
{
    class Program
    {
        static void Main(string[] args)
        {
            Game game = new Game();
            game.Start();
        }
    }

    class Game
    {
        private Player player;
        private Enemy enemy;

        public void Start()
        {
            Console.WriteLine("Welcome to the Text RPG!");
            Console.Write("Enter your hero's name: ");
            string name = Console.ReadLine();
            player = new Player(name);

            Console.WriteLine($"Hello, {player.Name}! A wild monster appears!");
            enemy = new Enemy("Goblin", 30, 5);

            while (player.IsAlive && enemy.IsAlive)
            {
                PlayerTurn();
                if (enemy.IsAlive)
                {
                    EnemyTurn();
                }
            }

            if (player.IsAlive)
            {
                Console.WriteLine("\nYou defeated the monster! You win!");
            }
            else
            {
                Console.WriteLine("\nYou were defeated... Game Over.");
            }
        }

        private void PlayerTurn()
        {
            Console.WriteLine($"\n{player.Name}: {player.HP} HP  |  {enemy.Name}: {enemy.HP} HP");
            Console.WriteLine("Choose your action: (A)ttack (H)eal");
            Console.Write("> ");
            var key = Console.ReadKey();
            Console.WriteLine();
            if (key.Key == ConsoleKey.A)
            {
                int damage = player.Attack();
                enemy.TakeDamage(damage);
                Console.WriteLine($"You strike the {enemy.Name} for {damage} damage!");
            }
            else if (key.Key == ConsoleKey.H)
            {
                int heal = player.Heal();
                Console.WriteLine($"You heal yourself for {heal} HP!");
            }
            else
            {
                Console.WriteLine("Invalid action. You lose your turn.");
            }
        }

        private void EnemyTurn()
        {
            int damage = enemy.Attack();
            player.TakeDamage(damage);
            Console.WriteLine($"The {enemy.Name} strikes you for {damage} damage!");
        }
    }

    class Character
    {
        public string Name { get; protected set; }
        public int HP { get; protected set; }
        public int AttackPower { get; protected set; }

        public bool IsAlive => HP > 0;

        public Character(string name, int hp, int attackPower)
        {
            Name = name;
            HP = hp;
            AttackPower = attackPower;
        }

        public virtual int Attack()
        {
            return AttackPower;
        }

        public void TakeDamage(int damage)
        {
            HP -= damage;
            if (HP < 0)
                HP = 0;
        }
    }

    class Player : Character
    {
        private Random random;

        public Player(string name) : base(name, 50, 10)
        {
            random = new Random();
        }

        public override int Attack()
        {
            // Randomize attack damage slightly
            return base.Attack() + random.Next(-2, 3);
        }

        public int Heal()
        {
            int healAmount = random.Next(5, 16);
            HP += healAmount;
            if (HP > 50)
                HP = 50;
            return healAmount;
        }
    }

    class Enemy : Character
    {
        private Random random;
        public Enemy(string name, int hp, int attackPower) : base(name, hp, attackPower)
        {
            random = new Random();
        }

        public override int Attack()
        {
            return base.Attack() + random.Next(-1, 2);
        }
    }
}

