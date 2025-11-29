/**
 * Example 5: Meta-programming Evolution
 *
 * This example shows how compile-time programming has evolved from complex
 * template metaprogramming to modern, readable techniques.
 *
 * Task: Implement a serialization system that works with different types
 */

#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <type_traits>
#include <concepts>    // C++20
#include <format>      // C++20
#include <print>       // C++23

// ============================================================================
// C++11 Style: Classic Template Metaprogramming
// ============================================================================

namespace cpp11_style {

// Type trait to check if type has serialize method
template<typename T, typename = void>
struct has_serialize : std::false_type {};

template<typename T>
struct has_serialize<T,
    typename std::enable_if<
        std::is_same<
            decltype(std::declval<T>().serialize()),
            std::string
        >::value
    >::type
> : std::true_type {};

// Serialize fundamental types
template<typename T>
typename std::enable_if<std::is_fundamental<T>::value, std::string>::type
serialize(const T& value) {
    return std::to_string(value);
}

// Serialize strings
template<typename T>
typename std::enable_if<std::is_same<T, std::string>::value, std::string>::type
serialize(const T& value) {
    return "\"" + value + "\"";
}

// Serialize types with serialize method
template<typename T>
typename std::enable_if<
    has_serialize<T>::value &&
    !std::is_fundamental<T>::value,
    std::string
>::type
serialize(const T& value) {
    return value.serialize();
}

// Serialize vectors - complex SFINAE
template<typename T>
typename std::enable_if<
    !std::is_fundamental<T>::value &&
    !std::is_same<T, std::string>::value &&
    !has_serialize<T>::value,
    std::string
>::type
serialize(const std::vector<T>& vec) {
    std::string result = "[";
    for (size_t i = 0; i < vec.size(); ++i) {
        if (i > 0) result += ", ";
        result += serialize(vec[i]);
    }
    result += "]";
    return result;
}

// Custom type
struct Person {
    std::string name;
    int age;

    std::string serialize() const {
        return "{name: " + cpp11_style::serialize(name) +
               ", age: " + cpp11_style::serialize(age) + "}";
    }
};

// Compile-time factorial (for demonstration)
template<int N>
struct Factorial {
    static const int value = N * Factorial<N - 1>::value;
};

template<>
struct Factorial<0> {
    static const int value = 1;
};

void demo() {
    std::cout << "=== C++11 Style: Template Metaprogramming ===\n\n";

    std::cout << "Serialization examples:\n";
    std::cout << "  int: " << serialize(42) << "\n";
    std::cout << "  double: " << serialize(3.14) << "\n";
    std::cout << "  string: " << serialize(std::string("hello")) << "\n";

    Person p{"Alice", 30};
    std::cout << "  Person: " << serialize(p) << "\n";

    std::vector<int> vec{1, 2, 3, 4, 5};
    std::cout << "  vector: " << serialize(vec) << "\n";

    std::cout << "\nCompile-time computation:\n";
    std::cout << "  5! = " << Factorial<5>::value << "\n";

    // Problems:
    // - SFINAE is hard to read and write
    // - Long, complex template syntax
    // - Poor error messages
    // - Difficult to debug
    // - Hard to understand intent

    std::cout << "\n";
}

} // namespace cpp11_style

// ============================================================================
// C++17 Style: if constexpr simplifies metaprogramming
// ============================================================================

namespace cpp17_style {

// Much simpler type checking
template<typename T>
std::string serialize(const T& value) {
    if constexpr (std::is_fundamental_v<T>) {
        // Fundamental types
        return std::to_string(value);
    }
    else if constexpr (std::is_same_v<T, std::string>) {
        // Strings
        return "\"" + value + "\"";
    }
    else {
        // Types with serialize method
        return value.serialize();
    }
}

// Serialize vectors
template<typename T>
std::string serialize(const std::vector<T>& vec) {
    std::string result = "[";
    for (size_t i = 0; i < vec.size(); ++i) {
        if (i > 0) result += ", ";
        result += serialize(vec[i]);
    }
    result += "]";
    return result;
}

struct Person {
    std::string name;
    int age;

