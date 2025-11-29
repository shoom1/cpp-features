# C++ Features Review: C++14 to C++26

A comprehensive guide to modern C++ features introduced from C++14 through C++26.

---

## C++14 Features

### 1. Generic Lambdas

**Description:** Lambda expressions can now use `auto` for parameter types, making them templates.

```cpp
auto lambda = [](auto x, auto y) { return x + y; };

// Works with different types
int i = lambda(1, 2);           // Returns 3
double d = lambda(1.5, 2.5);    // Returns 4.0
std::string s = lambda(std::string("Hello"), std::string(" World")); // "Hello World"
```

### 2. Return Type Deduction

**Description:** Functions can use `auto` as return type, with the compiler deducing it from return statements.

```cpp
auto add(int a, int b) {
    return a + b;  // Return type deduced as int
}

auto get_vector() {
    return std::vector<int>{1, 2, 3, 4, 5};  // Deduced as std::vector<int>
}
```

### 3. Variable Templates

**Description:** Variables can now be templated, not just functions and classes.

```cpp
template<typename T>
constexpr T pi = T(3.1415926535897932385);

float f = pi<float>;    // 3.14159f
double d = pi<double>;  // 3.14159265358979
```

### 4. Binary Literals

**Description:** Integer literals can now be specified in binary using `0b` prefix.

```cpp
int mask = 0b1111'0000;  // Binary literal with digit separator
int value = 0b0000'1111;
int result = mask & value;  // 0
```

### 5. Digit Separators

**Description:** Single quotes can be used to separate digits for readability.

```cpp
int million = 1'000'000;
double pi = 3.141'592'653'589;
unsigned long long big = 0xDEAD'BEEF'CAFE'BABE;
```

### 6. `std::make_unique`

**Description:** Factory function for creating unique pointers, completing the set with `std::make_shared`.

```cpp
#include <memory>

auto ptr = std::make_unique<int>(42);
auto vec = std::make_unique<std::vector<int>>(10, 0);  // Vector of 10 zeros

// Array version
auto arr = std::make_unique<int[]>(5);  // Array of 5 ints
```

---

## C++17 Features

### 1. Structured Bindings

**Description:** Unpack tuples, pairs, and structures into named variables.

```cpp
#include <map>
#include <string>

std::map<std::string, int> ages = {{"Alice", 30}, {"Bob", 25}};

for (const auto& [name, age] : ages) {
    std::cout << name << " is " << age << " years old\n";
}

// With pairs
std::pair<int, std::string> get_data() {
    return {42, "answer"};
}

auto [number, text] = get_data();
```

### 2. `if` and `switch` with Initializers

**Description:** Initialize variables in the scope of conditional statements.

```cpp
if (auto it = map.find(key); it != map.end()) {
    // 'it' is only in scope here
    return it->second;
}
// 'it' is out of scope

// With lock guards
if (std::lock_guard lock(mutex); condition) {
    // Lock held only in this scope
}
```

### 3. `std::optional`

**Description:** Represents a value that may or may not be present, avoiding null pointers.

```cpp
#include <optional>

std::optional<int> find_even(const std::vector<int>& vec) {
    for (int x : vec) {
        if (x % 2 == 0) return x;
    }
    return std::nullopt;  // No value
}

if (auto result = find_even({1, 3, 5, 8, 9})) {
    std::cout << "Found: " << *result << "\n";  // Found: 8
} else {
    std::cout << "No even number found\n";
}
```

### 4. `std::variant`

**Description:** Type-safe union that can hold one of several specified types.

```cpp
#include <variant>

std::variant<int, float, std::string> data;

data = 42;                      // Holds int
data = 3.14f;                   // Now holds float
data = std::string("hello");    // Now holds string

// Visiting variants
std::visit([](auto&& arg) {
    std::cout << arg << "\n";
}, data);

// Type checking
if (std::holds_alternative<std::string>(data)) {
    std::cout << std::get<std::string>(data) << "\n";
}
```

### 5. `std::any`

**Description:** Type-safe container for single values of any type.

