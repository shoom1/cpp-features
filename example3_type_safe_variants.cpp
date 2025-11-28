/**
 * Example 3: Type-Safe Variants Evolution
 *
 * This example shows the evolution from C-style unions to modern type-safe variants.
 * We implement a simple expression evaluator using different approaches.
 *
 * Task: Represent and evaluate mathematical expressions (numbers, addition, multiplication)
 */

#include <iostream>
#include <string>
#include <memory>
#include <variant>     // C++17
#include <print>       // C++23
#include <vector>

// ============================================================================
// C++11 Style: Tagged unions (manual type tracking)
// ============================================================================

namespace cpp11_style {

enum class ExprType {
    Number,
    Addition,
    Multiplication
};

struct Expr;  // Forward declaration

struct Addition {
    std::unique_ptr<Expr> left;
    std::unique_ptr<Expr> right;
};

struct Multiplication {
    std::unique_ptr<Expr> left;
    std::unique_ptr<Expr> right;
};

struct Expr {
    ExprType type;

    union {
        double number;
        Addition addition;
        Multiplication multiplication;
    };

    // Manual constructor/destructor needed for non-trivial types
    Expr(double n) : type(ExprType::Number), number(n) {}

    Expr(Addition&& add) : type(ExprType::Addition), addition(std::move(add)) {}

    Expr(Multiplication&& mul) : type(ExprType::Multiplication),
                                  multiplication(std::move(mul)) {}

    ~Expr() {
        // Manual cleanup based on type
        switch (type) {
            case ExprType::Number:
                break;  // No cleanup needed
            case ExprType::Addition:
                addition.~Addition();
                break;
            case ExprType::Multiplication:
                multiplication.~Multiplication();
                break;
        }
    }

    // Delete copy operations (too complex to implement correctly)
    Expr(const Expr&) = delete;
    Expr& operator=(const Expr&) = delete;

    // Move operations also complex
    Expr(Expr&& other) : type(other.type) {
        switch (type) {
            case ExprType::Number:
                number = other.number;
                break;
            case ExprType::Addition:
                new (&addition) Addition(std::move(other.addition));
                break;
            case ExprType::Multiplication:
                new (&multiplication) Multiplication(std::move(other.multiplication));
                break;
        }
    }
};

double evaluate(const Expr& expr) {
    switch (expr.type) {
        case ExprType::Number:
            return expr.number;

        case ExprType::Addition:
            return evaluate(*expr.addition.left) +
                   evaluate(*expr.addition.right);

        case ExprType::Multiplication:
            return evaluate(*expr.multiplication.left) *
                   evaluate(*expr.multiplication.right);
    }
    return 0.0;  // Should never reach
}

void demo() {
    std::cout << "=== C++11 Style: Tagged Unions ===\n\n";

    // Build expression: (2 + 3) * 4
    auto two = std::make_unique<Expr>(2.0);
    auto three = std::make_unique<Expr>(3.0);
    auto add = std::make_unique<Expr>(
        Addition{std::move(two), std::move(three)}
    );

    auto four = std::make_unique<Expr>(4.0);
    Expr expr(Multiplication{std::move(add), std::move(four)});

    double result = evaluate(expr);
    std::cout << "Result: " << result << "\n";  // 20

    // Problems:
    // - Manual type tracking (error-prone)
    // - Manual lifetime management
    // - Easy to access wrong union member
    // - Complex copy/move semantics
    // - No compile-time type safety
    // - Verbose and error-prone

    std::cout << "\n";
}

} // namespace cpp11_style

// ============================================================================
// C++17 Style: std::variant (type-safe discriminated union)
// ============================================================================

namespace cpp17_style {

// Forward declaration for recursive variant
struct Expr;

struct Addition {
    std::unique_ptr<Expr> left;
    std::unique_ptr<Expr> right;
};

struct Multiplication {
    std::unique_ptr<Expr> left;
    std::unique_ptr<Expr> right;
};

// Variant automatically handles type tracking and lifetime
struct Expr {
    std::variant<double, Addition, Multiplication> value;

    Expr(double n) : value(n) {}
    Expr(Addition&& add) : value(std::move(add)) {}
    Expr(Multiplication&& mul) : value(std::move(mul)) {}
};

// Using std::visit for type-safe pattern matching
struct EvaluateVisitor {
    double operator()(double n) const {
        return n;
    }

    double operator()(const Addition& add) const {
        return evaluate(*add.left) + evaluate(*add.right);
    }