    std::string serialize() const {
        return "{name: " + cpp17_style::serialize(name) +
               ", age: " + cpp17_style::serialize(age) + "}";
    }
};

// Compile-time computation is cleaner
constexpr int factorial(int n) {
    if constexpr (true) {  // Can use if constexpr for clarity
        return (n <= 1) ? 1 : n * factorial(n - 1);
    }
}

// Get type name at compile time
template<typename T>
constexpr const char* type_name() {
    if constexpr (std::is_same_v<T, int>) {
        return "int";
    } else if constexpr (std::is_same_v<T, double>) {
        return "double";
    } else if constexpr (std::is_same_v<T, std::string>) {
        return "string";
    } else {
        return "unknown";
    }
}

void demo() {
    std::cout << "=== C++17 Style: if constexpr ===\n\n";

    std::cout << "Serialization examples:\n";
    std::cout << "  int: " << serialize(42) << "\n";
    std::cout << "  double: " << serialize(3.14) << "\n";
    std::cout << "  string: " << serialize(std::string("world")) << "\n";

    Person p{"Bob", 25};
    std::cout << "  Person: " << serialize(p) << "\n";

    std::vector<int> vec{10, 20, 30};
    std::cout << "  vector: " << serialize(vec) << "\n";

    std::cout << "\nCompile-time computation:\n";
    std::cout << "  5! = " << factorial(5) << "\n";

    std::cout << "\nType names:\n";
    std::cout << "  type_name<int>(): " << type_name<int>() << "\n";
    std::cout << "  type_name<double>(): " << type_name<double>() << "\n";

    // Advantages:
    // - Much more readable than SFINAE
    // - Single function definition
    // - Better error messages
    // - Easier to understand and maintain

    std::cout << "\n";
}

} // namespace cpp17_style

// ============================================================================
// C++20 Style: Concepts make intent explicit
// ============================================================================

namespace cpp20_style {

// Define concepts for serialization
template<typename T>
concept Fundamental = std::is_fundamental_v<T>;

template<typename T>
concept StringLike = std::is_same_v<T, std::string> ||
                     std::is_same_v<T, std::string_view>;

template<typename T>
concept HasSerializeMethod = requires(T t) {
    { t.serialize() } -> std::convertible_to<std::string>;
};

template<typename T>
concept Container = requires(T t) {
    typename T::value_type;
    { t.begin() } -> std::input_iterator;
    { t.end() } -> std::input_iterator;
    { t.size() } -> std::convertible_to<size_t>;
};

// Overloaded serialize functions with concept constraints
std::string serialize(Fundamental auto value) {
    return std::to_string(value);
}

std::string serialize(StringLike auto value) {
    return "\"" + std::string(value) + "\"";
}

std::string serialize(HasSerializeMethod auto value) {
    return value.serialize();
}

std::string serialize(Container auto container) {
    std::string result = "[";
    bool first = true;
    for (const auto& item : container) {
        if (!first) result += ", ";
        first = false;
        result += serialize(item);
    }
    result += "]";
    return result;
}

struct Person {
    std::string name;
    int age;

