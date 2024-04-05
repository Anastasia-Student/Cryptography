#include <iomanip>
#include <cstring>
#include <sstream>
#include <vector>
#include <math.h>
#include <algorithm>

using namespace std;
using namespace std::chrono;
using namespace System::Numerics;

enum class base  // Система счисления
{
	bin,  // Двоичная система счисления
	hex  // Шестнадцатеричная система счисления
};

class GaloisField  // Элемент поля Галуа
{
private:
	static const int m = 191;  // Размерность поля Галуа
	bool value[m];  // Значение элемента поля Галуа
	static bool matrix[m][m]; // Мультипликативная матрица

	// Служебная функция сложения массивов битов по модулю 2
	static void add(const bool* termA, const bool* termB, bool* summ)
	{
		for (int i = 0; i < m; i++)  // Проходимся по всем элементам
			summ[i] = termA[i] != termB[i];  // Складываем элементы по модулю 2
	}

	// Служебная функция определения остатка от деления
	static BigInteger mod(BigInteger a, BigInteger b)
	{
		if (a > BigInteger(0))  // Если делимое больше 0
			return a % b;  // Возвращаем модуль a на b
		if ((a % b) != BigInteger(0))  // Если модуль не равен 0
			// Возвращаем остаток от деления
			return BigInteger::Subtract(BigInteger::Multiply(BigInteger::Add(BigInteger::Divide(BigInteger::Abs(a),
				b), BigInteger(1)), b), BigInteger::Abs(a));
		return 0;  // Возвращаем 0
	}

	// Служебная сдвига массива бит циклично (развороты массива и развороты массива со смещением равным необходимому количеству сдвигов)
	void static shift(bool* a, int s, int size, bool left = false)
	{
		if (!left)
			s = size - s;
		reverse(a, a + s);
		reverse(a + s, a + size);
		reverse(a, a + size);
	}

	// Служебная функция умножения массивов битов
	static void mult(const bool* multA, const bool* multB, bool* multip)
	{
		bool multipA[m];  // Копия первого множителя
		bool multipB[m];  // Копия второго множителя
		bool multipAM[m];  // Результат умножения вектора на матрицу
		memcpy(multipA, multA, m);  // Копируем первый множитель в переменную
		memcpy(multipB, multB, m);  // Копируем второй множитель в переменную	
		for (int mu = 0; mu < m; mu++)  // Проходимся по всем битам результата
		{
			memset(multipAM, 0, m);  // Обнуляем произведение матрицы и вектора
			for (int i = 0; i < m; i++)  // Проходимся по столбцам матрицы
				for (int j = 0; j < m; j++) // Проходимся по строкам матрицы
					multipAM[i] = multipAM[i] != (matrix[i][j] * multipA[m - j - 1]);  // Умножаем и прибавляем
			for (int i = 0; i < m; i++)// Проходимся по вектору
				multip[m - mu - 1] = multip[m - mu - 1] != (multipB[m - i - 1] * multipAM[i]);  // Умножаем и прибавляем		
			shift(multipA, 1, m);  // Сдвигаем множитель вправо
			shift(multipB, 1, m);  // Сдвигаем множитель вправо
		}
	}

	// Служебная функция возведения в квадрат массива битов
	static void squared(bool* element, int n = 1)
	{
		shift(element, n, m, true);  // Сдвигаем влево на 1 (или больше если квадрат не один)
	}