```cpp
#include <any>

std::any a = 1;
std::cout << std::any_cast<int>(a) << "\n";  // 1

a = 3.14;
std::cout << std::any_cast<double>(a) << "\n";  // 3.14

a = std::string("hello");
std::cout << std::any_cast<std::string>(a) << "\n";  // hello

// Type checking
if (a.type() == typeid(std::string)) {
    std::cout << "Contains string\n";
}
```

### 6. `std::string_view`

**Description:** Non-owning view of a string, avoiding unnecessary copies.

```cpp
#include <string_view>

void process(std::string_view sv) {
    // No copy made, works with string literals, std::string, char*
    std::cout << sv << "\n";
}

process("hello");                        // String literal
process(std::string("world"));           // std::string

const char* text = "erview";
process(std::string_view(text, 4));      // First 4 chars: "view"
```

### 7. `if constexpr`

**Description:** Compile-time conditional statements for templates.

```cpp
template<typename T>
auto get_value(T t) {
    if constexpr (std::is_pointer_v<T>) {
        return *t;  // Dereference if pointer
    } else {
        return t;   // Return as-is otherwise
    }
}

int x = 42;
int* ptr = &x;

auto a = get_value(x);    // Returns 42
auto b = get_value(ptr);  // Returns 42 (dereferenced)
```

### 8. Fold Expressions

**Description:** Apply binary operators to parameter packs.

```cpp
template<typename... Args>
auto sum(Args... args) {
    return (args + ...);  // Unary right fold
}

template<typename... Args>
void print(Args... args) {
    (std::cout << ... << args) << "\n";  // Unary left fold
}

int total = sum(1, 2, 3, 4, 5);  // 15
print("Hello", " ", "World", "!");  // Hello World!
```

### 9. Class Template Argument Deduction (CTAD)

**Description:** Template arguments can be deduced from constructor arguments.

```cpp
std::pair p(1, 2.0);           // std::pair<int, double>
std::vector v{1, 2, 3, 4, 5};  // std::vector<int>
std::tuple t{1, "hello", 3.14}; // std::tuple<int, const char*, double>

// Custom classes
template<typename T>
class Container {
    T value;
public:
    Container(T v) : value(v) {}
};

Container c(42);  // Container<int>, no need for Container<int>
```

### 10. Inline Variables

**Description:** Variables can be declared `inline`, allowing definition in headers.

```cpp
// header.h
inline int global_counter = 0;  // Can be in header, one definition

inline constexpr double pi = 3.141592653589793;
```

---

## C++20 Features

### 1. Concepts

**Description:** Constrain template parameters with named requirements.

```cpp
#include <concepts>

template<typename T>
concept Numeric = std::integral<T> || std::floating_point<T>;

template<Numeric T>
T add(T a, T b) {
    return a + b;
}

// Custom concept
template<typename T>
concept HasSize = requires(T t) {
    { t.size() } -> std::convertible_to<std::size_t>;
};

template<HasSize T>
void print_size(const T& container) {
    std::cout << "Size: " << container.size() << "\n";
}
```

### 2. Ranges

**Description:** Composable algorithms and views for working with sequences.

```cpp
#include <ranges>
#include <vector>

std::vector<int> numbers = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

// Filter even numbers, square them, take first 3
auto result = numbers
    | std::views::filter([](int n) { return n % 2 == 0; })
    | std::views::transform([](int n) { return n * n; })
    | std::views::take(3);

for (int n : result) {
    std::cout << n << " ";  // 4 16 36
}
```

### 3. Coroutines

**Description:** Functions that can suspend and resume execution.

```cpp
#include <coroutine>
#include <iostream>

struct Generator {
    struct promise_type {
        int current_value;

        Generator get_return_object() {
            return Generator{std::coroutine_handle<promise_type>::from_promise(*this)};
        }
        std::suspend_always initial_suspend() { return {}; }
        std::suspend_always final_suspend() noexcept { return {}; }
        std::suspend_always yield_value(int value) {
            current_value = value;
            return {};
        }
        void return_void() {}
        void unhandled_exception() {}
    };

    std::coroutine_handle<promise_type> handle;

    ~Generator() { if (handle) handle.destroy(); }

    bool move_next() {
        handle.resume();
        return !handle.done();
    }

    int current_value() {
        return handle.promise().current_value;
    }
};

Generator counter(int start, int end) {
    for (int i = start; i < end; ++i) {
        co_yield i;
    }
}

// Usage
auto gen = counter(1, 5);
while (gen.move_next()) {
    std::cout << gen.current_value() << " ";  // 1 2 3 4
}
```

