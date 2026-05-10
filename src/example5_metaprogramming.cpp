/**
 * Example 5: Compile-Time Dispatch Evolution
 *
 * This example keeps the metaprogramming story focused on a realistic task:
 * choosing the right serializer for a value.
 *
 * - C++11: SFINAE and type traits
 * - C++14: variable templates, generic lambdas, and relaxed constexpr
 * - C++17: if constexpr for readable compile-time branching
 * - C++20: concepts for named requirements and better diagnostics
 */

#include <concepts>
#include <iostream>
#include <iterator>
#include <print>
#include <sstream>
#include <string>
#include <string_view>
#include <type_traits>
#include <vector>

// ============================================================================
// C++11 Style: SFINAE and traits
// ============================================================================

namespace cpp11_style {

template<typename...>
struct make_void {
    typedef void type;
};

template<typename... Ts>
using void_t = typename make_void<Ts...>::type;

template<typename T, typename = void>
struct has_serialize : std::false_type {};

template<typename T>
struct has_serialize<T, void_t<decltype(std::declval<const T&>().serialize())>>
    : std::true_type {};

template<typename T>
typename std::enable_if<std::is_arithmetic<T>::value, std::string>::type
serialize(const T& value) {
    return std::to_string(value);
}

template<typename T>
typename std::enable_if<std::is_same<T, std::string>::value, std::string>::type
serialize(const T& value) {
    return "\"" + value + "\"";
}

template<typename T>
typename std::enable_if<has_serialize<T>::value, std::string>::type
serialize(const T& value) {
    return value.serialize();
}

template<typename T>
std::string serialize(const std::vector<T>& values) {
    std::string result = "[";
    for (std::size_t i = 0; i < values.size(); ++i) {
        if (i != 0) {
            result += ", ";
        }
        result += serialize(values[i]);
    }
    result += "]";
    return result;
}

struct Person {
    std::string name;
    int age;

    std::string serialize() const {
        return "{name: " + cpp11_style::serialize(name) +
               ", age: " + cpp11_style::serialize(age) + "}";
    }
};

template<int N>
struct Factorial {
    static const int value = N * Factorial<N - 1>::value;
};

template<>
struct Factorial<0> {
    static const int value = 1;
};

void demo() {
    std::cout << "=== C++11 Style: SFINAE ===\n\n";

    Person person{"Alice", 30};
    std::vector<int> scores{95, 88, 91};

    std::cout << "int: " << serialize(42) << "\n";
    std::cout << "string: " << serialize(std::string("hello")) << "\n";
    std::cout << "person: " << serialize(person) << "\n";
    std::cout << "scores: " << serialize(scores) << "\n";
    std::cout << "5! = " << Factorial<5>::value << "\n\n";
}

} // namespace cpp11_style

// ============================================================================
// C++14 Style: C++11 traits with C++14 variable templates and generic lambdas
// ============================================================================

namespace cpp14_style {

template<typename...>
struct make_void {
    using type = void;
};

template<typename... Ts>
using void_t = typename make_void<Ts...>::type;

template<typename T, typename = void>
struct has_serialize : std::false_type {};

template<typename T>
struct has_serialize<T, void_t<decltype(std::declval<const T&>().serialize())>>
    : std::true_type {};

template<typename T>
constexpr bool has_serialize_v = has_serialize<T>::value;

template<typename T>
constexpr bool arithmetic_v = std::is_arithmetic<T>::value;

template<typename T>
using arithmetic_serializer_t =
    typename std::enable_if<arithmetic_v<T>, std::string>::type;

template<typename T>
arithmetic_serializer_t<T> serialize(const T& value) {
    return std::to_string(value);
}

template<typename T>
typename std::enable_if<std::is_same<T, std::string>::value, std::string>::type
serialize(const T& value) {
    return "\"" + value + "\"";
}

template<typename T>
typename std::enable_if<has_serialize_v<T>, std::string>::type
serialize(const T& value) {
    return value.serialize();
}

template<typename T>
std::string serialize(const std::vector<T>& values) {
    std::string result = "[";
    for (std::size_t i = 0; i < values.size(); ++i) {
        if (i != 0) {
            result += ", ";
        }
        result += serialize(values[i]);
    }
    result += "]";
    return result;
}

struct Person {
    std::string name;
    int age;

    std::string serialize() const {
        auto field = [](const auto& key, const auto& value) {
            return std::string(key) + ": " + cpp14_style::serialize(value);
        };

        return "{" + field("name", name) + ", " + field("age", age) + "}";
    }
};

constexpr int factorial(int value) {
    return value <= 1 ? 1 : value * factorial(value - 1);
}

void demo() {
    std::cout << "=== C++14 Style: C++11 Traits with C++14 Variable Templates and Generic Lambdas ===\n\n";

    Person person{"Bob", 31};
    std::vector<std::string> tags{"admin", "editor"};

    std::cout << "double: " << serialize(3.14) << "\n";
    std::cout << "person: " << serialize(person) << "\n";
    std::cout << "tags: " << serialize(tags) << "\n";
    std::cout << "5! = " << factorial(5) << "\n\n";
}

} // namespace cpp14_style

