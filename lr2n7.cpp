#include <iostream>
#include <string>
#include <algorithm>
#include <unordered_map>
using namespace std;

struct DNode {// Узел для двусвязного списка LRU (должен хранить и ключ, и значение)
    int key;
    int value;
    DNode* next;
    DNode* prev;
    DNode(int k, int v) : key(k), value(v), next(nullptr), prev(nullptr) {}
};

struct DList { // Двусвязный список для LRU
    DNode* head;
    DNode* tail;
    size_t size;

    DList() : head(nullptr), tail(nullptr), size(0) {}

    ~DList() {
        DNode* current = head;
        while (current != nullptr) {
            DNode* temp = current;
            current = current->next;
            delete temp;
        }
    }

    DNode* push_front(int key, int value) {
        DNode* newNode = new DNode(key, value);

        if (head != nullptr) {
            newNode->next = head;
            head->prev = newNode;
        }
        else {
            tail = newNode;
        }

        head = newNode;
        size++;
        return newNode;
    }

    DNode* remove_tail() {
        if (tail == nullptr) return nullptr;

        DNode* temp = tail;
        tail = tail->prev;

        if (tail != nullptr) {
            tail->next = nullptr;
        }
        else {
            head = nullptr;
        }

        size--;
        temp->prev = temp->next = nullptr;
        return temp;
    }

    void move_to_front(DNode* node) {
        if (node == head) return;

        // Убираем узел из текущей позиции
        if (node->prev) node->prev->next = node->next;
        if (node->next) node->next->prev = node->prev;

        // Если узел был хвостом, обновляем хвост
        if (node == tail) tail = node->prev;

        // Вставляем в начало
        node->next = head;
        node->prev = nullptr;
        if (head) head->prev = node;
        head = node;

        // Если список был пуст, обновляем хвост
        if (!tail) tail = head;
    }

    void print() const {
        DNode* current = head;
        cout << "LRU порядок: ";
        while (current) {
            cout << "(" << current->key << ":" << current->value << ") ";
            current = current->next;
        }
        cout << endl;
    }
};

class LRUcache {
private:
    int capacity;
    unordered_map<int, DNode*> cache;// Хеш-таблица для связи ключ → узел списка
    DList order; // Двусвязный список для порядка использования

public:
    LRUcache(int cap) : capacity(cap) {}

    void SET(int x, int y) {
        // Проверяем, есть ли ключ в кэше
        if (cache.find(x) != cache.end()) {
            // Ключ уже есть - обновляем значение и перемещаем в начало
            DNode* node = cache[x];
            node->value = y;
            order.move_to_front(node);
            cout << "SET " << x << " " << y << " : ";
            order.print();
        }
        else {
            // Новый ключ
            if (cache.size() >= capacity) {
                // Кэш полный - удаляем самый старый элемент
                DNode* lastNode = order.remove_tail();
                if (lastNode) {
                    cout << "Удаляем самый старый: (" << lastNode->key << ":" << lastNode->value << ")" << endl;
                    cache.erase(lastNode->key);
                    delete lastNode;
                }
            }

            // Добавляем новый элемент в начало
            DNode* newNode = order.push_front(x, y);
            cache[x] = newNode;
            cout << "SET " << x << " " << y << " : ";
            order.print();
        }
    }

    int GET(int x) {
        if (cache.find(x) == cache.end()) {
            cout << "GET " << x << " : -1" << endl;
            return -1;
        }

        // Нашли ключ - перемещаем в начало и возвращаем значение
        DNode* node = cache[x];
        order.move_to_front(node);
        cout << "GET " << x << " : " << node->value << " ";
        order.print();
        return node->value;
    }
};

int main() {
    setlocale(LC_ALL, "RU");
    cout << "Введите ёмкость кэша: ";
    int cap;
    cin >> cap;

    cout << "Введите количество запросов: ";
    int q;
    cin >> q;

    LRUcache cache(cap);

    cout << "\nВведите запросы (SET x y или GET x):" << endl;
    for (int i = 0; i < q; i++) {
        string command;
        cin >> command;

        if (command == "SET") {
            int x, y;
            cin >> x >> y;
            cache.SET(x, y);
        }
        else if (command == "GET") {
            int x;
            cin >> x;
            cache.GET(x);
        }
        else {
            cout << "Неизвестная команда: " << command << endl;
            cout << "Введите команду заново: ";
            i--;
        }
    }

    return 0;
}