	// Служебная функция возведения в степень массива битов по модулю
	static void pow(const bool* element, const bool* power, bool* result)
	{
		int count = m - 1;  // Переменная для поиска старшего бита
		for (; count >= 0; count--)  // Проходимя по всем битам
			if (power[count])  // Если бит не нулевой
				break; // Выходим
		count++;  // Прибавляем 1 (Для конвертации в количество)

		if (count == 1)  // Если 1 бит ( Первая степень )
			memcpy(result, element, m);  // Копируем елемент в результат
		else
		{
			if (count == 0)  // Если нет ненулевых бит ( Нулевая степень )
				memset(result, 1, m);  // Возвращаем константу 1
			else
			{
				bool d[m];  // Переменная для алгоритма возведения в степень
				bool t[m];  // Переменная для алгоритма возведения в степень
				memcpy(t, element, m); // Копируем элемент в t
				memset(d, 0, m); // Обнуляем d
				bool empty = true;  // Флаг того что d пустой
				bool mul[m];  // Переменная для умножения
				for (int i = 0; i <= count; i++)  // Проходимся по битам степени
				{
					if (power[i]) {  // Если не нулевой бит
						if (empty)  // Если в d пусто
						{
							memcpy(d, t, m);  // Копируем t в d
							empty = false;  // В d пусто
						}
						else  // Иначе
						{
							memset(mul, 0, m);  // Обнуляем mul
							mult(d, t, mul);  // Умножаем d на t
							memcpy(d, mul, m);  // Кладем обратно в d
						}
					}
					squared(t);  // Возводим t в квадрат
				}
				memcpy(result, d, m);  // Копируем d в результат
			}
		}
	}

public:
	static base InputMode;  // Система счисление ввода элемента поля Галуа
	static base OutputMode;  // Система счисление вывода элемента поля Галуа

	// Коструктор
	GaloisField()
	{
		memset(value, 0, sizeof(value));  // Инициализируем значение нулями
	}

	// Поиск константы 0 – нейтрального элемента по операции "+"
	static GaloisField constantZero()
	{
		return GaloisField();  // Возвращаем константу 0
	}

	// Поиск константы 1 – нейтрального элемента по операции "*"
	static GaloisField constantOne()
	{
		GaloisField result;
		memset(result.value, 1,  m);
		return result;
	}

	// Инициализация мультипликативной матрицы
	static void Init() 
	{
		BigInteger one = 1;  // Единица
		BigInteger two = 2;  // Двойка
		BigInteger div = 2 * m + 1;  // Модуль
		for (int i = 0; i < m; i++)  // Проходимся по всем столбцам
		{
			for (int j = i; j < m; j++)  // Проходимся по всем строкам
			{
				matrix[i][j] =  // Определяем значение элемента матрицы
					// (2 ^ i + 2 ^ j) mod (2 * m + 1)
					mod(BigInteger::Add(BigInteger::Pow(two, i), BigInteger::Pow(two, j)), div) == one ||
					// (2 ^ i - 2 ^ j) mod (2 * m + 1)
					mod(BigInteger::Add(BigInteger::Pow(two, i), -BigInteger::Pow(two, j)), div) == one ||
					// (-2 ^ i + 2 ^ j) mod (2 * m + 1)
					mod(BigInteger::Add(-BigInteger::Pow(two, i), BigInteger::Pow(two, j)), div) == one ||
					// (-2 ^ i - 2 ^ j) mod (2 * m + 1)
					mod(BigInteger::Add(-BigInteger::Pow(two, i), -BigInteger::Pow(two, j)), div) == one;

				if (i != j)  // Если не находимя на главной диагонали
					matrix[j][i] = matrix[i][j];  // Отзеркаливаем элемент
			}
		}
	}

	// Сложение элементов поля Галуа
	GaloisField operator + (const GaloisField& other) const  // Сложение элементов
	{
		GaloisField result;  // Создаем новый элемент поля Галуа
		add(value, other.value, result.value);  // Используем служебную функцию для сложения
		return result;  // Возвращаем сумму
	}

	// Вычитание элементов поля Галуа
	GaloisField operator - (const GaloisField& other) const  // Сложение элементов
	{
		GaloisField result;  // Создаем новый элемент поля Галуа
		add(value, other.value, result.value);  // Используем служебную функцию для сложения
		return result;  // Возвращаем сумму
	}

	// Умножение элементов поля Галуа
	GaloisField operator * (const GaloisField& other) const
	{
		GaloisField result;  // Создаем новый элемент поля Галуа
		mult(value, other.value, result.value);  // Умножаем элементы испоьзуя служебную функцию
		return result;  // Возвращаем произведение
	}

	// Поиск следа элемента поля Галуа
	GaloisField trace() const
	{
		GaloisField result;  // Создает переменную для следа
		for (int i = 0; i < m; i++)  // Проходимя по всем битам элемента
			result.value[0] = result.value[0] != value[i];  // Складываем биты
		return result;  // Возвращаем сумму
	}

