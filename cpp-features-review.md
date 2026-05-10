# Selected C++ Features Review: C++14 to C++26

A practical guide to selected modern C++ features introduced from C++14 through C++26.

This is not an exhaustive standard reference. It focuses on features that are useful to recognize in real code. C++26 is still in-flight as of May 2026, and compiler/library support varies widely, so the C++26 section separates draft features from proposals that are not yet settled.

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

### 2. Generalized Lambda Capture

**Description:** Lambdas can initialize captured variables, including move-only objects.

```cpp
#include <memory>

auto ptr = std::make_unique<int>(42);

auto task = [value = std::move(ptr)] {
    return *value;
};

int result = task();  // 42
```

This is often called init-capture. It made lambdas much more practical for ownership-aware callbacks and asynchronous work.

### 3. Relaxed `constexpr`

**Description:** `constexpr` functions can contain more normal control flow, including local variables and loops.

```cpp
constexpr int factorial(int n) {
    int result = 1;

    for (int i = 2; i <= n; ++i) {
        result *= i;
    }

    return result;
}

static_assert(factorial(5) == 120);
```

### 4. Return Type Deduction

**Description:** Functions can use `auto` as return type, with the compiler deducing it from return statements.

```cpp
auto add(int a, int b) {
    return a + b;  // Return type deduced as int
}

auto get_vector() {
    return std::vector<int>{1, 2, 3, 4, 5};  // Deduced as std::vector<int>
}
```

### 5. Variable Templates

**Description:** Variables can now be templated, not just functions and classes.

```cpp
template<typename T>
constexpr T pi = T(3.1415926535897932385);

float f = pi<float>;    // 3.14159f
double d = pi<double>;  // 3.14159265358979
```

### 6. Binary Literals

**Description:** Integer literals can now be specified in binary using `0b` prefix.

```cpp
int mask = 0b1111'0000;  // Binary literal with digit separator
int value = 0b0000'1111;
int result = mask & value;  // 0
```

### 7. Digit Separators

**Description:** Single quotes can be used to separate digits for readability.

```cpp
int million = 1'000'000;
double pi = 3.141'592'653'589;
unsigned long long big = 0xDEAD'BEEF'CAFE'BABE;
```

### 8. `std::make_unique`

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

const char* text = "viewer";
process(std::string_view(text, 4));      // First 4 chars: "view"
```

### 7. `std::filesystem`

**Description:** Portable path manipulation, directory traversal, and filesystem operations.

```cpp
#include <filesystem>
#include <iostream>

namespace fs = std::filesystem;

fs::path config = fs::current_path() / "config" / "app.toml";

if (fs::exists(config)) {
    std::cout << "Config size: " << fs::file_size(config) << " bytes\n";
}

for (const auto& entry : fs::directory_iterator(".")) {
    if (entry.path().extension() == ".cpp") {
        std::cout << entry.path().filename() << "\n";
    }
}
```

### 8. `std::byte`

**Description:** Type-safe representation of raw memory that is distinct from character and integer types.

```cpp
#include <cstddef>
#include <vector>

std::vector<std::byte> packet(4);

packet[0] = std::byte{0xDE};
packet[1] = std::byte{0xAD};
packet[2] = std::byte{0xBE};
packet[3] = std::byte{0xEF};

auto first = std::to_integer<unsigned int>(packet[0]);  // 222
```

### 9. `if constexpr`

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

### 10. Fold Expressions

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

### 11. Class Template Argument Deduction (CTAD)

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

### 12. Inline Variables

**Description:** Variables can be declared `inline`, allowing definition in headers.

```cpp
// header.h
inline int global_counter = 0;  // Can be in header, one definition

inline constexpr double pi = 3.141592653589793;
```

### 13. Parallel Algorithms and Numeric Reductions

**Description:** Standard algorithms can accept execution policies, and new numeric algorithms support parallel-friendly reductions and scans.

```cpp
#include <execution>
#include <numeric>
#include <vector>

std::vector<int> values = {1, 2, 3, 4, 5};

int sum = std::reduce(
    std::execution::par,
    values.begin(),
    values.end(),
    0
);

