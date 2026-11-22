#include<iostream>
#include<vector>
#include<string>
#include<sstream>
#include<algorithm>
#include<cmath>
#include<queue>
#include <optional>
#include <functional>
#include <cstddef> 
#include <cassert>

template<typename T>
class Node {
public:
    T data;
    Node* next;
    Node(T data) : data(data), next(nullptr) {}
};

template <typename T>
class LinkedList {
public:
    Node<T>* head;
    Node<T>* tail;
    LinkedList() {
        head = nullptr;
        tail = nullptr;
    }
    ~LinkedList() {
        Node<T>* temp = head;
        while (temp != nullptr) {
            Node<T>* next = temp->next;
            delete temp;
            temp = next;
        }
    } 
    LinkedList(const LinkedList&) = delete;
    LinkedList& operator=(const LinkedList&) = delete;
class Iterator {
    private:
        Node<T>* current;
    public:
        explicit Iterator(Node<T>* node) : current(node) {}

        T& operator*() const { return current->data; }
        T* operator->() const { return &(current->data); }

        Iterator& operator++() {
            if (current) current = current->next;
            return *this;
        }

        bool operator!=(const Iterator& other) const {
            return current != other.current;
        }

        bool operator==(const Iterator& other) const {
            return current == other.current;
        }
    };
    Iterator begin() { return Iterator(head); }
    Iterator end() { return Iterator(nullptr); }
    Iterator begin() const { return Iterator(head); }
    Iterator end() const { return Iterator(nullptr); } 
    void add(T data) {
        Node<T>* newNode = new Node<T>(data);
        if (head == nullptr) {
            head = newNode;
            tail = newNode;
        }
        else {
            tail->next = newNode;
            tail = newNode;
        }
    }
    void printList() {
        Node<T>* temp = head;
        while (temp != nullptr) {
            std::cout << temp->data << " ";
            temp = temp->next;
        }
        std::cout << std::endl;
    }
    int  countNodes() {
        int count = 0;
        Node<T>* temp = head;
        while (temp != nullptr) {
            temp = temp->next;
            count++;
        }
        return count;
    }
    void pushFront(T value) {
        Node<T>* newNode = new Node<T>(value);
        if (head == nullptr) {
            head = newNode;
            tail = newNode;
        }
        else {
            newNode->next = head;
            head = newNode;
        }
    }
    void pushBack(T value) {
        Node<T>* newNode = new Node<T>(value);
        if (head == nullptr) {
            head = newNode;
            tail = newNode;
        }
        else {
            tail->next = newNode;
            tail = newNode;
        }
    }
    Node<T>* findNode(T value) {
        Node<T>* temp = head;
        while (temp != nullptr) {
            if (temp->data == value)
                return temp;
            temp = temp->next;
        }
        return nullptr;
    }
    void deleteNode(T value) {
        Node<T>* temp = head;
        Node<T>* prev = nullptr;
        if (temp != nullptr && temp->data == value) {
            head = head->next;
            delete temp;
            if (head == nullptr) tail = nullptr;
                return;
            return; 
        }
        while (temp != nullptr) {
            if (temp->data == value) {
                break;
            }
            else {
                prev = temp;
                temp = temp->next;
            }
        }
        if (temp == nullptr) {
            return;
        }
        else {
            if (temp != tail) {
                prev->next = temp->next;
                delete temp;
            }
            else {
                prev->next = nullptr;
                tail = prev;
                delete temp;
            }
        }
    }
};

enum class State { EMPTY, OCCUPIED, DELETED };
enum class ProbeType { LINEAR , QUADRATIC , DOUBLE};
template <typename K, typename V>
class Entry { public: K key{}; V value{}; State state = State::EMPTY; };

template<typename Key , typename Value , typename Hash = std::hash<Key>>
class OpenAdressingHashTable {
private:
    
    std::size_t maxSize;                        
    std::size_t size_ = 0;                    
    std::vector<Entry<Key, Value>> table;     
    ProbeType probeType;                   
    Hash hasher;

    std::size_t hashFunction(const Key& key) const {
        return hasher(key) % maxSize;
    }

    static std::size_t nextPrime(std::size_t n) {
        if (n <= 2) return 2;
        if (n % 2 == 0) ++n;                   
        for (;; n += 2) {                    
            bool prime = true;
            for (std::size_t d = 3; d * d <= n; d += 2) {
                if (n % d == 0) { prime = false; break; }
            }
            if (prime) return n;
        }
    }

    void insertNoRehash(const Key& key, const Value& value) {
        std::size_t base = hashFunction(key);   
        std::size_t i = 0;
        std::optional<std::size_t> firstTombstone; 
        std::size_t idx = base;

        while (table[idx].state != State::EMPTY) { 
            if (table[idx].state == State::OCCUPIED && table[idx].key == key) {
                table[idx].value = value;
                return;                  
            } 
            if (table[idx].state == State::DELETED && !firstTombstone)
                firstTombstone = idx;

            idx = nextIndex(base, ++i);    
        }

        std::size_t target = firstTombstone.value_or(idx);
        table[target].key = key;
        table[target].value = value;
        table[target].state = State::OCCUPIED;
        ++size_;                              
    }

    std::size_t nextIndex(std::size_t base, std::size_t i) const {
        switch (probeType) {
        case ProbeType::LINEAR:
            return (base + i) % maxSize;
        case ProbeType::QUADRATIC:
            return (base + i * i) % maxSize;
        case ProbeType::DOUBLE: {
            std::size_t h2 = 1 + (base % (maxSize - 1));
            return (base + i * h2) % maxSize;
            }
        }
        return base;
    }