    double operator()(const Multiplication& mul) const {
        return evaluate(*mul.left) * evaluate(*mul.right);
    }
};

double evaluate(const Expr& expr) {
    return std::visit(EvaluateVisitor{}, expr.value);
}

// Alternative: Generic lambda visitor
double evaluate_lambda(const Expr& expr) {
    return std::visit([](const auto& val) -> double {
        using T = std::decay_t<decltype(val)>;

        if constexpr (std::is_same_v<T, double>) {
            return val;
        }
        else if constexpr (std::is_same_v<T, Addition>) {
            return evaluate_lambda(*val.left) + evaluate_lambda(*val.right);
        }
        else if constexpr (std::is_same_v<T, Multiplication>) {
            return evaluate_lambda(*val.left) * evaluate_lambda(*val.right);
        }
    }, expr.value);
}

// Visitor for printing expressions
struct PrintVisitor {
    void operator()(double n) const {
        std::cout << n;
    }

    void operator()(const Addition& add) const {
        std::cout << "(";
        std::visit(PrintVisitor{}, add.left->value);
        std::cout << " + ";
        std::visit(PrintVisitor{}, add.right->value);
        std::cout << ")";
    }

    void operator()(const Multiplication& mul) const {
        std::cout << "(";
        std::visit(PrintVisitor{}, mul.left->value);
        std::cout << " * ";
        std::visit(PrintVisitor{}, mul.right->value);
        std::cout << ")";
    }
};

void print(const Expr& expr) {
    std::visit(PrintVisitor{}, expr.value);
}

void demo() {
    std::cout << "=== C++17 Style: std::variant ===\n\n";

    // Build expression: (2 + 3) * 4
    auto two = std::make_unique<Expr>(2.0);
    auto three = std::make_unique<Expr>(3.0);
    auto add = std::make_unique<Expr>(
        Addition{std::move(two), std::move(three)}
    );

    auto four = std::make_unique<Expr>(4.0);
    Expr expr(Multiplication{std::move(add), std::move(four)});

    std::cout << "Expression: ";
    print(expr);
    std::cout << "\n";

    double result = evaluate(expr);
    std::cout << "Result (visitor): " << result << "\n";

    // Test with lambda visitor
    auto five = std::make_unique<Expr>(5.0);
    auto six = std::make_unique<Expr>(6.0);
    Expr expr2(Addition{std::move(five), std::move(six)});

    double result2 = evaluate_lambda(expr2);
    std::cout << "Result (lambda): " << result2 << "\n";

    // Advantages:
    // - Type-safe: can't access wrong type
    // - Automatic lifetime management
    // - Proper copy/move semantics
    // - Exhaustive matching with std::visit
    // - Compile-time errors for missing cases
    // - Exception on wrong type access

    std::cout << "\n";
}

} // namespace cpp17_style

// ============================================================================
// C++20 Style: Enhanced variants with concepts
// ============================================================================

namespace cpp20_style {

// Forward declaration
struct Expr;

// Use concepts to constrain visitor types
template<typename T>
concept ExprVisitor = requires(T visitor, const Expr& expr) {
    { visitor(0.0) } -> std::convertible_to<double>;
};

struct Addition {
    std::unique_ptr<Expr> left;
    std::unique_ptr<Expr> right;
};

struct Multiplication {
    std::unique_ptr<Expr> left;
    std::unique_ptr<Expr> right;
};

struct Subtraction {
    std::unique_ptr<Expr> left;
    std::unique_ptr<Expr> right;
};

struct Expr {
    std::variant<double, Addition, Multiplication, Subtraction> value;

    Expr(double n) : value(n) {}
    Expr(Addition&& add) : value(std::move(add)) {}
    Expr(Multiplication&& mul) : value(std::move(mul)) {}
    Expr(Subtraction&& sub) : value(std::move(sub)) {}
};

// Overload pattern helper (C++20 feature often used with variants)
template<class... Ts>
struct overload : Ts... {
    using Ts::operator()...;
};

// Deduction guide
template<class... Ts>
overload(Ts...) -> overload<Ts...>;

double evaluate(const Expr& expr) {
    return std::visit(overload{
        [](double n) { return n; },
        [](const Addition& add) {
            return evaluate(*add.left) + evaluate(*add.right);
        },
        [](const Multiplication& mul) {
            return evaluate(*mul.left) * evaluate(*mul.right);
        },
        [](const Subtraction& sub) {
            return evaluate(*sub.left) - evaluate(*sub.right);
        }
    }, expr.value);
}

void print(const Expr& expr) {
    std::visit(overload{
        [](double n) { std::print("{}", n); },
        [](const Addition& add) {
            std::print("(");
            print(*add.left);
            std::print(" + ");
            print(*add.right);
            std::print(")");
        },
        [](const Multiplication& mul) {
            std::print("(");
            print(*mul.left);
            std::print(" * ");
            print(*mul.right);
            std::print(")");
        },
        [](const Subtraction& sub) {
            std::print("(");
            print(*sub.left);
            std::print(" - ");
            print(*sub.right);
            std::print(")");
        }
    }, expr.value);
}

void demo() {
    std::println("=== C++20 Style: Variants with Overload Pattern ===\n");

    // Build expression: (10 - 3) * (2 + 4)
    auto ten = std::make_unique<Expr>(10.0);
    auto three = std::make_unique<Expr>(3.0);
    auto sub = std::make_unique<Expr>(
        Subtraction{std::move(ten), std::move(three)}
    );

    auto two = std::make_unique<Expr>(2.0);
    auto four = std::make_unique<Expr>(4.0);
    auto add = std::make_unique<Expr>(
        Addition{std::move(two), std::move(four)}
    );

    Expr expr(Multiplication{std::move(sub), std::move(add)});

    std::print("Expression: ");
    print(expr);
    std::println("");

    double result = evaluate(expr);
    std::println("Result: {}", result);  // 42

    // Advantages of overload pattern:
    // - Cleaner syntax than separate visitor struct
    // - Lambdas can capture local variables
    // - Each case is defined inline
    // - Still type-safe and exhaustive

    std::println("");
}

} // namespace cpp20_style

