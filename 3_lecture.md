### **Структуры и объединения**

---

#### **1. Структуры (struct)**
**Что такое структура:** Одна или несколько переменных различных типов, сгруппированных под одним именем.

**Объявление и использование:**
```cpp
struct Point {
    int x;
    int y;
};

int main() {
    Point pt;
    Point max_point = {200, 300};  // инициализация
    
    pt.x = 200;     // доступ через оператор .
    pt.y = 250;
    
    return 0;
}
```

---

#### **2. Вложенные структуры и инициализация**
```cpp
struct Rect {
    Point pt1;
    Point pt2;
};

// Разные способы инициализации:
Point p1 = {1, 2};
Point p2 = {.x = 1, .y = 2};
Rect r1 = { {1, 2}, p2 };
Rect r2 = { .pt1 = {1, 2}, .pt2 = p2 };
```

**Анонимные структуры:**
```cpp
struct Button {
    struct { int x; int y; };          // анонимная
    struct { size_t width; height; };  // анонимная
};

Button btn = {.x = 0, .y = 100, .width = 400, .height = 80};
// Обращение: btn.x, btn.width (напрямую)
```

---

#### **3. Операции со структурами**
- **Копирование, присваивание, взятие адреса**
- **Возврат из функций:**

```cpp
Point MakePoint(int x, int y) {
    Point result = {x, y};
    return result;
}

Point AddPoint(Point p1, Point p2) {
    p1.x += p2.x;
    p1.y += p2.y;
    return p1;
}
```

---

#### **4. Массивы структур и указатели**
```cpp
struct Record {
    char name[10];
    char surname[10];
    long phone;
};

Record phonebook[200];  // массив структур

// Указатели на структуры:
Record* FindRecord(long phone, Record* records, int count) {
    for (int i = 0; i < count; ++i) {
        if (records[i].phone == phone)
            return &records[i];  // возвращаем указатель
    }
    return nullptr;
}

// Использование:
Record* key = FindRecord(22345, phonebook, 10);
if (key != nullptr) {
    printf("Name: %s", key->name);  // доступ через ->
}
```

---
#### **5. Выравнивание структур pragma pack**
**`#pragma pack`** - это директива препроцессора для управления выравниванием (alignment) данных в структурах. Она позволяет вручную контролировать padding (добавочные байты), который компилятор автоматически вставляет для оптимизации доступа к памяти.
**Пример:**
```cpp
// Обычная структура (с выравниванием)
struct NormalStruct {
    char a;       // 1 байт + 3 байта padding
    int b;        // 4 байта
    short c;      // 2 байта + 2 байта padding
};
// sizeof(NormalStruct) = 12 байт

#pragma pack(push, 1)
struct PackedStruct {
    char a;       // 1 байт
    int b;        // 4 байта (без padding!)
    short c;      // 2 байта
};
#pragma pack(pop)
// sizeof(PackedStruct) = 7 байт
```
#### **6. Выравнивание структур (Alignment)**
**Правила выравнивания:**
- **Каждое поле выравнивается по адресу, кратному своему размеру**
- Общий размер структуры кратен выравниванию самого крупного поля
- Размер структуры должен быть кратен максимальному выравниванию

**Пример:**
```cpp
struct Foo {
    char a;        // 1 байт
    int64_t b;     // 8 байт
    uint8_t c;     // 1 байт
    uint32_t d;    // 4 байта
};
// Размер: 24 байта (из-за выравнивания)

struct OptimizedFoo {
    int64_t b;     // 8 байт
    uint32_t d;    // 4 байта  
    char a;        // 1 байт
    uint8_t c;     // 1 байт
};
// Размер: 16 байт (оптимизированный порядок)
```

---
#### **6. Объединения (union)**
**Что такое объединение:** Переменная, которая может содержать объекты разных типов в одной области памяти.

```cpp
union Name {
    struct {
        char name[13];
        char code[3];
    };
    struct {
        int32_t i1, i2, i3, i4;
    };
};

// Использование:
Name a = {.name = "0123456789AB", .code = "12"};
Name b = {.name = "0123456789AB", .code = "10"};

// Сравнение по-разному:
bool name_cmp = strcmp(a.name, b.name) == 0;  // сравнение как строк
bool int_cmp = (a.i1 == b.i1) && (a.i2 == b.i2) && (a.i3 == b.i3) && (a.i4 == b.i4);    // побайтовое сравнение
```

**Зачем использовать?**
- **Экономия памяти** - не нужно хранить несколько копий
- **Быстрое преобразование** между представлениями
- **Оптимизация** - числовое сравнение быстрее строкового