std::vector<int> prefix(values.size());
std::inclusive_scan(values.begin(), values.end(), prefix.begin());
// prefix: 1, 3, 6, 10, 15
```

Execution-policy support varies by standard library and platform, so this is a feature to verify in the target toolchain.

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

### 11. `std::jthread` and `std::stop_token`

**Description:** `std::jthread` joins automatically and integrates cooperative cancellation.

```cpp
#include <chrono>
#include <stop_token>
#include <thread>

std::jthread worker([](std::stop_token stop) {
    while (!stop.stop_requested()) {
        do_one_unit_of_work();
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
});

// Request cancellation; worker joins automatically when destroyed.
worker.request_stop();
```

### 12. `std::source_location`

**Description:** Capture call-site file, line, column, and function name without macros.

```cpp
#include <iostream>
#include <source_location>
#include <string_view>

void log(
    std::string_view message,
    std::source_location location = std::source_location::current()
) {
    std::cout << location.file_name() << ":"
              << location.line() << " "
              << message << "\n";
}

log("failed to open config");
```

### 13. `<bit>` Utilities

**Description:** Low-level bit operations become standard vocabulary.

```cpp
#include <bit>
#include <cstdint>

float value = 1.0f;
auto bits = std::bit_cast<std::uint32_t>(value);

constexpr auto native_endian = std::endian::native;

int ones = std::popcount(0b1011u);  // 3
```

### 14. Latches, Barriers, and Semaphores

**Description:** Standard synchronization primitives for coordinating threads.

```cpp
#include <barrier>
#include <latch>
#include <semaphore>
#include <thread>

std::latch ready(1);
std::counting_semaphore<4> permits(2);

std::jthread worker([&] {
    ready.count_down();

    permits.acquire();
    process_shared_resource();
    permits.release();
});

ready.wait();  // Wait until all expected arrivals happen
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

### 11. `std::generator`

**Description:** Standard coroutine-based generator for pull-style lazy sequences.

```cpp
#include <generator>
#include <print>
#include <ranges>

std::generator<int> fibonacci() {
    int a = 0;
    int b = 1;

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

### 12. C++23 Ranges Additions

**Description:** More range adaptors and algorithms make common sequence transformations easier to express.

```cpp
#include <print>
#include <ranges>
#include <string_view>
#include <vector>

std::vector<std::string_view> names = {"Ada", "Bjarne", "Grace"};
std::vector<int> scores = {98, 95, 99};

for (auto [name, score] : std::views::zip(names, scores)) {
    std::println("{}: {}", name, score);
}

for (auto group : scores | std::views::chunk(2)) {
    // Process fixed-size groups.
}

for (auto [index, name] : std::views::enumerate(names)) {
    std::println("{}: {}", index, name);
}
```

Notable additions include `zip`, `enumerate`, `chunk`, `slide`, `chunk_by`, `starts_with`, and `ends_with`. Standard-library support is still uneven across vendors.

### 13. `std::move_only_function`

**Description:** Type-erased callable wrapper for callbacks that are movable but not copyable.

```cpp
#include <functional>
#include <memory>

struct Job {
    void run();
};

std::move_only_function<void()> task =
    [job = std::make_unique<Job>()] {
        job->run();
    };

task();
```

Use this when a callback owns move-only state and `std::function` is too restrictive.

### 14. `std::out_ptr` and `std::inout_ptr`

**Description:** Safer smart-pointer interop with C APIs that write through output pointer parameters.

```cpp
#include <memory>

struct resource_handle;

extern "C" int c_open_resource(resource_handle** out);
extern "C" void c_close_resource(resource_handle*);

std::unique_ptr<resource_handle, decltype(&c_close_resource)> handle(
    nullptr,
    c_close_resource
);

if (c_open_resource(std::out_ptr(handle)) == 0) {
    use_resource(handle.get());
}
```

This avoids manually calling `release()` and `reset()` around C APIs.

### 15. `std::forward_like`

**Description:** Forward one expression with the cv/ref qualifiers of another expression.

```cpp
#include <utility>

struct Wrapper {
    std::string value;

    template<typename Self>
    decltype(auto) get(this Self&& self) {
        return std::forward_like<Self>(self.value);
    }
};
```

This is useful in generic code, especially with explicit object parameters.

---

## C++26 Features (Draft / Support Varies)

The following items are expected C++26 features or are in the C++26 working draft. Treat examples as illustrative until your target compiler and standard library document support.

### 1. Contracts

**Description:** Function preconditions, function postconditions, and assertion statements.

```cpp
int divide(int numerator, int denominator)
    pre(denominator != 0)
    post(result: result * denominator == numerator)
{
    contract_assert(denominator != 0);
    return numerator / denominator;
}
```

Contracts make API assumptions visible in the function declaration. They are not class invariants, and they are not a replacement for normal runtime error handling.

### 2. Static Reflection and `<meta>`

**Description:** Compile-time introspection of declarations, types, and members.

```cpp
#include <meta>

struct Person {
    std::string name;
    int age;
};

constexpr std::meta::info person_type = ^^Person;
constexpr auto members = std::meta::nonstatic_data_members_of(person_type);

// Current reflection papers use ^^ for reflection and [: ... :] for splicing.
// Exact library names and examples may still require compiler-specific support.
```

Reflection is one of the largest C++26 changes, but implementation support is still emerging.

### 3. `std::hive`

**Description:** A sequence container that reuses erased element storage and keeps references/pointers to non-erased elements stable.

```cpp
#include <hive>
#include <memory>

std::hive<Entity> entities;

auto it = entities.insert(Entity{/* ... */});
Entity* entity = std::addressof(*it);

entities.erase(it);  // Invalidates references/pointers to the erased element only

for (Entity& item : entities) {
    item.update();
}
```

Unlike `std::vector`, `std::hive` is designed for workloads with frequent insertion/removal where stable addresses matter.

### 4. `std::inplace_vector`

**Description:** A fixed-capacity, variable-size sequence container with storage inside the container object.

```cpp
#include <inplace_vector>

std::inplace_vector<int, 10> values;  // Capacity is fixed at 10

values.push_back(1);
values.push_back(2);
values.push_back(3);

// No separate dynamic allocation for elements, but size cannot exceed capacity.
```

This is useful when a maximum size is known and allocation control matters.

### 5. `std::simd`

**Description:** Portable data-parallel types for SIMD-style operations.

```cpp
#include <simd>

void multiply_arrays(const float* left, const float* right, float* out, std::size_t count) {
    using Vec = std::native_simd<float>;

    std::size_t i = 0;
    for (; i + Vec::size() <= count; i += Vec::size()) {
        Vec a(left + i, std::element_aligned);
        Vec b(right + i, std::element_aligned);
        (a * b).copy_to(out + i, std::element_aligned);
    }

    for (; i < count; ++i) {
        out[i] = left[i] * right[i];
    }
}
```

The goal is portable vectorization without writing platform-specific intrinsics.

### 6. Hazard Pointers and RCU

**Description:** Standard library facilities for safe memory reclamation in lock-free and read-mostly concurrent data structures.

```cpp
#include <hazard_pointer>

// Conceptual shape only: protect a pointer before dereferencing it,
// then retire removed nodes so reclamation waits for active readers.
std::atomic<Node*> head;

void reader() {
    auto hazard = std::make_hazard_pointer();
    Node* node = hazard.protect(head);

    if (node != nullptr) {
        use(node->data);
    }
}
```

These facilities are specialized tools. Most application code should prefer higher-level synchronization unless lock-free reclamation is truly needed.

### 7. `std::text_encoding`

**Description:** A standard vocabulary type for identifying text encodings.

```cpp
#include <print>
#include <text_encoding>

std::text_encoding encoding = std::text_encoding::environment();
std::println("Environment encoding: {}", encoding.name());

if (std::text_encoding::environment_is<std::text_encoding::UTF8>()) {
    // Environment encoding is UTF-8.
}
```

This is useful for code that must reason about platform text encoding rather than assuming UTF-8 everywhere.

### 8. `std::execution` / Senders and Receivers

**Description:** A standard framework for composing asynchronous work as sender/receiver task graphs.

```cpp
#include <execution>
#include <thread>
#include <utility>

namespace ex = std::execution;

auto work =
    ex::just(40)
    | ex::then([](int value) {
          return value + 2;
      });

auto result = std::this_thread::sync_wait(std::move(work));
```

The important idea is composability: senders describe work, receivers consume completion, and algorithms connect pieces into asynchronous pipelines. Implementation support is still early.

### 9. Pack Indexing

**Description:** Access a specific element of a parameter pack by compile-time index.

```cpp
#include <print>

template<typename... Ts>
using first_type = Ts...[0];

void print_first(auto... values) {
    std::println("{}", values...[0]);
}

print_first("first", "second", "third");  // first
```

Pack indexing removes a lot of tuple-conversion boilerplate from template metaprogramming.

### 10. `std::function_ref` and `std::copyable_function`

**Description:** More precise callable wrappers for non-owning and qualifier-aware callback APIs.

```cpp
#include <functional>
#include <span>

void for_each_value(
    std::span<const int> values,
    std::function_ref<void(int)> visitor
) {
    for (int value : values) {
        visitor(value);
    }
}

std::copyable_function<int(int)> scale = [factor = 2](int value) {
    return value * factor;
};
```

`std::function_ref` is for borrowing a callable without owning it. `std::copyable_function` is a stricter successor-style wrapper that preserves cv/ref/noexcept call signatures better than `std::function`.

### 11. More `constexpr` Standard Library Algorithms

**Description:** More standard library algorithms become usable during constant evaluation.

```cpp
#include <algorithm>
#include <array>

consteval auto sorted_values() {
    std::array values{3, 1, 4, 1, 5, 9};
    std::ranges::stable_sort(values);
    return values;
}

constexpr auto values = sorted_values();
```

This continues the long-running trend of making normal library vocabulary usable in compile-time code.

### Still Proposed / Not Settled C++26 Features

These are important to know about, but should not be presented as available C++26 syntax in production code:

- **Pattern matching**: active proposal work currently uses `match`-expression syntax, not the older `inspect` examples.
- **Additional reflection facilities**: the core direction is C++26, but examples should be checked against the latest paper and compiler implementation.

---

## Summary

This review covers selected features introduced in modern C++ from C++14 to C++26:

- **C++14**: Foundation improvements such as generic lambdas, init-capture, relaxed `constexpr`, return type deduction, variable templates, digit separators, and `std::make_unique`
- **C++17**: Quality-of-life and vocabulary features such as structured bindings, `std::optional`, `std::variant`, `std::string_view`, `std::filesystem`, `std::byte`, fold expressions, CTAD, inline variables, and parallel algorithms
- **C++20**: Major language and library expansion through concepts, ranges, coroutines, modules, `<=>`, `std::span`, `consteval`, `std::format`, `std::jthread`, `std::stop_token`, `std::source_location`, `<bit>`, and synchronization primitives
- **C++23**: Refinements such as `std::expected`, `std::print`, explicit object parameters, multidimensional `operator[]`, `std::mdspan`, `std::flat_map`, `std::stacktrace`, `std::generator`, C++23 range adaptors, `std::move_only_function`, `std::out_ptr`, and `std::forward_like`
- **C++26**: Draft features such as contracts, reflection, `std::hive`, `std::inplace_vector`, `std::simd`, hazard pointers/RCU, `std::execution`, pack indexing, callable wrapper refinements, and additional constexpr/library refinements

Each version builds upon the previous, making C++ more expressive, safer, and easier to use while preserving the language's focus on explicit control and zero-overhead abstractions.

## References for Draft C++26 Status

- cppreference C++17 feature summary - https://en.cppreference.com/w/cpp/17.html
- cppreference C++20 feature summary - https://en.cppreference.com/cpp/20
- cppreference C++23 feature summary - https://en.cppreference.com/cpp/23
- WG21 P2996R13: Reflection for C++26 - https://www.open-std.org/jtc1/SC22/wg21/docs/papers/2025/p2996r13.html
- WG21 P2900R14: Contracts for C++ - https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2025/p2900r14.pdf
- WG21 P0447R28: `std::hive` - https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2024/p0447r28.html
- WG21 P2688R5: Pattern Matching `match` Expression - https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2025/p2688r5.html
- cppreference C++26 compiler/library support tables - https://en.cppreference.com/cpp/compiler_support/26
- cppreference C++26 execution library - https://en.cppreference.com/cpp/execution
- cppreference C++26 pack indexing - https://en.cppreference.com/cpp/language/pack_indexing
- cppreference C++26 `std::function_ref` - https://en.cppreference.com/cpp/utility/functional/function_ref
