#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
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
    string filename;

public:
    SetManager(const string& file) : filename(file) {
        loadFromFile();
    }

    // Загрузка данных из файла
    void loadFromFile() {
        fstream file(filename);
        if (!file.is_open()) {
            cerr << "Ошибка: Не удалось открыть файл " << filename << std::endl;
            return;
        }

        string line;
        while (getline(file, line)) {
            if (!line.empty()) {
                data.insert(line);
            }
        }
        file.close();
    }

    // Сохранение данных в файл
    void saveToFile() {
        ofstream file(filename);
        if (!file.is_open()) {
            cerr << "Ошибка: Не удалось открыть файл для записи " << filename << endl;
            return;
        }

        vector<string> elements = data.getAllElements();
        for (const auto& element : elements) {
            file << element << "\n";
        }
        file.close();
    }

    // SETADD - добавление элемента
    void SETADD(const std::string& element) {
        if (data.insert(element)) {
            cout << "Элемент '" << element << "' успешно добавлен в множество" << endl;
        }
        else {
            cout << "Элемент '" << element << "' уже существует в множестве" << endl;
        }
        saveToFile();
    }

    // SETDEL - удаление элемента
    void SETDEL(const std::string& element) {
        if (data.erase(element)) {
            cout << "Элемент '" << element << "' успешно удален из множества" << endl;
        }
        else {
            cout << "Элемент '" << element << "' не найден в множестве" << endl;
        }
        saveToFile();
    }

    // SET_AT - проверка наличия элемента
    void SET_AT(const string& element) {
        if (data.find(element)) {
            cout << "Элемент '" << element << "' присутствует в множестве" << endl;
        }
        else {
            cout << "Элемент '" << element << "' отсутствует в множестве" << endl;
        }
    }
};

// Функция для разбора аргументов командной строки
void parseArguments(int argc, char* argv[], string& filename, string& query) {
    for (int i = 1; i < argc; ++i) {
        string arg = argv[i];
        if (arg == "--file" && i + 1 < argc) {
            filename = argv[++i]; //следующий аргумент имя файла
        }
        else if (arg == "--query" && i + 1 < argc) {
            query = argv[++i]; //следующий аргумент запрос
        }
    }
}

// Функция для разбора запроса
void processQuery(SetManager& manager, const string& query) {
    istringstream iss(query);//преобразование строки в поток для рабора
    string operation, element;

    if (!(iss >> operation)) { //извлекаем первое слово - операцию
        cerr << "Ошибка: Неверный формат запроса" << endl;
        return;
    }

    // Читаем оставшуюся часть как элемент (может содержать пробелы)
    getline(iss, element);

    // Удаляем начальные пробелы
    size_t start = element.find_first_not_of(" \t");
    if (start != string::npos) {
        element = element.substr(start);
    }

    if (operation == "SETADD") {
        if (element.empty()) {
            cerr << "Ошибка: Для операции SETADD требуется элемент" << endl;
        }
        else {
            manager.SETADD(element);
        }
    }
    else if (operation == "SETDEL") {
        if (element.empty()) {
            cerr << "Ошибка: Для операции SETDEL требуется элемент" << endl;
        }
        else {
            manager.SETDEL(element);
        }
    }
    else if (operation == "SET_AT") {
        if (element.empty()) {
            cerr << "Ошибка: Для операции SET_AT требуется элемент" << endl;
        }
        else {
            manager.SET_AT(element);
        }
    }
    else {
        cerr << "Ошибка: Неизвестная операция '" << operation << "'" << endl;
        cerr << "Доступные операции: SETADD, SETDEL, SET_AT" << endl;
    }
}

int main(int argc, char* argv[]) {
    setlocale(LC_ALL, "RU");
    //количество аргументов и массив аргументов
    if (argc < 5) {
        cerr << "Использование: " << argv[0] << " --file <путь_до_файла> --query <запрос>" << endl;
        cerr << "Примеры запросов:" << endl;
        cerr << "  SETADD элемент" << endl;
        cerr << "  SETDEL элемент" << endl;
        cerr << "  SET_AT элемент" << endl;
        return 1;
    }

    string filename, query;
    parseArguments(argc, argv, filename, query);

    if (filename.empty() || query.empty()) {
        cerr << "Ошибка: Не указаны файл или запрос" << endl;
        return 1;
    }

    try {
        SetManager manager(filename);
        processQuery(manager, query);
    }
    catch (const exception& e) {
        cerr << "Ошибка: " << e.what() << endl;
        return 1;
    }

    return 0;
}