#include <iostream>
#include <string>
#include <cctype>
#include <vector>

using namespace std;

// Узел хеш-таблицы
struct HashNode {
    string key;
    HashNode* next;

    HashNode(const string& k) : key(k), next(nullptr) {}
};

// Простая хеш-функция для строк
size_t stringHash(const string& str, size_t tableSize) {
    size_t hash = 5381; //метод djb2 с этим числом
    for (char c : str) {
        hash = ((hash << 5) + hash) + c; // hash * 33 + c
    }
    return hash % tableSize;
}

// Собственная реализация множества на основе хеш-таблицы
class CustomSet {
private:
    vector<HashNode*> table;
    size_t capacity;
    size_t size_;

    // Рехеширование при необходимости (уменьшает коллизии)
    void rehash() {
        size_t newCapacity = capacity * 2;
        vector<HashNode*> newTable(newCapacity, nullptr);

        // Перехеширование всех элементов
        for (size_t i = 0; i < capacity; ++i) {
            HashNode* node = table[i];
            while (node != nullptr) {
                HashNode* next = node->next;
                size_t newIndex = stringHash(node->key, newCapacity);

                // Вставка в новую таблицу в начало цепочки
                node->next = newTable[newIndex]; //привязываем к существующей цепочке
                newTable[newIndex] = node;//делаем ноду началом цеочки

                node = next;
            }
        }

        table = move(newTable); //передаём данные владения без копирования
        capacity = newCapacity;
    }

public:
    CustomSet(size_t initialCapacity = 16) : capacity(initialCapacity), size_(0) {
        table.resize(capacity, nullptr);
    }

    ~CustomSet() {
        clear();
    }

    // Очистка множества
    void clear() {
        for (size_t i = 0; i < capacity; ++i) {
            HashNode* node = table[i];
            while (node != nullptr) {
                HashNode* next = node->next;
                delete node;
                node = next;
            }
            table[i] = nullptr;
        }
        size_ = 0;
    }

    // Вставка элемента
    bool insert(const string& key) {
        // Рехеширование при коэффициенте загрузки > 0.75
        if (size_ >= capacity * 0.75) {
            rehash();
        }

        size_t index = stringHash(key, capacity);
        HashNode* node = table[index];

        // Проверка на существование элемента
        while (node != nullptr) {
            if (node->key == key) {
                return false; // Элемент уже существует
            }
            node = node->next;
        }

        // Вставка нового элемента в начало цепочки
        HashNode* newNode = new HashNode(key);
        newNode->next = table[index];
        table[index] = newNode;
        size_++;

        return true;
    }

    // Удаление элемента
    bool erase(const string& key) {
        size_t index = stringHash(key, capacity);
        HashNode* node = table[index];
        HashNode* prev = nullptr;

        while (node != nullptr) {
            if (node->key == key) {
                if (prev == nullptr) {
                    // Удаление из начала цепочки
                    table[index] = node->next;
                }
                else {
                    // Удаление из середины/конца цепочки
                    prev->next = node->next;
                }
                delete node;
                size_--;
                return true;
            }
            prev = node;
            node = node->next;
        }

        return false; // Элемент не найден
    }

    // Поиск элемента
    bool find(const string& key) const {
        size_t index = stringHash(key, capacity);
        HashNode* node = table[index];

        while (node != nullptr) {
            if (node->key == key) {
                return true;
            }
            node = node->next;
        }

        return false;
    }

    // Получение размера множества
    size_t size() const {
        return size_;
    }

    // Получение всех элементов (для сохранения в файл)
    vector<string> getAllElements() const {
        vector<string> elements;
        for (size_t i = 0; i < capacity; ++i) {
            HashNode* node = table[i];
            while (node != nullptr) {
                elements.push_back(node->key);
                node = node->next;
            }
        }
        return elements;
    }
};

class SetManager {
private:
    CustomSet data;

public:
    SetManager() = default;

    // SETADD - добавление элемента
    void SETADD(const string& element) {
        auto result = data.insert(element);
    }

    // Дополнительный метод для проверки наличия элемента (без вывода)
    bool contains(const string& element) const {
        return data.find(element);
    }

    // Метод для очистки множества
    void clear() {
        data.clear();
    }

    // Метод для добавления нескольких элементов
    void addAll(const CustomSet& elements) {
        vector<string> allElements = elements.getAllElements();
        for (const auto& element : allElements) {
            data.insert(element);
        }
    }
};

// Функция для получения всех пар соседних оснований из строки
CustomSet getAllPairs(const string& genome) {
    CustomSet pairs;

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
    CustomSet pairs2 = getAllPairs(genome2);

    // Добавляем все пары в SetManager
    vector<string> allPairs = pairs2.getAllElements();
    for (const auto& pair : allPairs) {
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