    void maybeRehash() {
        if ((size_ + 1) * 4 > maxSize * 3) 
            rehash();    
    }
public:
    explicit OpenAdressingHashTable(std::size_t size,ProbeType type = ProbeType::LINEAR) : maxSize(nextPrime(size)), table(maxSize), probeType(type) {}

    void insert(const Key& key, const Value& value) {
        insertNoRehash(key, value);
        maybeRehash();
    }

    bool remove(const Key& key) {
        std::size_t base = hashFunction(key);
        std::size_t i = 0;
        std::size_t idx = base;

        while (table[idx].state != State::EMPTY) {
            if (table[idx].state == State::OCCUPIED && table[idx].key == key) {
                table[idx].state = State::DELETED;  
                --size_;                              
                return true;
            }
            idx = nextIndex(base, ++i);
            if (i > maxSize) break;
        }
        return false;                      
    }

    std::optional<Value> find(const Key& key) const {
        std::size_t base = hashFunction(key);
        std::size_t probesCount = 0;
        std::size_t idx = base;

        while (table[idx].state != State::EMPTY) {
            if (table[idx].state == State::OCCUPIED && table[idx].key == key)
                return table[idx].value;

            idx = nextIndex(base, probesCount++);
            if (probesCount > maxSize) break;
        }

        return std::nullopt;
    }

    std::size_t size()  const { return size_; }
    bool empty() const { return size_ == 0; }

    void rehash() {
        std::size_t newSize = nextPrime(maxSize*2);
        std::vector<Entry<Key, Value>> old = std::move(table);
        table.assign(newSize, Entry<Key, Value>{});
        maxSize = newSize;
        size_ = 0;

        for (auto& item : old)
            if (item.state == State::OCCUPIED)                 
                insertNoRehash(item.key, item.value);
    }

};
 
int main() { 
    return 0;
}
/* 
template <typename Key , typename Value, typename Hash = std::hash<Key>>
class HashTable {
private:
    class Entry { public:
        Key key;
        Value value;

        Entry(const Key& key , const Value& value): key(key),value(value) {}

        bool operator==(const Entry& other) const {
            return key == other.key;
        }
    };

    std::vector<LinkedList<Entry>> table;
    int currentSize;
    int maxSize;

    std::size_t hashFunction(Key key) {
        Hash hasher;
        return hasher(key) % maxSize;
    }

public:

    explicit HashTable(int size) : currentSize(0), maxSize(size), table(size>0? size:1) {}

    HashTable(const HashTable&) = delete;
    HashTable& operator=(const HashTable&) = delete;

    void insert(const Key& key, const Value& value) {

        std::size_t idx = hashFunction(key);

        for (auto& item : table[idx]) {
            if (item.key == key) {
                item.value = value;
                return;
            }

        }
        table[idx].pushBack(Entry(key, value));
        currentSize++;

    }

    bool get(const Key& key , Value& outValue) {
        std::size_t idx = hashFunction(key);

        for (auto& item : table[idx]) {
            if (item.key == key) {
                outValue = item.value;
                return true;
            } 
        }
        return false;
    }

    bool remove(const Key& key) {
        std::size_t idx = hashFunction(key);

        for (auto& item : table[idx]) {
            if (item.key == key) {
                table[idx].deleteNode(item);
                currentSize--;
                return true;
            }
        }
        return false;
    }

    int size() const { return currentSize; }
    bool empty() const { return currentSize == 0; }


};
*/