// ============================================================================
// C++26 Style: Pattern Matching (Proposed)
// ============================================================================

namespace cpp26_style {

// NOTE: This is proposed syntax for C++26 pattern matching
// It may not compile with current compilers

struct Expr;

struct Addition {
    std::unique_ptr<Expr> left;
    std::unique_ptr<Expr> right;
};

struct Multiplication {
    std::unique_ptr<Expr> left;
    std::unique_ptr<Expr> right;
};

struct Subtraction {
    std::unique_ptr<Expr> left;
    std::unique_ptr<Expr> right;
};

struct Expr {
    std::variant<double, Addition, Multiplication, Subtraction> value;

    Expr(double n) : value(n) {}
    Expr(Addition&& add) : value(std::move(add)) {}
    Expr(Multiplication&& mul) : value(std::move(mul)) {}
    Expr(Subtraction&& sub) : value(std::move(sub)) {}
};

/*
// Proposed C++26 pattern matching syntax:

double evaluate(const Expr& expr) {
    inspect (expr.value) {
        <double> n: return n;

        <Addition> [left, right]: {
            return evaluate(*left) + evaluate(*right);
        }

        <Multiplication> [left, right]: {
            return evaluate(*left) * evaluate(*right);
        }

        <Subtraction> [left, right]: {
            return evaluate(*left) - evaluate(*right);
        }
    }
}

void print(const Expr& expr) {
    inspect (expr.value) {
        <double> n: std::print("{}", n);

        <Addition> [left, right]: {
            std::print("(");
            print(*left);
            std::print(" + ");
            print(*right);
            std::print(")");
        }

        <Multiplication> [left, right]: {
            std::print("(");
            print(*left);
            std::print(" * ");
            print(*right);
            std::print(")");
        }

        <Subtraction> [left, right]: {
            std::print("(");
            print(*left);
            std::print(" - ");
            print(*right);
            std::print(")");
        }
    }
}

// Even more advanced: Pattern matching with guards

std::string classify(const Expr& expr) {
    inspect (expr.value) {
        <double> n if (n == 0.0): return "zero";
        <double> n if (n < 0.0): return "negative";
        <double> n if (n > 0.0): return "positive";

        <Addition> _: return "sum";
        <Multiplication> _: return "product";
        <Subtraction> _: return "difference";
    }
}

// Nested pattern matching
bool isConstant(const Expr& expr) {
    inspect (expr.value) {
        <double> _: return true;

        <Addition> [<double> _, <double> _]: return true;

        _: return false;
    }
}
*/

void demo() {
    std::println("=== C++26 Style: Pattern Matching (Proposed) ===\n");

    std::println("Pattern matching will provide:");
    std::println("  - Natural syntax like match in Rust/ML");
    std::println("  - Structural decomposition inline");
    std::println("  - Pattern guards (if conditions)");
    std::println("  - Nested patterns");
    std::println("  - More concise than std::visit");
    std::println("  - Compiler-enforced exhaustiveness");

    std::println("\nSee comments in source for proposed syntax examples.");
    std::println("");
}

} // namespace cpp26_style

// ============================================================================
// Comparison: Same operation in different styles
// ============================================================================

void comparison_demo() {
    std::println("=== Comparison: Expression (5 + 3) ===\n");

    // Show how checking type differs across versions
    std::println("C++11: Manual switch on type tag");
    std::println("  - Error-prone, can access wrong member");
    std::println("  - No compile-time safety");

    std::println("\nC++17: std::visit with variant");
    std::println("  - Type-safe, compiler-enforced");
    std::println("  - Requires visitor struct or lambda");

    std::println("\nC++20: Overload pattern");
    std::println("  - Cleaner inline lambdas");
    std::println("  - Still type-safe");

    std::println("\nC++26: Pattern matching");
    std::println("  - Most concise and natural");
    std::println("  - Built-in language feature");
    std::println("  - Supports complex patterns");
}

// ============================================================================
// Main
// ============================================================================

int main() {
    cpp11_style::demo();
    cpp17_style::demo();
    cpp20_style::demo();
    cpp26_style::demo();
    comparison_demo();

    return 0;
}
