#include <iostream>
#include <vector>
#include <string>
#include <chrono>
#include <random>
#include <algorithm>
#include <iomanip>
#include <climits>
#include <utility>
using namespace std;

// Базовый класс для хеш-таблицы
class HashTable {
protected:
    int capacity; //размер таблицы
    int size;//количество элементов

public:
    HashTable(int capacity) : capacity(capacity), size(0) {}
    virtual ~HashTable() {}

    virtual void add(pair<int, int> keyValue) = 0;
    virtual void remove(int key) = 0;
    virtual pair<bool, int> contains(int key) = 0; // возвращает (найдено ли, значение)
    virtual string toString() = 0;

    int getSize() const { return size; }
    int getCapacity() const { return capacity; }
    double getLoadFactor() const { return static_cast<double>(size) / capacity; }
};

// Узел для метода цепочек
struct Node {
    pair<int, int> keyValue; // пара ключ-значение
    Node* next;
    Node(int k, int v) : keyValue(make_pair(k, v)), next(nullptr) {}
};

// Метод цепочек
class ChainingHashTable : public HashTable {
private:
    vector<Node*> table;//вектор указателей на узлы

    int hash(int key) {
        return key % capacity;
    }

public:
    ChainingHashTable(int capacity) : HashTable(capacity) {
        table.resize(capacity, nullptr);
    }

    ~ChainingHashTable() {
        for (int i = 0; i < capacity; i++) {
            Node* current = table[i];
            while (current != nullptr) {
                Node* temp = current;
                current = current->next;
                delete temp;
            }
        }
    }

    void add(pair<int, int> keyValue) override {
        auto result = contains(keyValue.first);
        if (result.first) return; //проверка на дубликаты

        int h = hash(keyValue.first); //вычисляем хэш значение по ключу
        Node* newNode = new Node(keyValue.first, keyValue.second); //создаём новый узел с парой ключ-значение

        if (table[h] == nullptr) { //вставка в таблицу если это первый элемент в цепочке будет
            table[h] = newNode;
        }
        else { //коллизия
            Node* current = table[h]; //указывает на узел первый в цепочке, на саму ячейку
            while (current->next != nullptr) {
                current = current->next;//проходим до конца цепочки
            }
            current->next = newNode; //добавляем в конец цепочки узел новый
        }
        size++;
    }

    void remove(int key) override { //удалить 
        int h = hash(key);
        Node* current = table[h];
        Node* prev = nullptr; //для отслеживания предыдущего узла

        while (current != nullptr) {
            if (current->keyValue.first == key) {
                if (prev == nullptr) {
                    table[h] = current->next; //удаляем первый элемент цепочки
                }
                else {
                    prev->next = current->next; //из середины или конца
                }
                delete current;
                size--;
                return;
            }
            prev = current;
            current = current->next;
        }
    }

    pair<bool, int> contains(int key) override {//поиск - возвращает (найдено ли, значение)
        int h = hash(key);
        Node* current = table[h];//начало цепочки

        while (current != nullptr) {//проходим по цепочке
            if (current->keyValue.first == key) {
                return make_pair(true, current->keyValue.second);//нашли - возвращаем значение
            }
            current = current->next;
        }
        return make_pair(false, -1); // не найдено
    }

    string toString() override {
        string result;
        for (int i = 0; i < capacity; i++) {
            result += "[" + std::to_string(i) + "]: ";
            Node* current = table[i];
            while (current != nullptr) {
                result += "(" + std::to_string(current->keyValue.first) + "," + 
                         std::to_string(current->keyValue.second) + ") -> ";
                current = current->next;
            }
            result += "null\n";
        }
        return result;
    }
    // Для анализа длины цепочек
    void getChainLengths(int& minLength, int& maxLength, double& avgLength) {
        minLength = INT_MAX;
        maxLength = 0;
        int totalLength = 0;
        int nonEmptyChains = 0;

        for (int i = 0; i < capacity; i++) {
            int length = 0;
            Node* current = table[i];
            while (current != nullptr) {
                length++;
                current = current->next;
            }

            if (length > 0) {
                minLength = std::min(minLength, length);
                maxLength = std::max(maxLength, length);
                totalLength += length;
                nonEmptyChains++;
            }
        }

        if (nonEmptyChains == 0) {
            minLength = 0;
            avgLength = 0;
        }
        else {
            avgLength = static_cast<double>(totalLength) / nonEmptyChains;
        }
    }
};