	// Возведение элемента поля Галуа в квадрат
	GaloisField squared() const
	{
		GaloisField result = *this;  // Создаем новый элемент поля Галуа
		squared(result.value);  // Использую служебную функцию возводим элемент в квадрат
		return result;  // Возвращаем квадрат элемента поля Галуа
	}

	// Возведение элемента поля Галуа в степень
	GaloisField pow(const GaloisField& power) const
	{
		GaloisField result;  // Создаем новый элемент поля Галуа
		pow(value, power.value, result.value);  // Использую служебную функцию возводим элемент в степень
		return result;  // Возвращаем элемент в степени power поля Галуа
	}

	// Поиск обратного элемента поля Галуа
	GaloisField inverse() const
	{
		GaloisField result;
		bool p[m];
		memcpy(p, constantOne().value, m);
		p[0] = 0;
		pow(value, p, result.value);
		return result;  // Возвращаем результат
	}

	// Поиск обратного элемента поля Галуа (метод Ито-Цудзии)

	GaloisField inverse2() const
	{

		bool b[m]; // Переменная для возведения в квадрат
		bool mul[m];  // Переменная для умножения
		memset(b, 0, m);  // Обнуляем
		memset(mul, 0, m);  // Обнуляем
		GaloisField result = *this;  // Копируем значение в переменную результата
		int k = 1;  // k
		for (int i = 6; i >= 0; i--)  // Проходимся по битам m - 1 (190)
		{
			memset(mul, 0, m);  // Обнуляем
			memcpy(b, result.value, m);  // Копируем значение в б
			squared(b, k);  // Возводим в квадрат k раз
			mult(b, result.value, mul);  // Умножаем
			memcpy(result.value, mul, m);  // Копируем обратно
			k = (k * 2) % m;  // k:=2k
			if ((m - 1) >> i & 1)  // Если бит i в m - 1 равен 1 
			{
				memset(mul, 0, m);  // Обнуляем
				memcpy(b, result.value, m);  // Копируем значение в б
				squared(b, 1); // Возводим в квадрат
				mult(b, value, mul); // Умножаем
				memcpy(result.value, mul, m);  // Копируем обратно
				k = (k + 1) % m;  // k:=k + 1;
			}
		}
		squared(result.value, 1);  // Финальный квадрат 
		return result;  // Возвращаем результат
	}

	// Конвертирование элемента поля Галуа в строку
	string toString(base base = OutputMode) const
	{
		int i, j;  // Переменные для циклов
		string result = "";  // Результат преобразования в строку
		switch (base)  // В зависимости от системы счисления
		{
		case base::bin:  // Если двоичная система счисления
		{
			for (i = m - 1; i >= 0; i--)  // Проходим по всем битам элемента поля Галуа
				result += ('0' + value[i]);  // Преобразуем бит в соответствующий символ и добавляем к результирующей строке
			int oneIndex = result.find('1');
			if (oneIndex != -1)
				result = result.substr(oneIndex, result.size() - oneIndex);
			else
				result = "0";
			break;  // Выходим из switch
		}
		case base::hex:  // Если шестнадцатеричная система счисления
		{
			stringstream ss;  // Строковый поток
			for (i = m / 32 * 32 + 32; i >= 0; i -= 32)  // Проходимся по всем битам элемента поля галуа каждые 32 бита
			{
				long slice = 0;  // Переменная для хранения 32 бит
				for (j = i; j < i + 32; j++)  // Проходимся по 32 битам
				{
					if (j < m)  // Если номер бита < m (191)
						if (value[j])  // Если бит равен 1
							slice |= (1 << (j - i));  // Ставим 1 в позицию j - i в переменную slice
						else // Если бит равен 0
							slice &= ~(1 << (j - i)); // Ставим 0 в позицию j - i в переменную slice
				}
				ss << setw(8) << setfill('0') << uppercase << hex << slice;  // Преобразуем slice в hex строку длинной 8 символов
			}
			ss >> result;
			int indNotZero = 0;
			for (; indNotZero < result.size(); indNotZero++)
				if (result[indNotZero] != '0')
					break;
			if (indNotZero < result.size())
				result = result.substr(indNotZero, result.size() - indNotZero);
			else
				result = "0";
			break;  // Выходим из switch
		}
		}
		return result == "" ? "0" : result;  // Если строка пуста то возвращаем '0' иначе строку (result)
	}

