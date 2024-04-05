#include <iomanip>
#include <cstring>
#include <sstream>

using namespace std;

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
	bool generator[m + 1]; // Генератор поля Галуа

	// Служебная функция поиска старшего первого ненулевого бита в массиве
	static int bitsCount(const bool* array, int s = m)
	{
		for (int i = s - 1; i >= 0; i--)  // Проходимся по всем элементам массива начиная с конца
			if (array[i])  // Если элемент равен 1
				return i + 1;  // Возвращаем его позицию
		return 0;  // Возвращаем 0 если массив не содержит 1
	}

	// Служебная функция сравнения массивов битов
	static int compare(const bool* left, const bool* right, unsigned int leftCount, unsigned int rightCount)
	{
		if (leftCount > rightCount)  // Сравнение по размеру
			return 1;  // Если больше
		if (rightCount > leftCount)  // Сравнение по размеру
			return -1;  // Если меньше
		for (int i = leftCount - 1; i >= 0; i--)  // Проход по всем элементам массива
		{
			if (left[i] > right[i])  // Если элемент первого массива больше элемента второго массива
				return 1;  // То больше
			if (left[i] < right[i]) // Если элемент второго массива больше элемента первого массива
				return -1;  // То меньше
		}
		return 0;  // Равно
	}

	// Служебная функция сложения массивов битов по модулю 2
	static void add(const bool* termA, const bool* termB, bool* summ, int termACount, int termBCount)
	{
		int count = (termACount > termBCount ? termACount : termBCount);  // Находим размер большего массива
		for (int i = 0; i < count; i++)  // Проходимся по всем элементам
			summ[i] = termA[i] != termB[i];  // Складываем элемента по модулю 2
	}

	// Служебная функция умножения массивов битов
	static void mult(const bool* multA, const bool* multB, const bool* moddiv, bool* multip, int multACount, int multBCount, int modCount)
	{
		const int dm = m * 2 + 1;  // Считаем размер массива необходимый для хранения произведения
		bool remainder[dm];  // Остаток от деления (на генератор поля)
		bool multiptmp[dm];  // Результат умножения
		memset(multiptmp, 0, dm);  // Инициализируем произведение нулями
		memset(remainder, 0, sizeof(remainder));  // Инициализируем остаток нулями
		for (int i = 0; i < multACount; i++)  // Проход по всем битам первого множителя
			for (int j = 0; j < multBCount; j++)  // Проход по всем битам второго множителя
				multiptmp[j + i] = multiptmp[j + i] != (multA[i] && multB[j]);  // Умножаем и прибавляем по модулю 	
		mod(multiptmp, moddiv, remainder, bitsCount(multiptmp, dm), m + 1); // Находим остаток от деления на генератор поля Галуа
		copy(multip, remainder, m);  // Копируем результат умножения в значение нового элемента
	}

	// Служебная функция возведения в квадрат массива битов по модулю
	static void squared(const bool* element, const bool* moddiv, bool* result, int elementCount, int moddivCount)
	{
		const int dm = m * 2 + 1;  // Считаем размер массива необходимый для хранения квадрата элемента поля Галуа
		bool remainder[dm];  // Остаток от деления (на генератор поля)
		bool square[dm];  // Значение квадрата до деления по модуля
		memset(remainder, 0, sizeof(remainder));  // Инициализируем остаток нулями
		memset(square, 0, sizeof(square));  // Инициализируем остаток нулями
		for (int i = 0; i < m; i++)  // Проход по всем битам элемента поля Галуа
			square[2 * i] = element[i];  // Сдвигаем позицию битов элемента поля Галуа в два раза 
		mod(square, moddiv, remainder, bitsCount(square, dm), m + 1);  // Находим остаток от деления на генератор поля Галуа
		copy(result, remainder, m);  // Копируем результат возведения во вторую степень в значение нового элемента
	}

	// Служебная функция возведения в степень массива битов по модулю
	static void pow(const bool* element, const bool* power, const bool* moddiv, bool* result, int elementCount, int powerCount, int moddivCount)
	{
		bool d[m];  // Переменная для алгоритма возведения в степень по модулю
		bool dtmp[m];  // Переменная для алгоритма возведения в степень по модулю
		bool t[m];  // Переменная для алгоритма возведения в степень по модулю
		bool ttmp[m];  // Переменная для алгоритма возведения в степень по модулю
		memset(d, 0, m);  // Инициализируем нулями
		memset(t, 0, m);  // Инициализируем нулями
		d[0] = 1;  // Устанавливаем d = 1
		copy(t, element, elementCount); // Копируем элемент в t
		int count = bitsCount(power);  // Количество бит степени
		for (int i = 0; i < count; i++)  // Проходимся по битам степени
		{
			if (power[i])  // Если бит = 1
			{
				memset(dtmp, 0, m);  // Инициализируем нулями
				mult(d, t, moddiv, dtmp, m, m, m + 1);  // Умножаем d на t и кладем в dtmp
				memset(d, 0, m);  // Инициализируем нулями
				copy(d, dtmp, bitsCount(dtmp, m));  // Копируем обратно в d
			}
			memset(ttmp, 0, m);  // Инициализируем нулями
			squared(t, moddiv, ttmp, m, m + 1);  // Возводим t в квадрат и кладем в ttmp
			memset(t, 0, m);  // Инициализируем нулями
			copy(t, ttmp, bitsCount(ttmp, m));  // Копируем обратно в t
		}
		copy(result, d, m);  // Копируем d в результат
	}

	// Служебная функция сдвига массива битов
	static void move(bool* element, int shift, int elementCount)
	{
		int bqa = bitsCount(element, elementCount);  // Поиск первого не нулевого элемента
		if (shift > 0)  // Если сдвиг вправо
		{
			for (int i = bqa - 1; i >= 0; i--)
				if (i + shift >= 0 && i + shift < elementCount)
					element[i + shift] = element[i];
			for (int i = 0; i < shift; i++)
				element[i] = 0;
		}
		if (shift < 0)  // Если сдвиг влево
		{
			for (int i = -shift; i < bqa; i++)
				if (i + shift >= 0 && i + shift < elementCount)
					element[i + shift] = element[i];
			for (int i = bqa; i >= bqa + shift; i--)
				if (i >= 0 && i < elementCount)
					element[i] = 0;
		}
	}

	// Служебная функция копирование массивов
	static void copy(bool* dest, const bool* source, int count, int start = 0)
	{
		for (int i = 0; i < count; i++)  // Проход по всем элементам массива
			dest[i] = source[start + i];  // Копирование элементов
	}

	// Служебная функция вывода массива битов в консоль
	static void print(const bool* element, int count)
	{
		for (int i = count - 1; i >= 0; i--)  // Проход по всем элементам массива
			cout << element[i];  // Вывод элемента
		cout << endl;  // Вывод перевода строки
	}

	// Служебная функция поиска остатка от деления массива на массив
	static void mod(const bool* dividend, const bool* divider, bool* remainder, int dividendCount, int dividerCount)
	{
		//cout << "::> ";
		//print(dividend, dividendCount);
		int diff = dividendCount - dividerCount;  // Разница размеров делимого и делителя
		int shift = 0;  // Сдвиг делителя по делимому при делении
		if (bitsCount(dividend, dividendCount) >= dividerCount)  // Если делимое больше или равно делителю
		{
			for (int i = dividendCount; i >= diff; i--)  // Копируем количество битов
				remainder[i - diff] = dividend[i];  // Равное количеству битов делителя в ост
			do
			{
				add(remainder, divider, remainder, dividerCount, dividerCount);  // Вычитаем из остатка делитель
				shift = dividerCount - bitsCount(remainder, dividerCount);  // Высчитываем новый сдвиг
				if (diff - shift < 0)  // Если сдвиг больше оставшихся бит делимого
					shift = diff;  // То приравниваем количеству оставшихся бит делимого
				move(remainder, shift, dividerCount);  // Смещаем остаток
				diff -= shift;  // Вычитаем из оставшихся бит делимого сдвиг
				copy(remainder, dividend, shift, diff);  // Добавляем в конец остатка биты из делимого
			} while (bitsCount(remainder, dividerCount) == dividerCount || diff != 0);  // Выходим если остаток меньше делителя
		}
		else
			copy(remainder, dividend, dividendCount);  // Если нет то просто копируем делимое в остаток
	}

