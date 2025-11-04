#include <iostream>
#include <vector>
#include <stdexcept>
#include <unordered_map>
#include <sstream>
#include <algorithm>
#include <string> 
#include <cctype>

using namespace std;

struct SNode {
    string data;
    SNode* next;
};

struct Stack {
    SNode* top;
    size_t size;
};

// Функции стека
void SINIT(Stack& stack) {
    stack.top = nullptr;
    stack.size = 0;
}

void SPUSH(Stack& stack, const string& value) {
    SNode* newNode = new SNode{ value, stack.top };
    stack.top = newNode;
    stack.size++;
}

string SPOP(Stack& stack) {
    if (stack.top == nullptr) {
        throw runtime_error("Ошибка: стек пуст!");
    }
    SNode* temp = stack.top;
    string value = temp->data;
    stack.top = stack.top->next;
    delete temp;
    stack.size--;
    return value;
}

bool ISEMPTY(const Stack& stack) {
    return stack.top == nullptr;
}

void SPRINT(const Stack& stack) {
    if (stack.top == nullptr) {
        cout << "Стек пуст!" << endl;
        return;
    }
    cout << "Порядок выполнения: ";
    SNode* current = stack.top;
    while (current != nullptr) {
        cout << current->data;
        if (current->next != nullptr) {
            cout << " -> ";
        }
        current = current->next;
    }
    cout << endl;
}

// Функция для ввода задач
vector<string> inputTasks() {
    vector<string> tasks;
    string input;

    cout << "Введите задачи через запятую (например: A,B,C): ";
    getline(cin, input);

    // Удаляем пробелы
    input.erase(remove(input.begin(), input.end(), ' '), input.end());

    if (input.empty()) {
        throw runtime_error("Ошибка: список задач не может быть пустым!");
    }

    stringstream ss(input); //поток строк, который позволяет работать с строкой как с потоком ввода
    string task;

    while (getline(ss, task, ',')) {
        if (task.empty()) {
            throw runtime_error("Ошибка: обнаружена пустая задача!");
        }

        // Проверяем на дубликаты
        if (find(tasks.begin(), tasks.end(), task) != tasks.end()) {
            throw runtime_error("Ошибка: задача '" + task + "' уже существует!");
        }

        tasks.push_back(task);
    }
    return tasks;
}

// Функция для ввода зависимостей
vector<pair<string, string>> inputDependencies(const vector<string>& tasks) {
    vector<pair<string, string>> dependencies;
    string input;

    cout << "Введите зависимости через запятую (например: A-B, B-C): ";
    cout << "Для отсутствия зависимостей введите пустую строку" << endl;
    getline(cin, input);

    if (input.empty()) {
        return dependencies;
    }

    stringstream ss(input);
    string dependencyPair;

    while (getline(ss, dependencyPair, ',')) {
        // Удаляем пробелы
        dependencyPair.erase(remove(dependencyPair.begin(), dependencyPair.end(), ' '), dependencyPair.end());
        if (dependencyPair.empty()) {
            continue;
        }
        // Ищем разделитель
        size_t separator = dependencyPair.find('-');
        if (separator == string::npos) {
            throw runtime_error("Ошибка: неправильный формат зависимости '" + dependencyPair + "'. Используйте формат 'A-B'");
        }

        string dependent = dependencyPair.substr(0, separator);
        string prerequisite = dependencyPair.substr(separator + 1);

        if (dependent.empty() || prerequisite.empty()) {
            throw runtime_error("Ошибка: неправильный формат зависимости '" + dependencyPair + "'");
        }

        // Проверяем существование задач
        if (find(tasks.begin(), tasks.end(), dependent) == tasks.end()) {
            throw runtime_error("Ошибка: задача '" + dependent + "' не найдена в списке задач!");
        }

        if (find(tasks.begin(), tasks.end(), prerequisite) == tasks.end()) {
            throw runtime_error("Ошибка: задача '" + prerequisite + "' не найдена в списке задач!");
        }

        // Проверяем самозависимость
        if (dependent == prerequisite) {
            throw runtime_error("Ошибка: задача '" + dependent + "' не может зависеть от самой себя!");
        }

        dependencies.push_back({ dependent, prerequisite });
    }

    return dependencies;
}

