#include <iostream>
#include <Windows.h>
#include <fstream>

struct BankAccount
{
    int accountNumber;
    int balance;
};

void PrintMenu()
{
	std::cout << "\nМеню:\n";
	std::cout << "1. Просмотр текущего баланса банковского счета.\n";
	std::cout << "2. Пополнение счета на указанную сумму.\n";
	std::cout << "3. Снятие денежных средств.\n";
	std::cout << "4. Автоматическое выполнение 20 случайных операций.\n";
	std::cout << "5. Выход\n";
}

int main()
{
	srand(time(NULL));
    setlocale(LC_ALL, "Russian");

	std::ifstream file("account.bin", std::ios::binary);

	BankAccount account;

	if (!file.is_open())
	{
		account.accountNumber = 10001;
		account.balance = 10000;
	}
	else
	{
		file.read(reinterpret_cast<char*>(&account), sizeof(account));
	}

	bool exit = false;

	while (exit == false)
	{
		PrintMenu();

		std::cout << "Выберите пункт меню: ";
		int choice;
		std::cin >> choice;

		switch (choice)
		{
		case 1:
			std::cout << "Текущий баланс банковского счета: " << account.balance << std::endl;
			break;
		case 2:
			std::cout << "Введите сумму пополнения: ";
			int amount;
			std::cin >> amount;
			if (amount <= 0)
			{
				std::cout << "Сумма пополнения должна быть > 0\n";
			}
			else
			{
				account.balance += amount;				
			}
			break;
		case 3:
			std::cout << "Введите сумму, которую хотите снять: ";
			int amount;
			std::cin >> amount;
			if (account.balance - amount < 0)
			{
				std::cout << "Недостаточно денежных средств\n";
			}
			else
			{
				account.balance -= amount;
			}
			break;
		case 4:
			for (int i = 0; i < 20; i++)
			{
				int operation = rand() % 2;
				if (operation == 0)
				{
					int amount = 100 + rand() % 1001;
					account.balance += amount;
				}
				else if (operation == 1)
				{
					int amount = 100 + rand() % 1001;
					if (account.balance - amount >= 0)
					{
						account.balance -= amount;
					}
				}
				Sleep(100 + rand() % 501);
			}
			break;
		case 5:
			exit = true;
			break;
		default:
			std::cout << "Такого пунтка меню нет.\n";
			break;
		}
	}

	return 0;
}
