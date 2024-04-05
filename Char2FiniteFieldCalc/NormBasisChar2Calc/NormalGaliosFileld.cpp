#include <iostream>
#include <clocale>
#include <chrono>

#include "LibNormGF.h"

using namespace std;
using namespace std::chrono;

void input(string message, GaloisField& el)  // Ввод элементов
{
    cout << message;  // Выводим сообщение
    cin >> el;  // Вводим элемент
}

template<typename T>
void print(string message, T el)  // Вывод элементов
{
    cout << message;  // Выводим сообщение
    cout << el << endl;  // Выводим элемент
}

string elapsedTime(time_point<system_clock> start, time_point<system_clock> end)  // Конвертирование время в строку
{
	int count = duration_cast<microseconds>(end - start).count();  // Считаем прошедшее время
	return count > 1000 ? to_string(count / 1000) + " ms" : to_string(count) + " μs";  // Возвращаем строку
}


int main()
{
	setlocale(LC_ALL, "rus");  // Поддержка кириллицы
	
	string mode;
	cout << "Введите систему счисления (bin/hex): ";
	cin >> mode;
	if (mode == "bin")
	{
		GaloisField::InputMode = base::bin;  // Шестнадцатеричный ввод
		GaloisField::OutputMode = base::bin;  // Шестнадцатеричный вывод
		cout << "Устновлена двоичная система счисления (bin).\n" << endl;		
	}
	else
	{
		GaloisField::InputMode = base::hex;  // Шестнадцатеричный ввод
		GaloisField::OutputMode = base::hex;  // Шестнадцатеричный вывод
		cout << "Устновлена шестнадцатеричная система счисления (hex).\n" << endl;
	}

	GaloisField::Init();  // Инициализация мультипликативной матрицы

	GaloisField a, b, c, h, result;  // Переменные - элементы поля Галуа (2^191)
	time_point<system_clock> start, end;  // Переменныя для измерения времени

	input("Введите первый элемент (a): ", a);
	input("Введите второй элемент (b): ", b);
	cout << endl;

	print("Первый элемент в двоичном виде: ", a.toString(base::bin));
	print("Второй элемент в двоичном виде: ", b.toString(base::bin));
	cout << endl;

	input("Введите число H от 1 до (2^191 - 1): ", h);
	cout << endl;

	start = system_clock::now();
	result = a + b;
	end = system_clock::now();

	print("Сумма первого и второго: ", result);
	print("Время работы операции сложения: ", elapsedTime(start, end));
	print("Сумма первого элемента с самим собой: ", a + a);
	cout << endl;

	start = system_clock::now();
	result = a * b;
	end = system_clock::now();

	print("Умножение первого элемента на второй: ", result);
	print("Время работы операции умножения: ", elapsedTime(start, end));
	cout << endl;

	start = system_clock::now();
	result = a.squared();
	end = system_clock::now();

	print("Возведение первого элемента в квадрат: ", result);
	print("Время работы операции возведения в квадрат первого элемента: ", elapsedTime(start, end));
	cout << endl;

	start = system_clock::now();
	result = a.inverse();
	end = system_clock::now();

	print("Обратный элемент (aa) к первому элементу по умножению: ", result);
	print("Время вычисления обратного к первому элементу: ", elapsedTime(start, end));
	print("Результат умножения a на aa: ", result * a);
	cout << endl;

	start = system_clock::now();
	result = a.inverse2();
	end = system_clock::now();

	print("Обратный элемент (aa) к первому элементу по умножению (метод Ито-Цудзии): ", result);
	print("Время вычисления обратного к первому элементу (метод Ито-Цудзии): ", elapsedTime(start, end));
	print("Результат умножения a на aa: ", result * a);
	cout << endl;

	start = system_clock::now();
	result = a.pow(h);
	end = system_clock::now();

	print("Результат возведения числа a в степень H: ", result);
	print("Время возведения числа a в степень H: ", elapsedTime(start, end));
	cout << endl;

	start = system_clock::now();
	result = a.trace();
	end = system_clock::now();

	print("Результат вычисления Tr(первого элемента): ", result);
	print("Время вычисления Tr(первого элемента): ", elapsedTime(start, end));
	cout << endl;

	print("Тест на корректность (проверка дистрибутивности): ", "");
	print("Наш первый элемент (a): ", a);
	print("Наш второй элемент (b): ", b);
	input("Введите третий элемент (c): ", c);
	print("Результат (a+b)*c : ", (a + b) * c);
	print("Результат b*c+c*a : ", b * c + c * a);
	cout << endl;

	print("Тест на корректность (проверка свойства элементов конечных полей): ", "");
	print("Результат вычисления a (2^191) - a : ", a - a);

	cin.get();
	return 0;
}