public:
	static base InputMode;  // Система счисление ввода элемента поля Галуа
	static base OutputMode;  // Система счисление вывода элемента поля Галуа

	// Коструктор
	GaloisField(unsigned long long num = 0)
	{
		memset(value, 0, sizeof(value));  // Инициализируем значение нулями
		memset(generator, 0, m + 1);  // Инициализируем значение нулями генератор поля
		generator[191] = 1;  // Генератор поля x^191
		generator[9] = 1;  // Генератор поля x^9
		generator[0] = 1;  // Генератор поля 1
		for (int i = 0; i < 64; i++)  // Проходим по переменной num
			value[i] = (num >> i) & 1;  // И копируем биты
	}

	// Поиск константы 0 – нейтрального элемента по операции "+"
	static GaloisField constantZero()
	{
		return GaloisField();  // Возвращаем константу 0
	}

	// Поиск константы 1 – нейтрального элемента по операции "*"
	static GaloisField constantOne()
	{
		return GaloisField(1);  // Возвращаем константу 1
	}

	// Сложение элементов поля Галуа
	GaloisField operator + (const GaloisField& other) const  // Сложение элементов
	{
		GaloisField result;  // Создаем новый элемент поля Галуа
		add(value, other.value, result.value, bitsCount(), other.bitsCount());  // Используем служебную функцию для сложения
		return result;  // Возвращаем сумму
	}

	// Вычитание элементов поля Галуа
	GaloisField operator - (const GaloisField& other) const  // Сложение элементов
	{
		GaloisField result;  // Создаем новый элемент поля Галуа
		add(value, other.value, result.value, bitsCount(), other.bitsCount());  // Используем служебную функцию для сложения
		return result;  // Возвращаем сумму
	}

	// Умножение элементов поля Галуа
	GaloisField operator * (const GaloisField& other) const
	{
		GaloisField result;  // Создаем новый элемент поля Галуа
		mult(value, other.value, generator, result.value, bitsCount(), other.bitsCount(), m + 1);  // Умножаем элементы испоьзуя служебную функцию
		return result;  // Возвращаем произведение
	}

	// Поиск следа элемента поля Галуа
	GaloisField trace() const
	{
		GaloisField result = *this;  // Присваиваем результирующему элементу значение данного
		bool powerTwo[m];  // Массив для хранение степеней двойки 10 100 1000 ...
		bool donePower[m];  // Степень в которую возводится элемент
		memset(powerTwo, 0, m);  // Инициализируем нулями
		for (int i = 1; i < m; i++)  // Проходим по степеням от 1 до m - 1
		{
			memset(donePower, 0, m); // Инициализируем нулями
			powerTwo[i] = 1;  // Степень двойки (при i = 1 powerTwo = 10 при i = 2 powerTwo = 100 ...)
			powerTwo[i - 1] = 0;  // Обнуляем предыдущею степень
			pow(value, powerTwo, generator, donePower, bitsCount(), bitsCount(powerTwo), m + 1);  // Считаем слагаемое (a^2^m-...)
			add(result.value, donePower, result.value, result.bitsCount(), bitsCount(donePower));  // Добавляем к сумме
		}
		return result;  // Возвращаем сумму
	}

	// Возведение элемента поля Галуа в квадрат
	GaloisField squared() const
	{
		GaloisField result;  // Создаем новый элемент поля Галуа
		squared(value, generator, result.value, bitsCount(), m + 1);  // Использую служебную функцию возводим элемент в квадрат
		return result;  // Возвращаем квадрат элемента поля Галуа
	}

	// Возведение элемента поля Галуа в степень
	GaloisField pow(const GaloisField& power) const
	{
		GaloisField result;  // Создаем новый элемент поля Галуа
		pow(value, power.value, generator, result.value, bitsCount(), power.bitsCount(), m + 1);  // Использую служебную функцию возводим элемент в степень
		return result;  // Возвращаем элемент в степени power поля Галуа
	}

	// Поиск обратного элемента поля Галуа
	GaloisField inverse() const
	{
		GaloisField result = *this;  // Присваиваем результирующему элементу значение данного
		bool powerTwo[m];  // Массив для хранение степеней двойки 10 100 1000 ...
		bool multip[m];  // Множитель a^(2^m-2) a^(2^m-3) ...
		memset(powerTwo, 0, m);  // Инициализируем переменную хранящую степень двойки нулями
		for (unsigned int i = 1; i < m - 1; i++)  // Проходимся по степенях от 1 до m - 2
		{
			memset(multip, 0, m);  // Инициализируем множитель нулями
			powerTwo[i] = 1;  // Степень двойки (при i = 1 powerTwo = 10 при i = 2 powerTwo = 100 ...)
			powerTwo[i - 1] = 0;  // Обнуляем предыдущею степень
			pow(value, powerTwo, generator, multip, bitsCount(), bitsCount(powerTwo), m + 1);  // Считаем множитель (a^2^m-...)
			mult(result.value, multip, generator, result.value, result.bitsCount(), bitsCount(multip), m + 1);  // Умножаем результат на множитель
		}
		return result.squared();  // Возвращаем квадрат произведения
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
			for (i = bitsCount() - 1; i >= 0; i--)  // Проходим по всем битам элемента поля Галуа
				result += ('0' + value[i]);  // Преобразуем бит в соответствующий символ и добавляем к результирующей строке
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
			ss >> result;  // Сохраняем результат в result
			for (i = 0; result[i] == '0' && i < result.size(); i++) {}  // Ищем первый символ не '0' 
			result = result.substr(i, result.size() - i);  // Отсекаем нули в начале
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
				value[mbitnum.size() - 1 - i] = mbitnum[i] - '0';  // Преобразуем символ в соответствующий бит
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
					value[(mbitnum.size() - i - 1) * 4 + j] = ((slice >> j) & 1);
				}
			}
			break;  // Выход из switch
		}
		}
	}

	// Поиск старшего ненулевого бита в элементе поля Галуа
	int bitsCount() const
	{
		for (int i = m - 1; i >= 0; i--)  // Проходимся по всем битам элемента поля Галуа начиная с конца
			if (value[i])  // Если бит не равен нулю
				return i + 1;  // Возвращаем его позицию
		return 0;  // Возвращаем 0 (если не нулевых битов нет)
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