/* 

// a hand made data types
namespace dataTypes {

    // A node class to make nodes for linked lists
    template<typename T>
    class Node {
    public:
        T data;
        Node* next;
        Node(T data) : data(data), next(nullptr) {}
    };

    // a node with a previous pointer to use with doubley linked lists
    template<typename T>
    class Node2 {
    public:
        T data;
        Node2* next;
        Node2* prev;
        Node2(T data) : data(data), next(nullptr), prev(nullptr) {}
    };


    // a handmade vector
    template <typename T>
    class HandMadeVector {
    private:
        T* data;
        int size;
        int capacity;
        void claim() {
            capacity *= 2;
            T* newData = new T[capacity];
            for (int i = 0; i < size; i++) {
                newData[i] = data[i];
            }
            delete[]data;
            data = newData;
        }
    public:
        HandMadeVector() {
            size = 0;
            capacity = 2;
            data = new T[capacity];
        }
        ~HandMadeVector() {
            delete[] data;
        }
        void pushBack(T value) {
            if (size == capacity) {
                claim();
            }
            data[size] = value;
            size++;
        }
        void pushFront(T value) {
            if (size == capacity) { claim(); }
            T* newData = new T[size + 1];
            newData[0] = value;
            for (int i = 0; i < size; i++) {
                newData[i + 1] = data[i];
            }
            delete[] data;
            data = newData;
        }
        int getSize() { return size; }
        int getCapacity() { return capacity; }
        T& operator[](int index) { return data[index]; }

    };

    // a handmade singley linked list
    template <typename T>
    class LinkedList {
    public:
        Node<T>* head;
        Node<T>* tail;
        LinkedList() {
            head = nullptr;
            tail = nullptr;
        }
        void add(T data) {
            Node<T>* newNode = new Node<T>(data);
            if (head == nullptr) {
                head = newNode;
                tail = newNode;
            }
            else {
                Node<T>* temp = head;
                while (temp->next != nullptr) {
                    temp = temp->next;
                }
                temp->next = newNode;
                tail = newNode;
            }
        }
        void printList() {
            Node<T>* temp = head;
            while (temp != nullptr) {
                std::cout << temp->data << " ";
                temp = temp->next;
            }
            std::cout << temp->data << std::endl;
        }
        int  countNodes() {
            int count = 0;
            Node<T>* temp = head;
            while (temp != nullptr) {
                temp = temp->next;
                count++;
            }
            return count;
        }
        void pushFront(T value) {
            Node<T>* newNode = new Node<T>(value);
            if (head == nullptr) {
                head = newNode;
                tail = newNode;
            }
            else {
                newNode->next = head;
                head = newNode;
            }
        }
        void pushBack(T value) {
            Node<T>* newNode = new Node<T>(value);
            if (head == nullptr) {
                head = newNode;
                tail = newNode;
            }
            else {
                tail->next = newNode;
                tail = newNode;
            }
        }
        Node<T>* findNode(T value) {
            Node<T>* temp = head;
            while (temp != nullptr) {
                if (temp->data == value)
                    return temp;
                temp = temp->next;
            }
            return nullptr;
        }
        void deleteNode(T value) {
            Node<T>* temp = head;
            Node<T>* prev = nullptr;
            if (temp != nullptr && temp->data == value) {
                head = head->next;
                delete temp;
                if (tail == nullptr)
                    return;
            }
            while (temp != nullptr) {
                if (temp->data == value) {
                    break;
                }
                else {
                    prev = temp;
                    temp = temp->next;
                }
            }
            if (temp == nullptr) {
                return;
            }
            else {
                if (temp != tail) {
                    prev->next = temp->next;
                    delete temp;
                }
                else {
                    prev->next = nullptr;
                    tail = prev;
                    delete temp;
                }
            }
        }
    };

    // a circular linked list
    template<typename T>
    class CircularList {
    public:
        Node2<T>* head;
        Node2<T>* tail;
        CircularList() :head(nullptr), tail(nullptr) {}
        void printList() {
            if (head == nullptr) {
                std::cout << "Empty" << std::endl;
                return;
            }Node2<T>* temp = head;
            do {
                std::cout << temp->data << " ";
                temp = temp->next;
            } while (temp != head);

            std::cout << std::endl;
        }
        void pushBack(T value) {
            Node2<T>* newNode = new Node2<T>(value);
            if (head == nullptr) {
                head = newNode;
                tail = newNode;
                newNode->next = newNode;
                newNode->prev = newNode;
                return;
            }
            newNode->next = head;
            newNode->prev = tail;
            head->prev = newNode;
            tail->next = newNode;
            tail = newNode;
        }
        void pushFront(T value) {
            Node2<T>* newNode = new Node2<T>(value);
            if (head == nullptr) {
                head = newNode;
                tail = newNode;
                newNode->next = newNode;
                newNode->prev = newNode;
                return;
            }
            newNode->next = head;
            newNode->prev = tail;
            head->prev = newNode;
            tail->next = newNode;
            head = newNode;
        }
        void pushAt(T value, int place) {
            if (place <= 1) {
                pushFront(value);
                return;
            }
            if (head == nullptr) {
                pushBack(value);
                return;
            }Node2<T>* temp = head;
            int count = 1;
            while (count < place) {
                count++;
                temp = temp->next;
                if (temp == head) {
                    pushBack(value);
                    return;
                }
            }

            Node2<T>* newNode = new Node2<T>(value);
            newNode->next = temp;
            newNode->prev = temp->prev;
            temp->prev->next = newNode;
            temp->prev = newNode;
        }
        void popBack() {
            if (head == nullptr)
                return;
            if (head == tail) {
                head = nullptr;
                tail = nullptr;
                return;
            }
            Node2<T>* temp = tail;
            temp->prev->next = head;
            head->prev = temp->prev;
            delete temp;
            tail = head->prev;
        }
        void popFront() {
            if (head == nullptr)
                return;
            if (head == tail) {
                head = nullptr;
                tail = nullptr;
                return;
            }
            Node2<T>* temp = head;
            head = head->next;
            head->prev = tail;
            tail->next = head;
            delete temp;
        }
        void deleteAt(int place) {
            if (head == nullptr)
                return;
            if (place == 1) {
                popFront();
                return;
            }
            Node2<T>* temp = head;
            int count = 1;
            while (count < place) {
                count++;
                temp = temp->next;
                if (temp == head)
                    break;
            }
            if (temp == tail) {
                popBack();
                return;
            }
            temp->prev->next = temp->next;
            temp->next->prev = temp->prev;
            delete temp;
        }
        Node2<T>* findNode(T value) {
            if (head == nullptr) {
                std::cout << "Not founded....." << std::endl;
                return nullptr;
            }
            Node2<T>* temp = head;
            while (temp != nullptr) {
                if (temp->data == value)
                    return temp;
                temp = temp->next;
                if (temp == head) {
                    std::cout << "Not founded....." << std::endl;
                    return nullptr;
                }
            }
        }
    };


    // A deque based on a singly linked list
    template<typename T>
    class LinkedListBasedDeque {
    public:
        Node<T>* head;
        Node<T>* tail;
        LinkedListBasedDeque() : head(nullptr), tail(nullptr) {}
        ~LinkedListBasedDeque() {
            Node<T>* current = head;
            while (current != nullptr) {
                Node<T>* temp = current;
                current = current->next;
                delete temp;
            }
        }
        bool isEmpty() {
            return (head == nullptr);
        }
        void pushFront(T value) {
            if (isEmpty()) {
                Node<T>* newNode = new Node<T>(value);
                newNode->next = head;
                head = newNode;
                tail = newNode;
                return;
            }
            Node<T>* newNode = new Node<T>(value);
            newNode->next = head;
            head = newNode;
            return;
        }
        void pushBack(T value) {
            if (isEmpty()) {
                Node<T>* newNode = new Node<T>(value);
                newNode->next = head;
                head = newNode;
                tail = newNode;
                return;
            }
            Node<T>* newNode = new Node<T>(value);
            tail->next = newNode;
            tail = newNode;
            return;

        }
        T popBack() {
            if (isEmpty()) {
                throw std::runtime_error("The deck is empty");
            }
            if (head == tail) {
                T data = head->data;
                delete head;
                head = nullptr;
                tail = nullptr;
                return data;
            }
            Node<T>* temp = head;
            while (temp->next != tail) {
                temp = temp->next;
            }

            T data = tail->data;
            delete tail;
            temp->next = nullptr;
            tail = temp;
            return data;
        }
        T popFront() {
            if (isEmpty()) {
                throw std::runtime_error("The deck is empty");
            }
            if (head == tail) {
                T data = head->data;
                delete head;
                head = nullptr;
                tail = nullptr;
                return data;
            }
            Node<T>* temp = head;
            T data = temp->data;
            head = head->next;
            delete temp;
            return data;
        }
        T peekFront() {
            if (isEmpty()) {
                throw std::runtime_error("The deck is empty");
            }
            return head->data;
        }
        T peekBack() {
            if (isEmpty()) {
                throw std::runtime_error("The deck is empty");
            }
            return tail->data;
        }

    };

    // a deque based on a normal array
    template<typename T>
    class ArrayBasedDeque {
        int size, capacity;
        T* array;
        ArrayBasedDeque(int capacity) : size(0), capacity(capacity) {
            array = new T[capacity];
        }
        ~ArrayBasedDeque() {
            delete[] array;
        }
        bool isEmpty() {
            return (size == 0);
        }
        bool isFull() {
            return (size == capacity);
        }
        void pushFront(T value) {
            if (isFull()) {
                throw std::overflow_error("The deque is full");
            }
            if (isEmpty()) {
                array[0] = value;
                size++;
                return;
            }
            int i = 0;
            while (i != size - 1) {
                array[i + 1] = array[i];
                i++;
            }
            array[0] = value;
            size++;
            return;
        }
        void pushBack(T value) {
            if (isFull()) {
                throw std::overflow_error("The deque is full");
            }
            if (isEmpty()) {
                array[0] = value;
                size++;
                return;
            }
            array[size++] = value;
            return;
        }
        T popBack() {
            if (isEmpty()) {
                throw std::runtime_error("The deque is empty");
            }
            T data = array[size - 1];
            size--;
            return data;
        }
        T popFront() {
            if (isEmpty()) {
                throw std::runtime_error("The deque is empty");
            }
            int i = 1;
            T data = array[0];
            while (i != size - 1) {
                array[i] = array[i + 1];
                i++;
            }
            return data;
        }
        T peekFront() {
            if (isEmpty()) {
                throw std::runtime_error("TThe deque is empty");
            }
            return array[0];
        }
        T peekBack() {
            if (isEmpty()) {
                throw std::runtime_error("The deque is empty");
            }
            return array[size - 1];
        }
        int getSize() {
            return size;
        }


    };

    // a priority queue based on a singly linked list
    template <typename T>
    class LinkedListBasedPriorityQueue {
    public:
        Node<T>* head;
        int size;
        LinkedListBasedPriorityQueue() : head(nullptr), size(0) {}
        ~LinkedListBasedPriorityQueue() {
            Node<T>* current = head;
            while (current != nullptr) {
                Node<T>* temp = current;
                current = current->next;
                delete temp;
            }
        }
        bool isEmpty() {
            return (head == nullptr);
        }
        void enqueue(T value) {
            if (head == nullptr || head->data > value) {
                Node<T>* newNode = new Node<T>(value);
                newNode->next = head;
                head = newNode;
                size++;
                return;
            }

            Node<T>* temp = head;
            while (temp->next != nullptr && temp->next->data < value) {
                temp = temp->next;
            }

            Node<T>* newNode = new Node<T>(value);
            newNode->next = temp->next;
            temp->next = newNode;
            size++;
            return;
        }

        T dequeu() {
            if (isEmpty()) {
                throw std::runtime_error("The queue is empty");
            }
            T data = head->data;
            head = head->next;
            size--;
            return data;
        }
        T peek() {
            if (isEmpty()) {
                throw std::runtime_error("The queue is empty");
            }
            return head->data;
        }
        int getSize() {
            return size;
        }


    };

    // a priority queue based on an array
    template<typename T>
    class ArrayBasedPriorityQueue {
    private:
        int size;
        int capacity;
        T* array;

    public:
        ArrayBasedPriorityQueue(int capacity) : size(0), capacity(capacity) {
            array = new T[capacity];
        }

        void enqueue(T value) {
            if (size == capacity) {
                throw std::overflow_error("The queue is full");
            }
            if (size == 0) {
                array[0] = value;
                size++;
                return;
            }
            int i;
            for (i = size - 1; i >= 0; i--) {
                if (value < array[i]) {
                    array[i + 1] = array[i];
                }
                else {
                    break;
                }
            }
            array[i + 1] = value;
            size++;
        }

        T dequeue() {
            if (size == 0) {
                throw std::runtime_error("The queue is empty");
            }
            T data = array[0];
            for (int i = 0; i < size - 1; i++) {
                array[i] = array[i + 1];
            }
            size--;
            return data;
        }

        T peek() const {
            if (size == 0) {
                throw std::runtime_error("The queue is empty");
            }
            return array[0];
        }

        bool isEmpty() const {
            return (size == 0);
        }

        bool isFull() const {
            return (size == capacity);
        }

        int getSize() {
            return size;
        }
        ~ArrayBasedPriorityQueue() {
            delete[] array;
        }
    };

    // an array based queue
    template<typename T>
    class ArrayBasedQueue {
    private:
        int front, rear, size, capacity;
        T* array;
    public:
        ArrayBasedQueue(int capacity) : capacity(capacity), front(0), rear(capacity - 1), size(0) {
            array = new T[capacity];
        }
        ~ArrayBasedQueue() {
            delete[] array;
        }

        void enqueue(T value) {
            array[rear] = value;
            rear++;
            size++;
        }
        T dequeue() {
            if (size == 0) {
                throw std::runtime_error("Queue is empty");
            }
            T data = array[front];
            front++;
            size--;
            return data;
        }
        T peek() {
            if (size == 0) {
                throw std::runtime_error("Queue is empty");
            }
            return array[front];
        }
        bool isEmpty() {
            return(size == 0);
        }
        bool isFull() {
            return(size == capacity);
        }
    };

    // a singley linked list queue
    template<typename T>
    class LinkedListBasedQueue {
    public:
        Node<T>* front;
        Node<T>* rear;
        LinkedListBasedQueue() : front(nullptr), rear(nullptr) {};
        void enqueue(T value) {
            Node<T>* newNode = new Node<T>(value);
            if (front == nullptr) {
                front = newNode;
                rear = newNode;
            }
            rear->next = newNode;
            newNode->next = nullptr;
            rear = newNode;
        }
        T dequeue() {
            if (front == nullptr) {
                throw std::runtime_error("the Queue is empty!!");
            }
            T data = front->data;
            front = front->next;
            return data;
        }
        T peek() {
            if (front == nullptr) {
                throw std::runtime_error("the Queue is empty!!");
            }
            return front->data;
        }
        bool isEmpty() {
            return(rear == nullptr);
        }
        ~LinkedListBasedQueue() {
            while (!isEmpty()) {
                dequeue();
            }
        }

    };

    // a stack based on a singly linked list
    template<typename T>
    class LinkedListBasedStack {
    private:
        Node<T>* topNode;
    public:
        LinkedListBasedStack() : topNode(nullptr) {}
        void push(T value) {
            Node<T>* newNode = new Node<T>(value);
            newNode->next = topNode;
            topNode = newNode;
        }
        T pop() {
            if (isEmpty()) {
                std::cout << "Stack is empty" << std::endl;
                return T();
            }
            Node<T>* temp = topNode;
            T value = temp->data;
            topNode = topNode->next;
            delete temp;
            return value;
        }
        T peek() {
            if (isEmpty()) {
                std::cout << "Stack is empty" << std::endl;
                return T();
            }
            return topNode->data;
        }
        bool isEmpty() {
            return topNode == nullptr;
        }
    };

    // a stack based on a normal array
    template <typename T>
    class arrayBasedStack {
    private:
        int maxSize;
        int top;
        T* stackArray;
    public:
        arrayBasedStack(int size) {
            maxSize = size;
            stackArray = new T[size];
            top = -1;
        }
        ~arrayBasedStack() {
            delete[] stackArray;
        }
        void push(T value) {
            if (isFull()) {
                std::cout << "the stack is already filled" << std::endl;
                return;
            }
            top++;
            stackArray[top] = value;

            return;
        }
        T pop() {
            if (isEmpty()) {
                std::cout << "There is no element to delete" << std::endl;
                return T();
            }
            return stackArray[top--];
        }
        T peek() {
            if (isEmpty()) {
                std::cout << "There is no element in the array" << std::endl;
                return T();
            }
            return stackArray[top];
        }
        bool isFull() {
            return(top == maxSize - 1);
        }
        bool isEmpty() {
            return(top == -1);
        }
    };

    // A Node made specially for trees 
    template<typename T>
    class TreeNode {
    public:
        T data;
        TreeNode* right;
        TreeNode* left;
        TreeNode(T data) :right(nullptr), left(nullptr), data(data) {}

    };

    // A binary tree with the basic methods
    template<typename T>
    class BinaryTree {
    private:
        TreeNode<T>* root;

    public:
        BinaryTree() : root(nullptr) {}
        TreeNode<T>* copyTree(TreeNode<T>* node) {
            if (node == nullptr) return nullptr;
            TreeNode<T>* newNode = new TreeNode<T>(node->data);
            newNode->left = copyTree(node->left);
            newNode->right = copyTree(node->right);
            return newNode;
        }
        BinaryTree(const BinaryTree& other) {
            root = copyTree(other.root);
        }
        void preOrderTraversal(TreeNode<T>* start) {
            if (start != nullptr) {
                std::cout << start->data << " -> ";
                preOrderTraversal(start->left);
                preOrderTraversal(start->right);
            }
        }
        void inOrderTraversal(TreeNode<T>* start) {
            if (start != nullptr) {
                inOrderTraversal(start->left);
                std::cout << start->data << " -> ";
                inOrderTraversal(start->right);
            }
        }
        void outOrderTraversal(TreeNode<T>* start) {
            if (start != nullptr) {
                outOrderTraversal(start->right);
                std::cout << start->data << " -> ";
                outOrderTraversal(start->left);
            }
        }
        void postOrderTraversal(TreeNode<T>* start) {
            if (start != nullptr) {
                postOrderTraversal(start->left);
                postOrderTraversal(start->right);
                std::cout << start->data << " -> ";
            }
        }
        void dfsTraversal(TreeNode<T>* start) {
            if (start == nullptr) return;
            std::cout << start->data << " ";
            dfsTraversal(start->left);
            dfsTraversal(start->right);
            return;
        }
        void bfsTraversal(TreeNode<T>* start) {
            if (start == nullptr) return;

            LinkedListBasedQueue<TreeNode<T>*> q;
            q.enqueue(start);
            while (!q.isEmpty()) {

                TreeNode<T>* currentNode = q.dequeue();
                std::cout << currentNode->data << " ";
                if (currentNode->left != nullptr) q.enqueue(currentNode->left);
                if (currentNode->right != nullptr) q.enqueue(currentNode->right);

            }
        }
        void insert(T value) {
            if (root == nullptr) {
                root = new TreeNode<T>(value);
                return;
            }

            LinkedListBasedQueue<TreeNode<T>*> q;
            q.enqueue(root);

            while (!q.isEmpty()) {
                TreeNode<T>* currentNode = q.dequeue();

                if (currentNode->left != nullptr) q.enqueue(currentNode->left);
                if (currentNode->right != nullptr) q.enqueue(currentNode->right);

                if (currentNode->left == nullptr) {
                    currentNode->left = new TreeNode<T>(value);
                    return;
                }
                if (currentNode->right == nullptr) {
                    currentNode->right = new TreeNode<T>(value);
                    return;
                }
            }
        }

        bool search(T value) {
            TreeNode<T>* node = root;
            if (node == nullptr) return false;
            if (node->data == value) return true;

            bool check = false;
            LinkedListBasedQueue<TreeNode<T>*> q;
            q.enqueue(node);

            while (!q.isEmpty()) {
                TreeNode<T>* currentNode = q.dequeue();

                if (currentNode->left != nullptr) q.enqueue(currentNode->left);
                if (currentNode->right != nullptr) q.enqueue(currentNode->right);

                if (currentNode != nullptr && currentNode->data == value) return true;
            }
            return check;
        }

        int countNodes() {
            TreeNode<T>* node = root;
            if (node == nullptr) return 0;

            LinkedListBasedQueue<TreeNode<T>*> q;
            q.enqueue(node);
            int count = 0;
            while (!q.isEmpty()) {
                TreeNode<T>* currentNode = q.dequeue();

                if (currentNode->left != nullptr) q.enqueue(currentNode->left);
                if (currentNode->right != nullptr) q.enqueue(currentNode->right);

                if (currentNode != nullptr) count++;
            }
            return count;
        }

        TreeNode<T>* getRoot() {
            return root;
        }

        BinaryTree& operator=(const BinaryTree& other) {
            if (this == &other) return*this;

            TreeNode<T>* newRoot = copyTree(other.root);

            destroyTree(root);

            root = newRoot;

            return *this;

        }

        void destroyTree(TreeNode<T>* node) {
            if (node == nullptr) return;
            destroyTree(node->left);
            destroyTree(node->right);
            delete node;
            return;
        }

        ~BinaryTree() {
            destroyTree(root);
        }
    };

    // a BST with the basic methods 
    template<typename T>
    class BinarySearchTree {
    private:
        TreeNode<T>* root;
    public:

        BinarySearchTree() : root(nullptr) {}

        TreeNode<T>* copyTree(TreeNode<T>* node) {
            if (node == nullptr) return nullptr;
            TreeNode<T>* newNode = new TreeNode<T>(node->data);
            newNode->left = copyTree(node->left);
            newNode->right = copyTree(node->right);
            return newNode;
        }
        BinarySearchTree(const BinarySearchTree& other) {
            root = copyTree(other.root);
        }

        void insert(T value) {
            root = insertHelper(root, value);
            return;
        }

        TreeNode<T>* insertHelper(TreeNode<T>* node, T value) {
            if (node == nullptr) {
                TreeNode<T>* newNode = new TreeNode<T>(value);
                return newNode;
            }
            else {
                if (node->data > value) {
                    node->left = insertHelper(node->left, value);
                }
                else if (node->data < value) {
                    node->right = insertHelper(node->right, value);
                }
                else {
                    throw std::runtime_error("This value equals the parents value");
                }
            }
            return node;
        }

        TreeNode<T>* getRoot() {
            return root;
        }

        void preOrderTraversal(TreeNode<T>* start) {
            if (start != nullptr) {
                std::cout << start->data << " -> ";
                preOrderTraversal(start->left);
                preOrderTraversal(start->right);
            }
        }
        void inOrderTraversal(TreeNode<T>* start) {
            if (start != nullptr) {
                inOrderTraversal(start->left);
                std::cout << start->data << " -> ";
                inOrderTraversal(start->right);
            }
        }
        void outOrderTraversal(TreeNode<T>* start) {
            if (start != nullptr) {
                outOrderTraversal(start->right);
                std::cout << start->data << " -> ";
                outOrderTraversal(start->left);
            }
        }
        void postOrderTraversal(TreeNode<T>* start) {
            if (start != nullptr) {
                postOrderTraversal(start->left);
                postOrderTraversal(start->right);
                std::cout << start->data << " -> ";
            }
        }

        TreeNode<T>* minValue(TreeNode<T>* node) {
            if (node == nullptr) {
                return node;
            }
            while (node->left != nullptr) {
                node = node->left;
            }
            return node;
        }

        TreeNode<T>* deleteNode(TreeNode<T>* node, T value) {
            if (node == nullptr) return node;

            if (value < node->data) {
                node->left = deleteNode(node->left, value);
            }
            else if (value > node->data) {
                node->right = deleteNode(node->right, value);
            }
            else {
                if (node->left == nullptr) {
                    Node<T>* temp = node->right;
                    delete node;
                    return temp;
                }
                else if (node->right == nullptr) {
                    Node<T>* temp = node->left;
                    delete node;
                    return temp;
                }

                node->data = minValue(node->right)->data;
                node->right = deleteNode(node->right, node->data);
            }

            return node;
        }

        void printLevels(TreeNode<T>* node) {
            if (node == nullptr) {
                std::cout << "The tree is empty" << std::endl;
                return;
            }

            std::queue<TreeNode<T>*> queue;
            queue.push(node);

            int level = 0;

            while (!queue.empty()) {
                int size = queue.size();
                std::cout << "Level " << level << ": ";
                level++;

                for (int i = 0; i < size; i++) {
                    TreeNode<T>* currentNode = queue.front();
                    queue.pop();

                    std::cout << currentNode->data << " ";

                    if (currentNode->left != nullptr) queue.push(currentNode->left);
                    if (currentNode->right != nullptr) queue.push(currentNode->right);
                }

                std::cout << std::endl;
            }
        }

        int maxDepth(TreeNode<T>* node) {
            if (node == nullptr) return 0;
            int leftDepth = maxDepth(node->left);
            int rightDepth = maxDepth(node->right);
            return std::max(leftDepth, rightDepth) + 1;
        }

        bool hasPathSum(TreeNode<T>* node, int sum) {
            if (node == nullptr) return false;
            if (node->left == nullptr && node->right == nullptr) return (node->data == sum);
            return hasPathSum(node->left, sum - node->data) || hasPathSum(node->right, sum - node->data);
        }

        TreeNode<T>* findLCA(TreeNode<T>* node, TreeNode<T>* p, TreeNode<T>* q) {
            if (node == nullptr || node == p || node == q) return node;

            TreeNode<T>* left = findLCA(node->left, p, q);
            TreeNode<T>* right = findLCA(node->right, p, q);

            if (left != nullptr && right != nullptr) return node;

            return left != nullptr ? left : right;
        }

        int minDepth(TreeNode<T>* node) {
            if (node == nullptr) {
                return 0;
            }
            int depth = 0;
            std::queue<TreeNode<T>*> queue;
            queue.push(node);

            while (!queue.empty()) {
                int size = queue.size();
                depth++;
                for (int i = 0; i < size; i++) {
                    TreeNode<T>* currentNode = queue.front();
                    queue.pop();
                    if (currentNode->left == nullptr && currentNode->right == nullptr) return depth;
                    if (currentNode->left != nullptr) queue.push(currentNode->left);
                    if (currentNode->right != nullptr) queue.push(currentNode->right);
                }
            }
            return depth;
        }


        void destroyTree(TreeNode<T>* node) {
            if (node == nullptr) return;
            destroyTree(node->left);
            destroyTree(node->right);
            delete node;
            return;
        }


        BinarySearchTree& operator=(const BinarySearchTree& other) {
            if (this == &other) return*this;

            TreeNode<T>* newRoot = copyTree(other.root);

            destroyTree(root);

            root = newRoot;

            return *this;

        }

        ~BinarySearchTree() {
            destroyTree(root);
        }

    };


    // An array based min-Heap 
    template<typename T>
    class ArrayBasedMinHeap {
        T* heap;
        int capacity;
        int size;

    public:
        ArrayBasedMinHeap() {
            this->capacity = 1;
            heap = new T[capacity];
            this->size = 0;
        }
        ~ArrayBasedMinHeap() {
            delete[] heap;
        }
        void resize() {

            T* newHeap = new T[capacity * 2];

            for (int i = 0; i < capacity; i++) {
                newHeap[i] = heap[i];

            }
            delete[] heap;
            heap = newHeap;
            capacity *= 2;
            return;

        }

        void insert(T value) {
            if (size == capacity) resize();
            heap[size] = value;
            size++;
            heapifyUp(size - 1);
            return;
        }

        void print() {
            for (int i = 0; i < size; i++) {

                std::cout << heap[i] << " ";
                if (i == size - 1) {
                    std::cout << std::endl;
                }
            }
            return;
        }

        int getSize() { return size; }

        int getParent(int index) {
            return (index - 1) / 2;
        }

        bool hasParent(int index) { return index > 0; }

        void heapifyUp(int index) {
            while (hasParent(index) && heap[index] < heap[getParent(index)]) {
                T temp = heap[index];
                heap[index] = heap[getParent(index)];
                heap[getParent(index)] = temp;
                index = getParent(index);
            }
            return;
        }
        void pop() {
            if (size == 0) return;
            heap[0] = heap[size - 1];
            size--;
            if (size > 0) {
                heapifyDown(0);
            }
        }

        void deleteAt(int index) {
            if (index < 0 || index >= size) return;

            if (index == size - 1) {
                size--;
                return;
            }

            heap[index] = heap[size - 1];
            size--;

            if (index > 0 && heap[index] < heap[getParent(index)]) {
                heapifyUp(index);
            }
            else {
                heapifyDown(index);
            }
        }
        int compareChilds(int index) {
            int x = 2 * index + 1;
            int y = x + 1;

            if (x >= size) {
                return -1;
            }

            if (y >= size) {
                return x;
            }
            return heap[x] > heap[y] ? y : x;
        }


        void heapifyDown(int index) {
            int childIndex = compareChilds(index);
            while (childIndex != -1 && heap[index] > heap[childIndex]) {
                T temp = heap[index];
                heap[index] = heap[childIndex];
                heap[childIndex] = temp;
                index = childIndex;
                childIndex = compareChilds(index);

            }
            return;

        }

        ArrayBasedMinHeap(const ArrayBasedMinHeap& other) {
            capacity = other.capacity;
            size = other.size;
            heap = new T[capacity];
            for (int i = 0; i < size; i++) {
                heap[i] = other.heap[i];
            }
        }

        ArrayBasedMinHeap& operator=(const ArrayBasedMinHeap& other) {
            if (this == &other) return *this;

            delete[] heap;

            capacity = other.capacity;
            size = other.size;
            heap = new T[capacity];
            for (int i = 0; i < size; i++) {
                heap[i] = other.heap[i];
            }
        }
    };


}

// a self made library for searching functions;
// {linear search, binary search , max value search , min value search};
namespace searchAlgo {

    template <typename T>
    // a linear searching function;
    int linearSearch(std::vector<T> vec, int size, T target) {
        for (int i = 0; i < size; i++) {
            if (vec[i] == target) {
                return i;
            }
        }
        return -1;
    }

    template<typename T>
    // A jump search function for vectors;
    int jumpSearch(std::vector<T>& vec, int size, T target) {
        int jump = std::floor(std::sqrt(size));
        int i = 0;
        while (i < size && vec[i] < target) {
            i += jump;
        }
        int right = std::min(i, size - 1);
        int left = std::max(i - jump, 0);
        for (int j = left; j <= right; j++) {
            if (j < size) {
                if (vec[j] == target) {
                    return j;
                }
            }
        }
        return -1;
    }

    template <typename T>
    // A recursive binary search , slower but works;
    int recBinarySearch(const std::vector<T>& vec, int startIndex, int endIndex, T target) {
        int midIndex = startIndex + (endIndex - startIndex) / 2;
        if (startIndex > endIndex) {
            return -1;
        }
        else {
            if (vec[midIndex] == target) {
                return midIndex;
            }
            else if (vec[midIndex] < target) {
                return recBinarySearch(vec, midIndex + 1, endIndex, target);
            }
            else if (vec[midIndex] > target) {
                return recBinarySearch(vec, startIndex, midIndex - 1, target);
            }
            return -1;
        }
    }

    template <typename T>
    // A recursive Linear search , slower but works;
    int recLinearSearch(std::vector<T>& vec, int index, T target) {
        if (index >= vec.size()) {
            return -1;
        }
        else if (vec[index] == target) {
            return index;
        }
        else {
            return recLinearSearch(vec, index + 1, target);
        }

    }

    template <typename T>
    // a binary searching function for arrays only;
    int binarySearch(T* array, int size, T target) {
        int left = 0;
        int right = size - 1;
        while (left <= right) {
            int mid = left + (right - left) / 2;
            if (array[mid] == target)
                return mid;
            else if (array[mid] > target)
                right = mid - 1;
            else
                left = mid + 1;
        }
        return -1;
    }

}

//a self made library for sorting functions;
// { merge sort  , merge};
namespace sortAlgo{

template <typename T>
// A side merge function to help with the mergeSort;
void merge(T* array, int left , int mid , int right){
    int n1 = mid - left + 1;
    int n2 = right - mid;

    T* arr1 = new T[n1];
    T* arr2 = new T[n2];

    for (int i = 0; i < n1; i++) {
        arr1[i] = array[left + i];
    }
    for (int i = 0; i < n2; i++) {
        arr2[i] = array[mid + 1 + i];
    }

    int i = 0, j = 0, k = left;
    while (i < n1 && j < n2) {
        if (arr1[i] <= arr2[j]) {
            array[k++] = arr1[i++];
        } else {
            array[k++] = arr2[j++];
        }
    }
    while (i < n1) {
        array[k++] = arr1[i++];
    }
    while (j < n2) {
        array[k++] = arr2[j++];
    }

    delete[] arr1;
    delete[] arr2;
}

template <typename T>
//A recursive mergeSort function ;
void mergeSort(T* array, int left , int right){
    if(left>=right) {
        return;
    }
    int mid = left+(right-left)/2;
    mergeSort(array,left,mid);
    mergeSort(array,mid+1,right);
    merge(array,left,mid,right);
}
}
*/