// ============================================================================
// C++17 Style: if constexpr
// ============================================================================

namespace cpp17_style {

template<typename T, typename = void>
struct has_serialize : std::false_type {};

template<typename T>
struct has_serialize<T, std::void_t<decltype(std::declval<const T&>().serialize())>>
    : std::true_type {};

template<typename T>
inline constexpr bool has_serialize_v = has_serialize<T>::value;

template<typename T>
struct is_vector : std::false_type {};

template<typename T, typename Allocator>
struct is_vector<std::vector<T, Allocator>> : std::true_type {};

template<typename T>
inline constexpr bool is_vector_v = is_vector<T>::value;

template<typename T>
inline constexpr bool always_false_v = false;

template<typename T>
std::string serialize(const T& value) {
    using Value = std::decay_t<T>;

    if constexpr (std::is_arithmetic_v<Value>) {
        return std::to_string(value);
    } else if constexpr (std::is_same_v<Value, std::string>) {
        return "\"" + value + "\"";
    } else if constexpr (is_vector_v<Value>) {
        std::string result = "[";
        for (std::size_t i = 0; i < value.size(); ++i) {
            if (i != 0) {
                result += ", ";
            }
            result += serialize(value[i]);
        }
        result += "]";
        return result;
    } else if constexpr (has_serialize_v<Value>) {
        return value.serialize();
    } else {
        static_assert(always_false_v<Value>, "No serializer available for this type");
    }
}

struct Person {
    std::string name;
    int age;

    std::string serialize() const {
        return "{name: " + cpp17_style::serialize(name) +
               ", age: " + cpp17_style::serialize(age) + "}";
    }
};

template<typename... Values>
std::string serialize_tuple_like(const Values&... values) {
    std::ostringstream out;
    out << "[";
    std::string separator;
    ((out << separator << serialize(values), separator = ", "), ...);
    out << "]";
    return out.str();
}

void demo() {
    std::cout << "=== C++17 Style: if constexpr ===\n\n";

    Person person{"Charlie", 32};
    std::vector<Person> people{{"Dana", 28}, {"Eve", 35}};

    std::cout << "person: " << serialize(person) << "\n";
    std::cout << "people: " << serialize(people) << "\n";
    std::cout << "mixed: " << serialize_tuple_like(42, std::string("ok"), 2.5)
              << "\n\n";
}

} // namespace cpp17_style

// ============================================================================
// C++20 Style: Concepts
// ============================================================================

namespace cpp20_style {

template<typename T>
concept Arithmetic = std::is_arithmetic_v<std::remove_cvref_t<T>>;

template<typename T>
concept StringLike =
    std::is_same_v<std::remove_cvref_t<T>, std::string> ||
    std::is_same_v<std::remove_cvref_t<T>, std::string_view>;

template<typename T>
concept HasSerializeMethod = requires(const T& value) {
    { value.serialize() } -> std::convertible_to<std::string>;
};

template<typename T>
concept Sequence =
    !StringLike<T> &&
    requires(const T& value) {
        typename std::remove_cvref_t<T>::value_type;
        { value.begin() } -> std::input_iterator;
        { value.end() } -> std::sentinel_for<decltype(value.begin())>;
    };

template<typename T>
concept Serializable =
    Arithmetic<T> || StringLike<T> || HasSerializeMethod<T> || Sequence<T>;

std::string serialize(Arithmetic auto value) {
    return std::to_string(value);
}

std::string serialize(const StringLike auto& value) {
    return "\"" + std::string(value) + "\"";
}

std::string serialize(const HasSerializeMethod auto& value) {
    return value.serialize();
}

std::string serialize(const Sequence auto& values) {
    std::string result = "[";
    std::string separator;
    for (const auto& value : values) {
        result += separator;
        result += serialize(value);
        separator = ", ";
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

void printSerialized(const Serializable auto& value) {
    std::println("  {}", serialize(value));
}

void demo() {
    std::println("=== C++20 Style: Concepts ===\n");

    Person person{"Frank", 40};
    std::vector<int> scores{100, 98, 97};
    std::vector<Person> reviewers{{"Grace", 29}, {"Heidi", 34}};

    std::println("Serializable values:");
    printSerialized(42);
    printSerialized(std::string_view{"concepts"});
    printSerialized(person);
    printSerialized(scores);
    printSerialized(reviewers);

    std::println("\nConcepts turn template assumptions into named contracts.");
    std::println("");
}

} // namespace cpp20_style

// ============================================================================
// Main
// ============================================================================

int main() {
    cpp11_style::demo();
    cpp14_style::demo();
    cpp17_style::demo();
    cpp20_style::demo();

    return 0;
}