### 4. Modules

**Description:** Modern alternative to header files for code organization.

```cpp
// math.cppm
export module math;

export int add(int a, int b) {
    return a + b;
}

export int multiply(int a, int b) {
    return a * b;
}

// main.cpp
import math;

int main() {
    int sum = add(5, 3);      // 8
    int product = multiply(4, 7);  // 28
}
```

### 5. Three-Way Comparison (Spaceship Operator)

**Description:** Single operator `<=>` generates all comparison operators.

```cpp
#include <compare>

struct Point {
    int x, y;

    auto operator<=>(const Point&) const = default;
};

Point p1{1, 2};
Point p2{1, 3};

bool less = p1 < p2;      // true (compares x, then y)
bool equal = p1 == p2;    // false
bool greater = p1 > p2;   // false
```

### 6. Designated Initializers

**Description:** Initialize struct members by name.

```cpp
struct Config {
    int width;
    int height;
    bool fullscreen;
    std::string title;
};

Config cfg{
    .width = 1920,
    .height = 1080,
    .fullscreen = true,
    .title = "My Window"
};
```

### 7. `constinit`

**Description:** Ensure variables are initialized at compile-time.

```cpp
constinit int global = 42;  // Must be compile-time initialized

constexpr int compute() { return 100; }
constinit int value = compute();  // OK, compile-time

// Error: runtime initialization
// constinit int error = get_runtime_value();
```

### 8. `std::span`

**Description:** Non-owning view over a contiguous sequence.

```cpp
#include <span>

void process(std::span<int> data) {
    for (int& x : data) {
        x *= 2;
    }
}

int arr[] = {1, 2, 3, 4, 5};
std::vector<int> vec = {10, 20, 30};

process(arr);  // Works with arrays
process(vec);  // Works with vectors
process(std::span(arr, 3));  // First 3 elements
```

### 9. `consteval` (Immediate Functions)

**Description:** Functions that must be evaluated at compile-time.

```cpp
consteval int square(int n) {
    return n * n;
}

constexpr int value = square(5);  // OK, compile-time
// int x = square(runtime_value);  // Error, must be compile-time
```

### 10. `std::format`

**Description:** Type-safe string formatting similar to Python's f-strings.

```cpp
#include <format>

int age = 30;
std::string name = "Alice";

std::string msg = std::format("Hello, {}! You are {} years old.", name, age);
// "Hello, Alice! You are 30 years old."

std::string aligned = std::format("{:>10} | {:<10}", "right", "left");
// "     right | left      "

std::string num = std::format("{:.2f}", 3.14159);  // "3.14"
```

---

## C++23 Features

### 1. `std::expected`

**Description:** Represents either a value or an error, for explicit error handling.

```cpp
#include <expected>

std::expected<int, std::string> divide(int a, int b) {
    if (b == 0) {
        return std::unexpected("Division by zero");
    }
    return a / b;
}

auto result = divide(10, 2);
if (result) {
    std::cout << "Result: " << *result << "\n";  // Result: 5
} else {
    std::cout << "Error: " << result.error() << "\n";
}

// Chaining operations
auto value = divide(10, 2)
    .and_then([](int x) { return divide(x, 2); })
    .transform([](int x) { return x * 10; });
```

### 2. `std::print` and `std::println`

**Description:** Simpler output functions with formatting support.

```cpp
#include <print>

std::print("Hello, World!\n");
std::println("Hello with newline");

int x = 42;
std::println("The answer is {}", x);

std::println("Multiple values: {}, {}, {}", 1, 2, 3);
std::println("{:#x}", 255);  // 0xff (hex format)
```

### 3. Multidimensional Subscript Operator

**Description:** `operator[]` can now accept multiple arguments.

