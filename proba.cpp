#include <cstdint>
#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <stdexcept>
#include <map>
#include <algorithm>
#include <queue>
#include <functional>

using namespace std;

struct HuffmanNode {
    wchar_t symbol;
    int frequency;
    HuffmanNode* left;
    HuffmanNode* right;

    HuffmanNode(wchar_t s, int freq) : symbol(s), frequency(freq), left(nullptr), right(nullptr) {}
    HuffmanNode(int freq, HuffmanNode* l, HuffmanNode* r) : symbol(L'\0'), frequency(freq), left(l), right(r) {}
};

struct QNode {
    HuffmanNode* data;
    QNode* next;
};

struct PriorityQueue { //моя собственная, работает не как все очереди
    QNode* front;
    QNode* rear;
    size_t sz;

    PriorityQueue() : front(nullptr), rear(nullptr), sz(0) {}
};

// Функция для подсчета частот символов
map<wchar_t, int> howOften(const wstring& input) {
    map<wchar_t, int> result;
    for (wchar_t elem : input) {
        result[elem]++;
    }
    return result;
}

// Добавление в конец
void QPUSH(PriorityQueue& queue, HuffmanNode* value) {
    QNode* newNode = new QNode{ value, nullptr };

    if (queue.rear == nullptr) {
        queue.front = newNode;
        queue.rear = newNode;
    }
    else {
        queue.rear->next = newNode;
        queue.rear = newNode;
    }
    queue.sz++;
}

// Удаление из начала 
void QPOP(PriorityQueue& queue) { // передача по ссылке
    if (queue.front == nullptr) {
        throw runtime_error("Ошибка: очередь пуста!");
    }

    QNode* temp = queue.front;
    queue.front = queue.front->next;

    if (queue.front == nullptr) {
        queue.rear = nullptr;
    }

    delete temp;
    queue.sz--;
}

// Получение минимального элемента (из начала)
HuffmanNode* QFRONT(const PriorityQueue& queue) {
    if (queue.front == nullptr) {
        throw runtime_error("Ошибка: очередь пуста!");
    }
    return queue.front->data;
}

// Вывод очереди для моего просмотра
void QPRINT(const PriorityQueue& queue) {
    if (queue.front == nullptr) {
        cout << "Очередь пуста!" << endl;
        return;
    }

    cout << "Очередь [" << queue.sz << "] (min->max): ";
    QNode* current = queue.front;
    while (current != nullptr) {
        wcout << L"'";
        if (current->data->symbol == L' ') wcout << L"ПРОБЕЛ";
        else if (current->data->symbol == L'\n') wcout << L"\\n";
        else if (current->data->symbol == L'\0') wcout << L"ВНУТР";
        else wcout << current->data->symbol;
        wcout << L"'(" << current->data->frequency << L")";
        if (current->next != nullptr) cout << " -> ";
        current = current->next;
    }
    cout << " -> NULL" << endl;
}

 //Сортировка очереди по возрастанию частот(min в начале)
void sortQueue(PriorityQueue& queue) {
    if (queue.sz <= 1) return;

    // Собираем все узлы в вектор
    vector<HuffmanNode*> nodes;
    QNode* current = queue.front;
    while (current != nullptr) {
        nodes.push_back(current->data);
        current = current->next;
    }

    // Сортируем по возрастанию частот
    sort(nodes.begin(), nodes.end(), [](HuffmanNode* a, HuffmanNode* b) {
        return a->frequency < b->frequency;
        });

    // Очищаем очередь
    while (queue.front != nullptr) {
        QPOP(queue);
    }

    // Заполняем отсортированными элементами
    for (HuffmanNode* node : nodes) {
        QPUSH(queue, node);
    }
}

// Создание отсортированной очереди
PriorityQueue createList(const map<wchar_t, int>& frequency) {
    PriorityQueue freeNodes;

    for (const auto& elem : frequency) {
        HuffmanNode* newNode = new HuffmanNode(elem.first, elem.second);
        QPUSH(freeNodes, newNode);
    }

    // Сортируем по возрастанию (min в начале)
    sortQueue(freeNodes);
    return freeNodes;
}