// Открытая адресация
class OpenAddressingHashTable : public HashTable {
private:
    vector<pair<int, int>> table; // храним пары ключ-значение
    vector<bool> occupied; //флаги занятости ячеек
    vector<bool> deleted; //флаг, что ячейка удалялась

    int hash(int key, int attempt) {//метод линейного хэширования
        return (key % capacity + attempt) % capacity;
    }

public:
    OpenAddressingHashTable(int capacity) : HashTable(capacity) {
        table.resize(capacity, make_pair(-1, -1));
        occupied.resize(capacity, false);
        deleted.resize(capacity, false);
    }

    void add(pair<int, int> keyValue) override {
        for (int attempt = 0; attempt < capacity; attempt++) {
            int h = hash(keyValue.first, attempt);
            if (!occupied[h] || deleted[h]) {  
                table[h] = keyValue;
                occupied[h] = true;
                deleted[h] = false;
                size++;
                return;
            }
            // Если ячейка занята тем же ключом - это дубликат
            if (occupied[h] && !deleted[h] && table[h].first == keyValue.first) {
                return; // дубликат - не увеличиваем size
            }
        }
        // Таблица заполнена
        throw std::runtime_error("Hash table is full");
    }

    void remove(int key) override {
        for (int attempt = 0; attempt < capacity; attempt++) {
            int h = hash(key, attempt);
            if (!occupied[h] && !deleted[h]) {
                return; // Элемент не найден
            }
            if (occupied[h] && !deleted[h] && table[h].first == key) {
                deleted[h] = true;
                size--;
                return;
            }
        }
    }

    pair<bool, int> contains(int key) override { //поиск элементов - возвращает (найдено ли, значение)
        for (int attempt = 0; attempt < capacity; attempt++) {
            int h = hash(key, attempt);
            if (!occupied[h] && !deleted[h]) {
                return make_pair(false, -1); //дошли до пустой ячейки - элемента нет
            }
            if (occupied[h] && !deleted[h] && table[h].first == key) {
                return make_pair(true, table[h].second); // возвращаем значение
            }
        }
        return make_pair(false, -1);
    }

    string toString() override { //для визуального вывода таблицы
        string result;
        for (int i = 0; i < capacity; i++) {
            result += "[" + to_string(i) + "]: ";
            if (occupied[i] && !deleted[i]) {
                result += "(" + to_string(table[i].first) + "," + to_string(table[i].second) + ")";
            } else if (deleted[i]) {
                result += "deleted";
            } else {
                result += "empty";
            }
            result += "\n";
        }
        return result;
    }
};

// Генератор случайных чисел
class RandomGenerator {
private:
    mt19937 gen;//генератор
    uniform_int_distribution<int> dist; //равномерное распределение

public:
    RandomGenerator(int min = 0, int max = 10000)
        : gen(std::chrono::steady_clock::now().time_since_epoch().count()), dist(min, max) {
    }

    int generate() {
        return dist(gen);
    }

    vector<int> generateSequence(int n) {
        vector<int> sequence;
        for (int i = 0; i < n; i++) {
            sequence.push_back(generate());
        }
        return sequence;
    }

    // Генерация пар ключ-значение
    vector<pair<int, int>> generateKeyValuePairs(int n) {
        vector<pair<int, int>> pairs;
        for (int i = 0; i < n; i++) {
            pairs.push_back(make_pair(generate(), generate()));
        }
        return pairs;
    }
};

