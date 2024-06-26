#include <iostream>
#include <chrono>

#include "PolyNormGF.h"

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
	GaloisField::InputMode = base::hex;  // Шестнадцатеричный ввод
	GaloisField::OutputMode = base::hex;  // Шестнадцатеричный вывод

	GaloisField a, b, c, h, result;  // Переменные - элементы поля Галуа (2^191)
	time_point<system_clock> start, end;  // Переменныя для измерения времени

	input("Enter the first element (a): ", a);
	input("Enter the second element (b): ", b);
	cout << endl;

	print("The first element in binary form: ", a.toString(base::bin));
	print("The second element in binary form: ", b.toString(base::bin));
	cout << endl;

	input("Enter the number H from 1 to (2^191 - 1): ", h);
	cout << endl;

	start = system_clock::now();
	result = a + b;
	end = system_clock::now();

	print("The sum of the first and second: ", result);
	print("Addition operation execution time: ", elapsedTime(start, end));
	print("The sum of the first element with itself: ", a + a);
	cout << endl;

	start = system_clock::now();
	result = a * b;
	end = system_clock::now();

	print("Multiplication of the first element by the second: ", result);
	print("Multiplication operation execution time: ", elapsedTime(start, end));
	cout << endl;

	start = system_clock::now();
	result = a.squared();
	end = system_clock::now();

	print("Squaring the first element: ", result);
	print("Squaring operation execution time: ", elapsedTime(start, end));
	cout << endl;

	start = system_clock::now();
	result = a.inverse();
	end = system_clock::now();

	print("Inverse element (aa) to the first element by multiplication: ", result);
	print("Inversion operation execution time: ", elapsedTime(start, end));
	print("Multiplication of a by aa: ", result * a);
	cout << endl;

	start = system_clock::now();
	result = a.pow(h);
	end = system_clock::now();

	print("The result of raising number a to the power of H: ", result);
	print("Raising to the power of H operation execution time: ", elapsedTime(start, end));
	cout << endl;

	start = system_clock::now();
	result = a.trace();
	end = system_clock::now();

	print("The result of computing Tr (trace) of the first element: ", result);
	print("The time taken to compute Tr (trace) of the first element: ", elapsedTime(start, end));
	cout << endl;

	print("Test for correctness (checking distributivity): ", "");
	print("Our first element (a): ", a);
	print("Our second element (b): ", b);
	input("Enter the third element (c): ", c);
	print("The result of (a+b)*c: ", (a + b) * c);
	print("The result of bc+ca: ", b * c + c * a);
	cout << endl;

	print("Test for correctness (checking properties of finite field elements): ", "");
	print("The result of computing a * (2^191) - a: ", a - a);

	cin.get();
	return 0;
}