    std::string serialize() const {
        return "{name: " + cpp20_style::serialize(name) +
               ", age: " + cpp20_style::serialize(age) + "}";
    }
};

// Advanced: Concept-based type selection
template<typename T>
concept Numeric = std::integral<T> || std::floating_point<T>;

template<Numeric T>
constexpr T abs_value(T value) {
    return value < 0 ? -value : value;
}

// Concept composition
template<typename T>
concept Serializable = Fundamental<T> || StringLike<T> ||
                       HasSerializeMethod<T> || Container<T>;

// Constrained template
template<Serializable T>
void print_serialized(const T& value) {
    std::println("  {}", serialize(value));
}

// Requires clause for complex constraints
template<typename T>
requires (sizeof(T) <= 8) && std::is_trivially_copyable_v<T>
void fast_copy(T& dest, const T& src) {
    dest = src;  // Optimized for small types
}

void demo() {
    std::println("=== C++20 Style: Concepts ===\n");

    std::println("Serialization with concepts:");
    print_serialized(42);
    print_serialized(3.14);
    print_serialized(std::string("concepts"));

    Person p{"Charlie", 35};
    print_serialized(p);

    std::vector<int> vec{100, 200, 300};
    print_serialized(vec);

    std::println("\nConcept-based functions:");
    std::println("  abs(-42) = {}", abs_value(-42));
    std::println("  abs(3.14) = {}", abs_value(-3.14));

    // Advantages:
    // - Self-documenting requirements
    // - Clear error messages
    // - Composable constraints
    // - Better IDE support
    // - Easier to understand and maintain

    std::println("");
}

} // namespace cpp20_style

// ============================================================================
// C++23 Style: Deducing this enables new patterns
// ============================================================================

namespace cpp23_style {

// CRTP pattern - old way
template<typename Derived>
class SerializableBase {
public:
    std::string to_json() const {
        return static_cast<const Derived*>(this)->serialize();
    }
};

// New way with deducing this
class Serializable {
public:
    template<typename Self>
    std::string to_json(this const Self& self) {
        return self.serialize();
    }
};

struct Person : Serializable {
    std::string name;
    int age;

    std::string serialize() const {
        return std::format("{{\"name\": \"{}\", \"age\": {}}}", name, age);
    }
};

// Recursive lambda with deducing this
auto make_factorial() {
    return [](this auto self, int n) -> int {
        return (n <= 1) ? 1 : n * self(n - 1);
    };
}

// Builder pattern simplified
class QueryBuilder {
    std::string query_;

public:
    template<typename Self>
    auto&& select(this Self&& self, std::string fields) {
        self.query_ += "SELECT " + fields + " ";
        return std::forward<Self>(self);
    }

    template<typename Self>
    auto&& from(this Self&& self, std::string table) {
        self.query_ += "FROM " + table + " ";
        return std::forward<Self>(self);
    }

    template<typename Self>
    auto&& where(this Self&& self, std::string condition) {
        self.query_ += "WHERE " + condition;
        return std::forward<Self>(self);
    }

    std::string build() const { return query_; }
};

void demo() {
    std::println("=== C++23 Style: Deducing This ===\n");

    Person p{"David", 40};
    std::println("Serialization: {}", p.to_json());

    std::println("\nRecursive lambda:");
    auto factorial = make_factorial();
    std::println("  5! = {}", factorial(5));

    std::println("\nFluent interface:");
    QueryBuilder builder;
    auto query = builder
        .select("*")
        .from("users")
        .where("age > 18")
        .build();
    std::println("  Query: {}", query);

    // Advantages:
    // - Simpler than CRTP
    // - Perfect forwarding automatically
    // - Recursive lambdas
    // - Cleaner fluent interfaces

    std::println("");
}

} // namespace cpp23_style

// ============================================================================
// C++26 Style: Reflection (Proposed)
// ============================================================================