// Функции для выполнения заданий
void task1() {
    cout << "ПУНКТ 1: Эмпирический анализ методов хеширования\n";
    const int N = 100;  // Количество элементов
    const int M = 100;   // Количество поисков
    const int capacity = N / 10; // Размер таблицы

    RandomGenerator rng;
    auto pairs = rng.generateKeyValuePairs(N); // генерируем пары ключ-значение
    vector<int> searchKeys;
    for (const auto& p : pairs) {
        searchKeys.push_back(p.first);
    }

    cout << "Параметры теста" << endl;
    cout << "N (элементов): " << N << endl;
    cout << "M (поисков): " << M << endl;
    cout << "Емкость таблицы: " << capacity << endl;

    cout << "МЕТОД ЦЕПОЧЕК:\n";
    auto start = chrono::high_resolution_clock::now(); //начинаем отсчёт времени

    ChainingHashTable chainTable(capacity); //создаём таблицу
    for (const auto& pair : pairs) {//заполняем парами
        chainTable.add(pair);
    }

    auto insertEnd = chrono::high_resolution_clock::now();

    int foundCount = 0; // измерение времени поиска
    int totalValue = 0;
    for (int key : searchKeys) {
        auto result = chainTable.contains(key);
        if (result.first) {
            foundCount++;
            totalValue += result.second; // используем значение для сравнения
        }
    }

    auto searchEnd = chrono::high_resolution_clock::now();

    auto insertTime = chrono::duration_cast<chrono::microseconds>(insertEnd - start);
    auto searchTime = chrono::duration_cast<chrono::microseconds>(searchEnd - insertEnd);

    cout << "Время вставки " << N << " элементов: " << insertTime.count() << " мкс\n";
    cout << "Время выполнения " << M << " поисков: " << searchTime.count() << " мкс\n";
    cout << "Найдено элементов: " << foundCount << "/" << M << "\n";
    cout << "Сумма найденных значений: " << totalValue << "\n";

    int minLen, maxLen;
    double avgLen;
    chainTable.getChainLengths(minLen, maxLen, avgLen);
    cout << "Длина цепочек - мин: " << minLen << ", макс: " << maxLen
        << ", средняя: " << std::fixed << std::setprecision(2) << avgLen << "\n\n";

    // Тестирование открытой адресации
    cout << "ОТКРЫТАЯ АДРЕСАЦИЯ:\n";
    const int capacityForOpen = N*2; // так как в открытой адресации можем разместить столько элементов, сколько места в таблице
    cout << "Емкость таблицы (меняем так как в этом методе нельзя вставить больше элементов): " << capacityForOpen << endl;
    start = chrono::high_resolution_clock::now(); //перезаписываем время начала 

    OpenAddressingHashTable openTable(capacityForOpen);
    for (const auto& pair : pairs) {
        openTable.add(pair);
    }

    insertEnd = chrono::high_resolution_clock::now();

    foundCount = 0;
    totalValue = 0;
    for (int key : searchKeys) {
        auto result = openTable.contains(key);
        if (result.first) {
            foundCount++;
            totalValue += result.second; // используем значение для сравнения
        }
    }

    searchEnd = chrono::high_resolution_clock::now();

    insertTime = chrono::duration_cast<chrono::microseconds>(insertEnd - start);
    searchTime = chrono::duration_cast<chrono::microseconds>(searchEnd - insertEnd);

    cout << "Время вставки " << N << " элементов: " << insertTime.count() << " мкс\n";
    cout << "Время выполнения " << M << " поисков: " << searchTime.count() << " мкс\n";
    cout << "Найдено элементов: " << foundCount << "/" << M << "\n";
    cout << "Сумма найденных значений: " << totalValue << "\n";
}

void task2() {
    cout << "ПУНКТ 2: Анализ длины цепочек при различных N\n";
    vector<int> testSizes = { 5000, 10000, 20000 }; // уменьшил для скорости
    RandomGenerator rng;

    cout << setw(8) << "N" << setw(12) << "Емкость"
        << setw(10) << "Мин" << setw(10) << "Макс" 
        << setw(15) << "Время вставки" << setw(15) << "Сумма значений\n";
    cout << string(85, '-') << "\n";

    for (int N : testSizes) { //цикл тестирование вставки
        int capacity = N / 10; //размер таблицы
        if (capacity < 1) capacity = 1;

        auto pairs = rng.generateKeyValuePairs(N);//N случайных пар генерируем
        vector<int> searchKeys;
        for (const auto& p : pairs) {
            searchKeys.push_back(p.first);
        }

        auto start = std::chrono::high_resolution_clock::now();

        ChainingHashTable table(capacity);
        for (const auto& pair : pairs) {//добавляем пары
            table.add(pair);
        }

        auto end = chrono::high_resolution_clock::now();
        auto time = chrono::duration_cast<std::chrono::microseconds>(end - start);

        // Подсчет суммы значений для демонстрации работы с значениями
        int totalValue = 0;
        for (int key : searchKeys) {
            auto result = table.contains(key);
            if (result.first) {
                totalValue += result.second;
            }
        }

        int minLen, maxLen;//мин и макс длины цепочек
        double avgLen; //средняя длина непустых цепочек
        table.getChainLengths(minLen, maxLen, avgLen);

        cout << setw(8) << N
            << setw(12) << capacity
            << setw(10) << minLen
            << setw(10) << maxLen
            << setw(10) << time.count() << " мкс"
            << setw(15) << totalValue << "\n";
    }
}

int main() {
    setlocale(LC_ALL, "RU");
    try {
        task1();
        cout << "\n\n";
        task2();
    }
    catch (const std::exception& e) {
        std::cerr << "Ошибка: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
