#include <iostream>
#include <Windows.h>
#include <fstream>
#include <climits>
#include <string>

struct BankAccount
{
	int accountNumber;
	int balance;
};

HANDLE hMutex;

BankAccount LoadAccount()
{
	BankAccount acc;

	std::ifstream in("account.bin", std::ios::binary);

	if (!in.is_open())
	{
		acc.accountNumber = 10001;
		acc.balance = 10000;

		std::ofstream out("account.bin", std::ios::binary);
		out.write(reinterpret_cast<char*>(&acc), sizeof(acc));
		out.close();
	}
	else
	{
		in.read(reinterpret_cast<char*>(&acc), sizeof(acc));
		in.close();

		if (acc.accountNumber != 10001 || acc.balance < 0)
		{
			acc.accountNumber = 10001;
			acc.balance = 10000;

			std::ofstream out("account.bin", std::ios::binary);
			out.write(reinterpret_cast<char*>(&acc), sizeof(acc));
			out.close();
		}
	}

	return acc;
}

void SaveAccount(BankAccount acc)
{
	std::ofstream file("account.bin", std::ios::binary);
	file.write(reinterpret_cast<char*>(&acc), sizeof(acc));
	file.close();
}

void DoOperation(int amount, bool isDeposit)
{
	DWORD pid = GetCurrentProcessId();

	BankAccount before = LoadAccount();

	DWORD w = WaitForSingleObject(hMutex, 3000);

	if (w == WAIT_TIMEOUT)
	{
		std::cout << pid << " | Время мьютекса вышло, операция отменена\n";

		std::ofstream log("operations.log", std::ios::app);

		log << pid << " | " << before.balance << " | " << (isDeposit ? "Пополнение" : "Снятие")
			<< " | " << amount << " | " << before.balance << " | " << "Время вышло\n";

		log.close();

		return;
	}

	if (w == WAIT_FAILED)
	{
		std::cout << pid << " | " << "Ошибка ожидания мьютекса\n";
		return;
	}

	if (w == WAIT_ABANDONED)
	{
		std::cout << pid << " | " << "Проверка данных счета\n";
	}

	BankAccount acc = LoadAccount();

	int start = acc.balance;
	std::string status = "Успешно";
	bool ok = false;

	if (amount <= 0)
	{
		status = "Сумма <= 0";
	}
	else if (isDeposit)
	{
		if (acc.balance > INT_MAX - amount)
		{
			status = "Переполнение";
		}
		else
		{
			acc.balance = acc.balance + amount;
			ok = true;
		}
	}
	else
	{
		if (acc.balance < amount)
		{
			status = "Недостаточно средств";
		}
		else
		{
			acc.balance = acc.balance - amount;
			ok = true;
		}
	}

	if (ok)
	{
		SaveAccount(acc);
	}

	std::ofstream log("operations.log", std::ios::app);
	log << pid << " | " << start << " | " << (isDeposit ? "Пополнение" : "Снятие")
		<< " | " << amount << " | " << acc.balance << " | " << status << "\n";
	log.close();

	std::cout << pid << " | " << (isDeposit ? "Пополнение" : "Снятие") <<
		" | " << amount << " руб. | Баланс: " << acc.balance << " | " <<
		status << "\n";

	ReleaseMutex(hMutex);
}

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
	setlocale(LC_ALL, "Russian");
	srand((unsigned int)time(NULL));

	hMutex = CreateMutex(NULL, FALSE, L"Local\\SavitsinMutex");
	if (hMutex == NULL)
	{
		std::cout << "Не удалось создать мьютекс\n";
		return GetLastError();
	}

	bool exit = false;

	while (exit == false)
	{
		PrintMenu();

		std::cout << "Выберите пункт меню: ";
		int choice;
		std::cin >> choice;

		DWORD w = 0;
		BankAccount acc;
		int amount = 0;

		switch (choice)
		{
		case 1:
		{
			w = WaitForSingleObject(hMutex, 3000);

			if (w == WAIT_TIMEOUT)
			{
				std::cout << "Вышло время мьютекса\n";
				break;
			}
			if (w == WAIT_FAILED)
			{
				std::cout << "Ошибка ожидания мьютекса\n";
				break;
			}

			if (w == WAIT_ABANDONED)
			{
				std::cout << "Проверка данных счета\n";				
			}

			acc = LoadAccount();
			std::cout << "Счет №: " << acc.accountNumber << " | " <<
				"Баланс: " << acc.balance << " руб.\n";

			ReleaseMutex(hMutex);
			break;
		}
		case 2:
		{
			std::cout << "Введите сумму пополнения: ";
			int amountDeposit;
			std::cin >> amountDeposit;
			DoOperation(amountDeposit, true);
			break;
		}
		case 3:
		{
			std::cout << "Введите сумму, которую хотите снять: ";
			int amountWithdraw;
			std::cin >> amountWithdraw;
			DoOperation(amountWithdraw, false);
			break;
		}
		case 4:
		{
			for (int i = 0; i < 20; i++)
			{
				amount = 100 + rand() % 901;
				bool deposit = (rand() % 2 == 0);
				DoOperation(amount, deposit);
				Sleep(100 + rand() % 401);
			}
			break;
		}
		case 5:
		{
			exit = true;
			break;
		}
		default:
			std::cout << "Такого пунтка меню нет.\n";
			break;
		}
	}

	CloseHandle(hMutex);

	return 0;
}