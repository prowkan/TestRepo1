#include <iostream>

#include <Windows.h>

int main()
{
	UINT OldConsoleCP = GetConsoleCP();
	UINT OldConsoleOutputCP = GetConsoleOutputCP();

	(void)SetConsoleCP(65001);
	(void)SetConsoleOutputCP(65001);

	std::cout << "Тест, тест, тест." << std::endl;

	(void)SetConsoleCP(OldConsoleCP);
	(void)SetConsoleOutputCP(OldConsoleOutputCP);

	return 0;
}