```cpp
template<typename T>
class Matrix {
    std::vector<T> data;
    size_t rows, cols;
public:
    Matrix(size_t r, size_t c) : data(r * c), rows(r), cols(c) {}

    // C++23: multiple subscripts
    T& operator[](size_t row, size_t col) {
        return data[row * cols + col];
    }
};

Matrix<int> mat(3, 4);
mat[1, 2] = 42;  // Access element at row 1, column 2
```

### 4. `if consteval`

**Description:** Check if code is executing at compile-time.

```cpp
constexpr int compute(int n) {
    if consteval {
        // Executed at compile-time
        return n * n;
    } else {
        // Executed at runtime
        return n * n * n;
    }
}

constexpr int compile_time = compute(5);  // Returns 25
int runtime = compute(5);                  // Returns 125
```

### 5. Deducing `this`

**Description:** Explicit object parameters simplify CRTP and recursive lambdas.

```cpp
struct Counter {
    int value = 0;

    // Deducing this
    void increment(this Counter& self) {
        self.value++;
    }

    void increment_by(this Counter& self, int n) {
        self.value += n;
    }

    // Works with const, rvalue references, etc.
    int get(this const Counter& self) {
        return self.value;
    }
};

// Recursive lambda
auto factorial = [](this auto self, int n) -> int {
    return n <= 1 ? 1 : n * self(n - 1);
};

int result = factorial(5);  // 120
```

### 6. `std::flat_map` and `std::flat_set`

**Description:** Sorted associative containers using contiguous storage.

```cpp
#include <flat_map>

std::flat_map<int, std::string> map;
map[1] = "one";
map[2] = "two";
map[3] = "three";

// Better cache locality than std::map
// Lower memory overhead
// Faster iteration
```

### 7. `std::mdspan`

**Description:** Multi-dimensional view over contiguous data.

```cpp
#include <mdspan>

std::vector<int> data(12);
std::mdspan<int, std::dextents<size_t, 2>> matrix(data.data(), 3, 4);

matrix[1, 2] = 42;  // Access element at row 1, column 2

// Different layouts possible
std::mdspan<int, std::dextents<size_t, 3>> tensor(data.data(), 2, 2, 3);
```

### 8. `std::to_underlying`

**Description:** Convert enum to its underlying type.

```cpp
#include <utility>

enum class Color : uint8_t {
    Red = 1,
    Green = 2,
    Blue = 3
};

Color c = Color::Red;
auto value = std::to_underlying(c);  // uint8_t(1)
```

### 9. `std::unreachable`

**Description:** Indicate code path that should never execute.

```cpp
#include <utility>

int classify(int x) {
    if (x < 0) return -1;
    if (x > 0) return 1;
    if (x == 0) return 0;

    std::unreachable();  // Optimization hint: this is unreachable
}
```

### 10. `std::stacktrace`

**Description:** Capture and inspect call stacks.

```cpp
#include <stacktrace>

void debug_info() {
    auto trace = std::stacktrace::current();
    std::cout << "Stack trace:\n" << trace << "\n";
}

void function_c() { debug_info(); }
void function_b() { function_c(); }
void function_a() { function_b(); }
```

---

## C++26 Features (Upcoming/Draft)

### 1. Reflection

**Description:** Compile-time introspection of types and members.

```cpp
// Proposed syntax
template<typename T>
void print_members() {
    constexpr auto members = std::meta::members_of(^T);

    for (constexpr auto member : members) {
        std::println("{}", std::meta::name_of(member));
    }
}

struct Person {
    std::string name;
    int age;
};

print_members<Person>();  // Prints: name, age
```

### 2. Pattern Matching

**Description:** Powerful switch-like construct with destructuring.

```cpp
// Proposed syntax
std::variant<int, std::string, float> value = 42;

inspect (value) {
    <int> i: std::println("int: {}", i);
    <std::string> s: std::println("string: {}", s);
    <float> f: std::println("float: {}", f);
}

// With structured bindings
std::pair p{1, "hello"};
inspect (p) {
    [x, y]: std::println("pair: {} {}", x, y);
}
```

### 3. Contracts