	// Конвертирование строки в элемент поля Галуа
	void fromString(string mbitnum, base base = InputMode)  // Конвертирование строки в большое число
	{
		memset(value, 0, sizeof(value));  // Очищаем значение элемента поля Галуа
		switch (base)  // В зависимости от системы счисления
		{
		case base::bin:  // Если двоичная система счисления
		{
			if (mbitnum.size() > m)  // Если длинна строки больше m (191)
				throw string("Элемент не принадлежит полю: размер элемента больше размерности поля (191)!");  // То выводим ошибку
			for (int i = mbitnum.size() - 1; i >= 0; i--)  // Проходимся по всем символам строки
			{
				if (mbitnum[i] < '0' || mbitnum[i] > '1')  // Если символ не 0 и не 1
					throw "Некорректное двоичное число: символ '" + string(1, mbitnum[i]) + "' не является цифрой!";  // То выводим ошибку
				value[mbitnum.size() - i - 1] = mbitnum[i] - '0';  // Преобразуем символ в соответствующий бит
			}
			break;  // Выход из switch
		}
		case base::hex:  // Если шестнадцатеричная система счисления
		{
			int slice = 0;
			for (int i = 0; i < mbitnum.size(); i++)  // Проходимся по всем символам
			{
				if (!(mbitnum[i] >= '0' && mbitnum[i] <= '9') &&  // Если символ не соответствует цифре шестнадцатеричной система
					!(mbitnum[i] >= 'A' && mbitnum[i] <= 'F') &&
					!(mbitnum[i] >= 'a' && mbitnum[i] <= 'f'))
					throw "Некорректное шестнадцатеричное число: символ '" + string(1, mbitnum[i]) + "' не является цифрой!";  // То выводим ошибку
				slice = stoi(mbitnum.substr(i, 1), 0, 16);  // Преобразуем символ в 4 бита
				for (int j = 0; j < 4; j++)  // Проходимся по битам
					if ((mbitnum.size() - i - 1) * 4 + j >= m && ((slice >> j) & 1) == 1)
						throw string("Элемент не принадлежит полю: размер элемента превышает размерность поля (191)!");  // То выводим ошибку
			}
			for (int i = mbitnum.size() - 1; i >= 0; i--)  // Проходимся по всем символам
			{
				slice = stoi(mbitnum.substr(i, 1), 0, 16);  // Преобразуем символ в 4 бита
				for (int j = 0; j < 4 && (mbitnum.size() - i - 1) * 4 + j < m; j++)  // Проходимся по битам
				{
					value[m - (i * 4) + j - (m - (mbitnum.size() - 1) * 4)] = ((slice >> j) & 1);
				}
			}
			break;  // Выход из switch
		}
		}
	}

	// Вывод элемента поля Галуа в консоль
	friend std::ostream& operator << (std::ostream& out, const GaloisField& galoisField)
	{
		out << galoisField.toString();  // Вывод строки в поток вывода в системе счисления записанной в переменную InputMode
		return out;  // Возвращаем поток вывода
	}

	// Ввод элемента поля Галуа из консоли
	friend std::istream& operator >> (std::istream& in, GaloisField& galoisField)  // Перегрузка оператора ввода
	{
		string str;  // Строка для преобразования
		in >> str;  // Ввод из потока ввода в строку для преобразования
		try {
			galoisField.fromString(str);  // Преобразование строки в элемент поля Галуа
		}
		catch (string error) // Обработка ошибки
		{
			cout << error;  // Вывод ошибки
			cin.get();  // Ожидание ввода
			cin.get();  // Ожидание ввода
			exit(0);  // Выход из приложения
		}
		return in;  // Возвращаем поток ввода
	}
};

base GaloisField::InputMode = base::bin;  // Инициализация системы счисление ввода элемента поля Галуа
base GaloisField::OutputMode = base::bin;  // Инициализация системы счисление вывода элемента поля Галуа
bool GaloisField::matrix[m][m];  // Инициализация мультипликативной матрицы