// Построение дерева Хаффмана (исправлено)
HuffmanNode* buildHuffmanTree(PriorityQueue& freeNodes) { // передача по ссылке
    //int step = 1;
    //cout << "\n=== ПОСТРОЕНИЕ ДЕРЕВА ===" << endl;
    //QPRINT(freeNodes);

    while (freeNodes.sz > 1) {
       // cout << "\n=== ШАГ " << step << " ===" << endl;
        HuffmanNode* left = QFRONT(freeNodes);
        QPOP(freeNodes);
        HuffmanNode* right = QFRONT(freeNodes);
        QPOP(freeNodes);
        //wcout << L"Объединяем: '" << left->symbol << L"' и '" << right->symbol << L"'" << endl;
        int parentFreq = left->frequency + right->frequency;
        HuffmanNode* parent = new HuffmanNode(parentFreq, left, right);
        QPUSH(freeNodes, parent);
        sortQueue(freeNodes);

        //cout << "После шага " << step << ":" << endl;
        //QPRINT(freeNodes);
        //step++;
    }

    return QFRONT(freeNodes);
}
// Рекурсивная функция для красивого вывода дерева Хаффмана
void printHuffmanTree(HuffmanNode * node, const string & prefix = "", bool isLeft = false) {
    if (node == nullptr) return;

    cout << prefix;
    cout << (isLeft ? "|--" : "|__");

    // Выводим информацию об узле
    if (node->symbol == L'\0') {
        // Внутренний узел
        cout << "[" << node->frequency << "]";
    }
    else {
        // Листовой узел
        wcout << L"'";
        if (node->symbol == L' ') {
            wcout << L"ПРОБЕЛ";
        }
        else if (node->symbol == L'\n') {
            wcout << L"\\n";
        }
        else {
            wcout << node->symbol;
        }
        wcout << L"' (" << node->frequency << ")";
    }
    cout << endl;

    // Рекурсивно выводим левое и правое поддеревья
    printHuffmanTree(node->left, prefix + (isLeft ? "|   " : "    "), true);
    printHuffmanTree(node->right, prefix + (isLeft ? "|   " : "    "), false);
}
// Функция для генерации кодов Хаффмана
void generateHuffmanCodes(HuffmanNode* root, const string& code = "", map<wchar_t, string>& codes = *new map<wchar_t, string>) {
    if (root == nullptr) return;

    // Если это листовой узел
    if (root->symbol != L'\0') {
        codes[root->symbol] = code;
    }

    // Рекурсивно обходим левое и правое поддеревья
    generateHuffmanCodes(root->left, code + "0", codes);
    generateHuffmanCodes(root->right, code + "1", codes);
}

// Функция для вывода кодов Хаффмана
void printHuffmanCodes(const map<wchar_t, string>& codes) {
    wcout << L"\n=== КОДЫ ХАФФМАНА ===" << endl;
    for (const auto& pair : codes) {
        wcout << L"  '";
        if (pair.first == L' ') {
            wcout << L"ПРОБЕЛ";
        }
        else if (pair.first == L'\n') {
            wcout << L"\\n";
        }
        else {
            wcout << pair.first;
        }
        wcout << L"': ";
        // Преобразуем string в wstring для вывода через wcout
        wstring codeStr(pair.second.begin(), pair.second.end());
        wcout << codeStr << endl;
    }
}

void cleanupHuffmanTree(HuffmanNode* root) {
    if (root == nullptr) return;
    cleanupHuffmanTree(root->left);
    cleanupHuffmanTree(root->right);
    delete root;
}

int main() {
    setlocale(LC_ALL, "ru");

    cout << "Введите исходную строку: ";
    wstring input;
    getline(wcin, input);

    map<wchar_t, int> frequency = howOften(input);
    PriorityQueue freeNodes = createList(frequency);
    HuffmanNode* huffmanTree = buildHuffmanTree(freeNodes);

    cout << "\n" << string(50, '=') << endl;
    cout << "ИТОГОВОЕ ДЕРЕВО ХАФФМАНА" << endl;
    cout << string(50, '=') << endl;
    printHuffmanTree(huffmanTree);

    map<wchar_t, string> huffmanCodes;
    generateHuffmanCodes(huffmanTree, "", huffmanCodes);
    printHuffmanCodes(huffmanCodes);

    cleanupHuffmanTree(huffmanTree);
    return 0;
}