#include <iostream>
#include <fstream>
#include <string>
#include <unordered_set>
#include <vector>
#include <sstream>
using namespace std;
class SetManager {
private:
    unordered_set<string> data;
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

        for (const auto& element : data) {
            file << element << "\n";
        }
        file.close();
    }

    // SETADD - добавление элемента
    void SETADD(const std::string& element) {
        auto result = data.insert(element);
        if (result.second) {
            cout << "Элемент '" << element << "' успешно добавлен в множество" << endl;
        } else {
            cout << "Элемент '" << element << "' уже существует в множестве" << endl;
        }
        saveToFile();
    }

    // SETDEL - удаление элемента
    void SETDEL(const std::string& element) {
        if (data.erase(element) > 0) {
            cout << "Элемент '" << element << "' успешно удален из множества" << endl;
        } else {
            cout << "Элемент '" << element << "' не найден в множестве" << endl;
        }
        saveToFile();
    }

    // SET_AT - проверка наличия элемента
    void SET_AT(const string& element) {
        if (data.find(element) != data.end()) {
            cout << "Элемент '" << element << "' присутствует в множестве" << endl;
        } else {
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
        } else if (arg == "--query" && i + 1 < argc) {
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
        } else {
            manager.SETADD(element);
        }
    } else if (operation == "SETDEL") {
        if (element.empty()) {
            cerr << "Ошибка: Для операции SETDEL требуется элемент" << endl;
        } else {
            manager.SETDEL(element);
        }
    } else if (operation == "SET_AT") {
        if (element.empty()) {
            cerr << "Ошибка: Для операции SET_AT требуется элемент" << endl;
        } else {
            manager.SET_AT(element);
        }
    } else {
        cerr << "Ошибка: Неизвестная операция '" << operation << "'" << endl;
        cerr << "Доступные операции: SETADD, SETDEL, SET_AT" << endl;
    }
}

int main(int argc, char* argv[]) {
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
    } catch (const exception& e) {
        cerr << "Ошибка: " << e.what() << endl;
        return 1;
    }

    return 0;
}