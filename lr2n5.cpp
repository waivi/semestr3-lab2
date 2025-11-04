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
    char symbol;
    int frequency;
    HuffmanNode* left;
    HuffmanNode* right;

    HuffmanNode(char s, int freq) : symbol(s), frequency(freq), left(nullptr), right(nullptr) {}
    HuffmanNode(int freq, HuffmanNode* l, HuffmanNode* r) : symbol('\0'), frequency(freq), left(l), right(r) {}
};

// Компаратор для приоритетной очереди (минимальная частота first)
struct CompareNodes {
    bool operator()(HuffmanNode* a, HuffmanNode* b) {
        if (a->frequency == b->frequency) {
            // При одинаковых частотах сортируем по символу
            return a->symbol > b->symbol;
        }
        return a->frequency > b->frequency;
    }
};

// Функция для подсчета частот символов
map<char, int> howOften(const string& input) {
    map<char, int> result;
    for (char elem : input) {
        result[elem]++;
    }
    return result;
}

// Функция для создания списка свободных узлов (приоритетная очередь)
priority_queue<HuffmanNode*, vector<HuffmanNode*>, CompareNodes>
createFreeNodesList(const map<char, int>& frequency) {
    priority_queue<HuffmanNode*, vector<HuffmanNode*>, CompareNodes> freeNodes;

    for (const auto& pair : frequency) {
        HuffmanNode* newNode = new HuffmanNode(pair.first, pair.second);
        freeNodes.push(newNode);
    }
    return freeNodes;
}

// Рекурсивная функция для красивого вывода дерева Хаффмана
void printHuffmanTree(HuffmanNode* node, const string& prefix = "", bool isLeft = false) {
    if (node == nullptr) return;

    cout << prefix;
    cout << (isLeft ? "|--" : "|__");

    // Выводим информацию об узле
    if (node->symbol == '\0') {
        // Внутренний узел
        cout << "[" << node->frequency << "]";
    }
    else {
        // Листовой узел
        cout << "'" << node->symbol << "' (" << node->frequency << ")";
    }
    cout << endl;

    // Рекурсивно выводим левое и правое поддеревья
    printHuffmanTree(node->left, prefix + (isLeft ? "|   " : "    "), true);
    printHuffmanTree(node->right, prefix + (isLeft ? "|   " : "    "), false);
}

// Функция для построения дерева Хаффмана
HuffmanNode* buildHuffmanTree(priority_queue<HuffmanNode*, vector<HuffmanNode*>, CompareNodes>& freeNodes) {
    while (freeNodes.size() > 1) {
        HuffmanNode* left = freeNodes.top();
        freeNodes.pop();
        HuffmanNode* right = freeNodes.top();
        freeNodes.pop();

        int parentFreq = left->frequency + right->frequency;
        HuffmanNode* parent = new HuffmanNode(parentFreq, left, right);

        freeNodes.push(parent);
    }
    return freeNodes.top();
}

// Функция для генерации кодов Хаффмана
void generateHuffmanCodes(HuffmanNode* root, const string& code = "", map<char, string>& codes = *new map<char, string>) {
    if (root == nullptr) return;

    if (root->symbol != '\0') {
        codes[root->symbol] = code;
    }
    // Рекурсивно обходим левое и правое поддеревья
    generateHuffmanCodes(root->left, code + "0", codes);
    generateHuffmanCodes(root->right, code + "1", codes);
}

// Функция для вывода кодов Хаффмана
void printHuffmanCodes(const map<char, string>& codes) {
    cout << "\n-------Коды Хаффмана--------" << endl;

    // Создаем вектор для сортировки по длине кода (частоте)
    vector<pair<char, string>> sortedCodes(codes.begin(), codes.end());
    sort(sortedCodes.begin(), sortedCodes.end(),
        [](const auto& a, const auto& b) {
            if (a.second.length() == b.second.length()) {
                return a.second < b.second;
            }
            return a.second.length() < b.second.length();
        });

    for (const auto& pair : sortedCodes) {
        cout << "  '" << pair.first << "': " << pair.second << endl;;
    }
}
// Функция для очистки памяти
void cleanupHuffmanTree(HuffmanNode* root) {
    if (root == nullptr) return;
    cleanupHuffmanTree(root->left);
    cleanupHuffmanTree(root->right);
    delete root;
}
// Функция для кодирования строки с использованием кодов Хаффмана
string encodeString(const string& input, const map<char, string>& codes) {
    string encoded;
    for (char ch : input) {
        auto it = codes.find(ch);
        if (it != codes.end()) {
            encoded += it->second;
        }
    }
    return encoded;
}
// Функция для декодирования строки с использованием дерева Хаффмана
string decodeString(const string& encoded, HuffmanNode* root) {
    string decoded;
    HuffmanNode* current = root;

    for (char bit : encoded) {
        // Двигаемся по дереву: 0 - влево, 1 - вправо
        if (bit == '0') {
            current = current->left;
        }
        else if (bit == '1') {
            current = current->right;
        }

        // Если достигли листового узла
        if (current->left == nullptr && current->right == nullptr) {
            decoded += current->symbol;
            current = root; // Возвращаемся к корню для следующего символа
        }
    }
    return decoded;
}

int main() {
    setlocale(LC_ALL, "RU");
    cout << "Введите исходную строку: ";
    string input;
    getline(cin, input);
    map<char, int> frequency = howOften(input); //Вычисление частот символов
    auto freeNodes = createFreeNodesList(frequency);// Создание списка свободных узлов
    HuffmanNode* huffmanTree = buildHuffmanTree(freeNodes);// Построение дерева Хаффмана

    // Вывод итогового дерева
    //cout << "\n" << string(50, '=') << endl;
    cout << "ДЕРЕВО ХАФФМАНА" << endl;
    cout << string(50, '=') << endl;
    printHuffmanTree(huffmanTree);

    // Генерация и вывод кодов Хаффмана
    map<char, string> huffmanCodes;
    generateHuffmanCodes(huffmanTree, "", huffmanCodes);
    printHuffmanCodes(huffmanCodes);

    // Кодирование исходной строки
    string encoded = encodeString(input, huffmanCodes);
    cout << "\n---------Результаты--------" << endl;
    cout << "Исходная строка: " << input << endl;
    cout << "Закодированная строка: " << encoded << endl;
    cout << "Длина закодированной строки: " << encoded.length() << " бит" << endl;
    cout << "Длина исходной строки в битах (8 бит на символ): " << input.length() * 8 << " бит" << endl;

    // Декодирование обратно
    string decoded = decodeString(encoded, huffmanTree);
    cout << "Декодированная строка: " << decoded << endl;
    // Проверка корректности
    if (input == decoded) {
        cout << "Декодирование выполнено успешно!" << endl;
    }
    else {
        cout << "Ошибка декодирования!" << endl;
    }
    // Очистка памяти
    cleanupHuffmanTree(huffmanTree);

    return 0;
}