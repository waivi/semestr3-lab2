#include <iostream>
#include <iomanip>
#include <fstream>
#include <vector>
#include <stdexcept>
#include <algorithm>
#include <cctype>
#include <sstream>

using namespace std;
struct MArray {
    string* data;
    size_t capacity;
    size_t size;
};

void MRESIZE(MArray& arr) {
    if (arr.size < arr.capacity) return;

    size_t newCapacity = (arr.capacity == 0) ? 4 : arr.capacity * 2;
    string* newData = new string[newCapacity];

    for (size_t i = 0; i < arr.size; i++) {
        newData[i] = arr.data[i];
    }

    delete[] arr.data;
    arr.data = newData;
    arr.capacity = newCapacity;
}

void MINIT(MArray& arr) {
    arr.data = nullptr;
    arr.size = 0;
    arr.capacity = 0;
}

void MCLEAR(MArray& arr) {
    delete[] arr.data;
    arr.data = nullptr;
    arr.size = 0;
    arr.capacity = 0;
    cout << "Массив очищен." << endl;
}

void MADDEND(MArray& arr, const string& value) {
    MRESIZE(arr);

    arr.data[arr.size] = value;
    arr.size++;
}

void MPRINT(const MArray& arr) {
    if (arr.size == 0) {
        cout << "Словарь пуст!" << endl;
        return;
    }

    cout << "Словарь [" << arr.size << "]: ";
    for (size_t i = 0; i < arr.size; i++) {
        cout << "\"" << arr.data[i] << "\"";
        if (i < arr.size - 1) {
            cout << ", ";
        }
    }
    cout << endl;
}
void getNumber(int& n) {
    bool isCorrect = false;
    while (isCorrect == false) {
        cout << "Введите количество слов, которые будут находиться в словаре: ";
        if (cin >> n) { isCorrect = true; }
        else { cout << "Ошибка ввода! Недопустимый символ, введите число!" << endl; }
    }
}
void getWord(string& word) {
    bool isCorrect = false;
    while (isCorrect == false)
    {
        cin >> word; int countGrand = 0;
        for (const auto& letter : word) {
            if ((isalpha(letter)) or (letter == ' ')) {
                if (isupper(letter)) {
                    countGrand++;
                }
            }
            else {
                countGrand = 0;
                break;
            }
        }
        if (countGrand != 1) {
            cout << "Ошибка ввода! Недопустимый формат! Необходимо ввести слово с ОДНИМ вариантом ударения заглавной буквой! " << endl;
            cout << "Попробуйте снова: ";
        }
        else { isCorrect = true; }
    }
}
bool MFIND(const string& word, const MArray& list) {
    for (int i = 0; i < list.size; i++) {
        if (word == list.data[i]) { return true; }
    }
    return false;
}

bool isWordInList(const MArray& list, string word) {
    string lowerWord = word;
    transform(lowerWord.begin(), lowerWord.end(), lowerWord.begin(), ::tolower);

    for (int i = 0; i < lowerWord.size(); i++) {
        string variant = lowerWord;
        variant[i] = toupper(variant[i]);  // ставим ударение на i-ю позицию
        for (size_t j = 0; j < list.size; j++) {
            if (variant == list.data[j]) {
                return true;
            }
        }
    }
    return false;
}
int isCorrectText(const string& text, const MArray& list) {
    int errorCount = 0;
    string Text = text + " "; // Добавляем пробел в конец для корректной работы
    size_t startPos = 0;

    while (startPos < Text.size()) {
        size_t index = Text.find(' ', startPos);
        if (index == string::npos) break;

        string word = Text.substr(startPos, index - startPos);
        startPos = index + 1;

        if (word.empty()) continue; // Пропускаем пустые слова

        // Твоя логика проверки слова
        if (MFIND(word, list)) {
            // Слово найдено в словаре - правильно
        }
        else if (true) {
            if (!isWordInList(list, word)) {
                int countGrand = 0;
                for (const auto& letter : word) {
                    if (isalpha(letter)) {
                        if (isupper(letter)) {
                            countGrand++;
                        }
                    }
                }
                if (countGrand != 1) {
                    errorCount++;
                }
            }
            else { errorCount++; }
        }
    }
    return errorCount;
}
int main() {
    setlocale(LC_ALL, "RU");
    int sz; getNumber(sz);
    cout << "Введите " << sz << " слов в словарь, учитывая, что ударную букву надо писать заглавной (пример: stArted):" << endl;
    MArray list; MINIT(list);
    for (int i = 0; i < sz; i++) {
        string word;
        getWord(word); MADDEND(list, word);
    }
    cin.ignore();
    cout << "Введите строку для проверки:" << endl;
    string text;  getline(cin, text);
    cout << "В тексте " << isCorrectText(text, list) << " ошибок" << endl;
    return 0;
}