// Основная функция проверки зависимостей
bool canCompleteAllTasks(const vector<string>& tasks, const vector<pair<string, string>>& dependencies) {
    // Считаем зависимости для каждой задачи
    unordered_map<string, int> dependencyCount;

    // Инициализируем счетчики
    for (const auto& task : tasks) {
        dependencyCount[task] = 0;
    }

    // Заполняем счетчики зависимостей
    for (const auto& dep : dependencies) {
        dependencyCount[dep.first]++; // Увеличиваем счетчик для задачи, которая зависит от другой
    }

    // Стек для задач без зависимостей
    Stack zeroDependencyStack;
    SINIT(zeroDependencyStack);

    // Стек для порядка выполнения
    Stack executionOrder;
    SINIT(executionOrder);

    // Добавляем в стек задачи без зависимостей
    for (const auto& task : tasks) {
        if (dependencyCount[task] == 0) {
            SPUSH(zeroDependencyStack, task);
        }
    }

    int processedCount = 0;

    // Обрабатываем задачи из стека
    while (!ISEMPTY(zeroDependencyStack)) {
        string current = SPOP(zeroDependencyStack);
        SPUSH(executionOrder, current);
        processedCount++;

        // Уменьшаем счетчики зависимостей для всех задач, которые зависели от текущей
        for (const auto& dep : dependencies) {
            if (dep.second == current) { // Если текущая задача - это та, от которой кто-то зависит
                dependencyCount[dep.first]--; // Уменьшаем счетчик зависимости

                // Если у задачи больше нет зависимостей, добавляем в стек
                if (dependencyCount[dep.first] == 0) {
                    SPUSH(zeroDependencyStack, dep.first);
                }
            }
        }
    }

    // Выводим порядок выполнения (в правильном порядке)
    if (processedCount == tasks.size()) {
        Stack correctOrder;
        SINIT(correctOrder);

        // Перекладываем чтобы показать правильный порядок (от первой к последней)
        while (!ISEMPTY(executionOrder)) {
            SPUSH(correctOrder, SPOP(executionOrder));
        }
        SPRINT(correctOrder);
    }

    // Если обработали все задачи - цикла нет
    return processedCount == tasks.size();
}

int main() {
    setlocale(LC_ALL, "RU");
    try {
        // Ввод задач
        vector<string> tasks = inputTasks();
        cout << "Задачи: ";
        for (size_t i = 0; i < tasks.size(); i++) {
            cout << tasks[i];
            if (i < tasks.size() - 1) cout << ", ";
        }
        cout << endl;

        // Ввод зависимостей
        vector<pair<string, string>> dependencies = inputDependencies(tasks);

        if (!dependencies.empty()) {
            cout << "Зависимости: ";
            for (size_t i = 0; i < dependencies.size(); i++) {
                cout << dependencies[i].first << " зависит от " << dependencies[i].second;
                if (i < dependencies.size() - 1) cout << ", ";
            }
            cout << endl;
        }
        else {
            cout << "Зависимости: нет" << endl;
        }

        if (canCompleteAllTasks(tasks, dependencies)) {
            cout << "ВОЗМОЖНО выполнить все задачи!" << endl;
        }
        else {
            cout << "НЕВОЗМОЖНО выполнить все задачи! Обнаружена циклическая зависимость." << endl;
        }

    }
    catch (const exception& e) {
        cout << "Ошибка ввода: " << e.what() << endl;
        cout << "Пожалуйста, запустите программу заново и введите данные корректно." << endl;
    }

    return 0;
}