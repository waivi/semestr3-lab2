#include <iostream>
#include <unordered_set>
#include <string>
#include <cctype>

using namespace std;

class SetManager {
private:
    unordered_set<string> data;

public:
    SetManager() = default;

    // SETADD - добавление элемента
    void SETADD(const string& element) {
        auto result = data.insert(element);
    }

    // Дополнительный метод для проверки наличия элемента (без вывода)
    bool contains(const string& element) const {
        return data.find(element) != data.end();
    }

    // Метод для очистки множества
    void clear() {
        data.clear();
    }

    // Метод для добавления нескольких элементов
    void addAll(const unordered_set<string>& elements) {
        for (const auto& element : elements) {
            data.insert(element);
        }
    }
};

// Функция для получения всех пар соседних оснований из строки
unordered_set<string> getAllPairs(const string& genome) {
    unordered_set<string> pairs;

    for (size_t i = 0; i < genome.length() - 1; ++i) {
        string pair = genome.substr(i, 2);
        pairs.insert(pair);
    }

    return pairs;
}

// Функция для проверки корректности генома
bool isValidGenome(const string& genome) {
    // Проверка на пустую строку
    if (genome.empty()) {
        cerr << "Ошибка: Геном не может быть пустой строкой" << endl;
        return false;
    }

    // Проверка на длину (по условию задачи <= 10^5)
    if (genome.length() > 100000) {
        cerr << "Ошибка: Длина генома не может превышать 100000 символов" << endl;
        return false;
    }

    // Проверка что все символы - заглавные английские буквы
    for (char c : genome) {
        if (c < 'A' || c > 'Z') {
            cerr << "Ошибка: Геном может содержать только заглавные английские буквы (A-Z)" << endl;
            cerr << "Найден недопустимый символ: '" << c << "'" << endl;
            return false;
        }
    }

    return true;
}

// Функция для безопасного ввода генома
string inputGenome(const string& prompt) {
    string genome;
    while (true) {
        cout << prompt;
        cin >> genome;

        if (isValidGenome(genome)) {
            return genome;
        }
        else {
            cout << "Пожалуйста, введите геном снова." << endl;
        }
    }
}

int main() {
    setlocale(LC_ALL, "RU");

    // Ввод и проверка геномов
    string genome1 = inputGenome("Введите первый геном: ");
    string genome2 = inputGenome("Введите второй геном: ");

    // Создаем SetManager и добавляем все пары из второго генома
    SetManager setManager;
    setManager.clear(); // Очищаем множество

    // Получаем все уникальные пары из второго генома
    unordered_set<string> pairs2 = getAllPairs(genome2);

    // Добавляем все пары в SetManager
    for (const auto& pair : pairs2) {
        setManager.SETADD(pair);
    }

    // Подсчитываем степень близости
    int closeness = 0;
    for (size_t i = 0; i < genome1.length() - 1; ++i) {
        string pair = genome1.substr(i, 2);
        if (setManager.contains(pair)) {
            closeness++;
        }
    }

    cout << "Степень близости: " << closeness << endl;

    return 0;
}