namespace cpp26_style {

struct Person {
    std::string name;
    int age;
    double salary;
};

/*
// Proposed C++26 reflection syntax

template<typename T>
std::string serialize(const T& value) {
    std::ostringstream oss;
    oss << "{";

    constexpr auto members = std::meta::members_of(^T);
    bool first = true;

    [: members :] >> [&]<auto member> {
        if (!first) oss << ", ";
        first = false;

        // Get member name at compile-time
        oss << std::meta::name_of(member) << ": ";

        // Get member value
        auto& member_value = value.[:member:];

        // Recursively serialize
        if constexpr (std::is_same_v<decltype(member_value), std::string>) {
            oss << "\"" << member_value << "\"";
        } else {
            oss << member_value;
        }
    };

    oss << "}";
    return oss.str();
}

// Automatic equality comparison
template<typename T>
bool equal(const T& a, const T& b) {
    constexpr auto members = std::meta::members_of(^T);

    bool result = true;
    [: members :] >> [&]<auto member> {
        result = result && (a.[:member:] == b.[:member:]);
    };

    return result;
}

// Generate visitor pattern automatically
template<typename... Types>
class Visitor {
public:
    // For each type, generate a visit function
    [: std::meta::type_list<Types...> :] >> []<typename T> {
        virtual void visit(const T&) = 0;
    };
};

// Iterate over enum values
template<typename E>
void print_enum_values() {
    constexpr auto enumerators = std::meta::enumerators_of(^E);

    [: enumerators :] >> []<auto e> {
        std::println("  {} = {}",
                    std::meta::name_of(e),
                    std::to_underlying([:e:]));
    };
}
*/

void demo() {
    std::println("=== C++26 Style: Reflection (Proposed) ===\n");

    std::println("Reflection will enable:");
    std::println("  - Compile-time introspection of types");
    std::println("  - Automatic serialization/deserialization");
    std::println("  - Generated comparison operators");
    std::println("  - Enum iteration and string conversion");
    std::println("  - Automatic visitor patterns");
    std::println("  - ORM-like functionality");
    std::println("  - Dependency injection");

    std::println("\nExample use cases:");
    std::println("  - JSON serialization without macros");
    std::println("  - Database mapping");
    std::println("  - Unit test generators");
    std::println("  - GUI bindings");
    std::println("  - Network protocol handlers");

    std::println("\nSee comments in source for proposed syntax.");
    std::println("");
}

} // namespace cpp26_style

// ============================================================================
// Comparison: Code complexity
// ============================================================================

void complexity_comparison() {
    std::println("=== Complexity Comparison ===\n");

    std::println("Lines of code to implement type checking:");
    std::println("  C++11 SFINAE: ~15-20 lines per overload");
    std::println("  C++17 if constexpr: ~5-10 lines per function");
    std::println("  C++20 Concepts: ~3-5 lines per overload");
    std::println("  C++26 Reflection: ~1-2 lines (automatic)");

    std::println("\nError message quality:");
    std::println("  C++11: Pages of template errors");
    std::println("  C++17: Better, but still template-heavy");
    std::println("  C++20: Clear concept violation messages");
    std::println("  C++26: Human-readable reflection errors");

    std::println("\nMaintainability:");
    std::println("  C++11: Expert-level knowledge required");
    std::println("  C++17: Intermediate C++ developers");
    std::println("  C++20: Readable by most C++ developers");
    std::println("  C++26: Self-explanatory code");

    std::println("");
}

// ============================================================================
// Performance comparison
// ============================================================================

void performance_comparison() {
    std::println("=== Performance Comparison ===\n");

    std::println("All techniques are zero-overhead:");
    std::println("  - C++11 SFINAE: Compile-time only");
    std::println("  - C++17 if constexpr: Dead code eliminated");
    std::println("  - C++20 Concepts: No runtime cost");
    std::println("  - C++26 Reflection: Compile-time introspection");

    std::println("\nCompilation time:");
    std::println("  C++11: Slowest (complex template instantiation)");
    std::println("  C++17: Faster (less instantiation)");
    std::println("  C++20: Similar to C++17");
    std::println("  C++26: Potentially faster (less codegen)");

    std::println("");
}

// ============================================================================
// Main
// ============================================================================

int main() {
    cpp11_style::demo();
    cpp17_style::demo();
    cpp20_style::demo();
    cpp23_style::demo();
    cpp26_style::demo();
    complexity_comparison();
    performance_comparison();

    return 0;
}