**Description:** Preconditions, postconditions, and assertions.

```cpp
// Proposed syntax
int divide(int a, int b)
    pre(b != 0)      // Precondition
    post(r: r == a / b)  // Postcondition
{
    return a / b;
}

class Buffer {
    size_t size_;
public:
    [[assert: size_ > 0]]  // Class invariant
    void resize(size_t new_size)
        pre(new_size > 0)
    {
        size_ = new_size;
    }
};
```

### 4. `std::simd`

**Description:** Portable SIMD (Single Instruction Multiple Data) operations.

```cpp
#include <simd>

void multiply_arrays(float* a, float* b, float* result, size_t n) {
    namespace simd = std::experimental::simd;
    using Vec = simd::native_simd<float>;

    for (size_t i = 0; i < n; i += Vec::size()) {
        Vec va(&a[i], simd::element_aligned);
        Vec vb(&b[i], simd::element_aligned);
        Vec vr = va * vb;
        vr.copy_to(&result[i], simd::element_aligned);
    }
}
```

### 5. `std::generator`

**Description:** Standard coroutine-based generator.

```cpp
#include <generator>

std::generator<int> fibonacci() {
    int a = 0, b = 1;
    while (true) {
        co_yield a;
        auto next = a + b;
        a = b;
        b = next;
    }
}

for (int n : fibonacci() | std::views::take(10)) {
    std::println("{}", n);  // 0 1 1 2 3 5 8 13 21 34
}
```

### 6. Explicit `this` Parameter (Enhancement)

**Description:** Unified function call syntax and better forwarding.

```cpp
struct Widget {
    void process(this Widget& self) {
        // Unified call syntax
        self.internal();
    }

    // Perfect forwarding of self
    template<typename Self>
    auto get_data(this Self&& self) -> decltype(auto) {
        return std::forward<Self>(self).data;
    }
};
```

### 7. `std::hive`

**Description:** Container optimized for stable element addresses and fast iteration.

```cpp
#include <hive>

std::hive<Entity> entities;

// Elements never move in memory (stable pointers)
Entity* e1 = &entities.insert(Entity{...});
Entity* e2 = &entities.insert(Entity{...});

entities.erase(e1);  // Fast removal

// Fast iteration despite holes
for (auto& entity : entities) {
    entity.update();
}
```

### 8. `constexpr std::vector` and `std::string`

**Description:** More standard library containers usable at compile-time.

```cpp
constexpr std::vector<int> compile_time_sort() {
    std::vector<int> v = {3, 1, 4, 1, 5, 9, 2, 6};
    std::ranges::sort(v);
    return v;
}

constexpr auto sorted = compile_time_sort();  // Computed at compile-time
```

### 9. `std::inplace_vector`

**Description:** Fixed-capacity vector without heap allocation.

```cpp
#include <inplace_vector>

std::inplace_vector<int, 10> vec;  // Max 10 elements, stack allocated

vec.push_back(1);
vec.push_back(2);
vec.push_back(3);

// No dynamic allocation, but size limited
```

### 10. Hazard Pointers

**Description:** Safe memory reclamation for lock-free data structures.

```cpp
#include <hazard_pointer>

std::atomic<Node*> head;

void reader_thread() {
    std::hazard_pointer hp = std::make_hazard_pointer();
    Node* node = hp.protect(head);  // Protected from reclamation

    if (node) {
        use(node->data);  // Safe to use
    }
}

void writer_thread() {
    Node* old = head.exchange(new_node);
    std::retire(old);  // Will be deleted when no hazard pointers reference it
}
```

---

## Summary

This review covers the major features introduced in modern C++ from C++14 to C++26:

- **C++14**: Foundation improvements (generic lambdas, return type deduction)
- **C++17**: Quality of life (structured bindings, optional, variant, string_view)
- **C++20**: Major revolution (concepts, ranges, coroutines, modules)
- **C++23**: Refinements (expected, print, deducing this)
- **C++26**: Future innovations (reflection, pattern matching, contracts)

Each version builds upon the previous, making C++ more expressive, safer, and easier to use while maintaining backward compatibility and zero-overhead abstraction principles.
