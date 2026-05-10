/**
 * Example 3: Type-Safe Alternatives Evolution
 *
 * This example uses a tiny expression tree to show a realistic progression:
 * - C++11: object-oriented AST with unique_ptr and virtual functions
 * - C++17: std::variant for closed sets of alternatives
 * - C++20: concepts to make visitor requirements explicit
 *
 * Task: Represent and evaluate numbers, addition, and multiplication.
 */

#include <concepts>
#include <iostream>
#include <memory>
#include <print>
#include <utility>
#include <variant>

// ============================================================================
// C++11 Style: Polymorphic expression tree
// ============================================================================

namespace cpp11_style {

class Expr {
public:
    virtual ~Expr() = default;
    virtual double evaluate() const = 0;
    virtual void print(std::ostream& out) const = 0;
};

using ExprPtr = std::unique_ptr<Expr>;

class Number final : public Expr {
    double value_;

public:
    explicit Number(double value) : value_(value) {}

    double evaluate() const override {
        return value_;
    }

    void print(std::ostream& out) const override {
        out << value_;
    }
};

class Addition final : public Expr {
    ExprPtr left_;
    ExprPtr right_;

public:
    Addition(ExprPtr left, ExprPtr right)
        : left_(std::move(left)), right_(std::move(right)) {}

    double evaluate() const override {
        return left_->evaluate() + right_->evaluate();
    }

    void print(std::ostream& out) const override {
        out << "(";
        left_->print(out);
        out << " + ";
        right_->print(out);
        out << ")";
    }
};

class Multiplication final : public Expr {
    ExprPtr left_;
    ExprPtr right_;

public:
    Multiplication(ExprPtr left, ExprPtr right)
        : left_(std::move(left)), right_(std::move(right)) {}

    double evaluate() const override {
        return left_->evaluate() * right_->evaluate();
    }

    void print(std::ostream& out) const override {
        out << "(";
        left_->print(out);
        out << " * ";
        right_->print(out);
        out << ")";
    }
};

ExprPtr number(double value) {
    return ExprPtr(new Number(value));
}

ExprPtr add(ExprPtr left, ExprPtr right) {
    return ExprPtr(new Addition(std::move(left), std::move(right)));
}

ExprPtr multiply(ExprPtr left, ExprPtr right) {
    return ExprPtr(new Multiplication(std::move(left), std::move(right)));
}

void demo() {
    std::cout << "=== C++11 Style: Polymorphic AST ===\n\n";

    ExprPtr expr = multiply(
        add(number(2.0), number(3.0)),
        number(4.0)
    );

    std::cout << "Expression: ";
    expr->print(std::cout);
    std::cout << "\nResult: " << expr->evaluate() << "\n";

    std::cout << "\nTradeoffs:\n";
    std::cout << "  - Good fit when users can add new expression node types\n";
    std::cout << "  - Requires heap allocation and virtual dispatch\n";
    std::cout << "  - Adding a new operation means adding another virtual function\n\n";
}

} // namespace cpp11_style

// ============================================================================
// C++17 Style: std::variant for a closed set of node types
// ============================================================================

namespace cpp17_style {

struct Expr;

struct Number {
    double value;
};

struct Addition {
    std::unique_ptr<Expr> left;
    std::unique_ptr<Expr> right;
};

struct Multiplication {
    std::unique_ptr<Expr> left;
    std::unique_ptr<Expr> right;
};

struct Expr {
    std::variant<Number, Addition, Multiplication> node;

    explicit Expr(Number value) : node(value) {}
    explicit Expr(Addition value) : node(std::move(value)) {}
    explicit Expr(Multiplication value) : node(std::move(value)) {}
};

template<class... Ts>
struct Overloaded : Ts... {
    using Ts::operator()...;
};

template<class... Ts>
Overloaded(Ts...) -> Overloaded<Ts...>;

std::unique_ptr<Expr> number(double value) {
    return std::make_unique<Expr>(Number{value});
}

std::unique_ptr<Expr> add(std::unique_ptr<Expr> left, std::unique_ptr<Expr> right) {
    return std::make_unique<Expr>(
        Addition{std::move(left), std::move(right)}
    );
}

std::unique_ptr<Expr> multiply(std::unique_ptr<Expr> left, std::unique_ptr<Expr> right) {
    return std::make_unique<Expr>(
        Multiplication{std::move(left), std::move(right)}
    );
}

double evaluate(const Expr& expr) {
    return std::visit(Overloaded{
        [](const Number& number) {
            return number.value;
        },
        [](const Addition& addition) {
            return evaluate(*addition.left) + evaluate(*addition.right);
        },
        [](const Multiplication& multiplication) {
            return evaluate(*multiplication.left) * evaluate(*multiplication.right);
        }
    }, expr.node);
}

void print(const Expr& expr) {
    std::visit(Overloaded{
        [](const Number& number) {
            std::cout << number.value;
        },
        [](const Addition& addition) {
            std::cout << "(";
            print(*addition.left);
            std::cout << " + ";
            print(*addition.right);
            std::cout << ")";
        },
        [](const Multiplication& multiplication) {
            std::cout << "(";
            print(*multiplication.left);
            std::cout << " * ";
            print(*multiplication.right);
            std::cout << ")";
        }
    }, expr.node);
}

void demo() {
    std::cout << "=== C++17 Style: std::variant ===\n\n";

    auto expr = multiply(
        add(number(2.0), number(3.0)),
        number(4.0)
    );

    std::cout << "Expression: ";
    print(*expr);
    std::cout << "\nResult: " << evaluate(*expr) << "\n";

    std::cout << "\nTradeoffs:\n";
    std::cout << "  - Good fit when the set of node types is known and closed\n";
    std::cout << "  - Adding a new operation is a new visitor, not a base-class change\n";
    std::cout << "  - Adding a new node type requires updating visitors\n\n";
}

} // namespace cpp17_style

// ============================================================================
// C++20 Style: Concepts for visitor contracts
// ============================================================================

namespace cpp20_style {

struct Expr;

struct Number {
    double value;
};

struct Addition {
    std::unique_ptr<Expr> left;
    std::unique_ptr<Expr> right;
};

struct Multiplication {
    std::unique_ptr<Expr> left;
    std::unique_ptr<Expr> right;
};

struct Negation {
    std::unique_ptr<Expr> operand;
};

struct Expr {
    std::variant<Number, Addition, Multiplication, Negation> node;

    explicit Expr(Number value) : node(value) {}
    explicit Expr(Addition value) : node(std::move(value)) {}
    explicit Expr(Multiplication value) : node(std::move(value)) {}
    explicit Expr(Negation value) : node(std::move(value)) {}
};

template<class... Ts>
struct Overloaded : Ts... {
    using Ts::operator()...;
};

template<class... Ts>
Overloaded(Ts...) -> Overloaded<Ts...>;

template<typename Visitor>
concept EvaluatesExpression =
    requires(Visitor visitor,
             const Number& number,
             const Addition& addition,
             const Multiplication& multiplication,
             const Negation& negation) {
        { visitor(number) } -> std::convertible_to<double>;
        { visitor(addition) } -> std::convertible_to<double>;
        { visitor(multiplication) } -> std::convertible_to<double>;
        { visitor(negation) } -> std::convertible_to<double>;
    };

std::unique_ptr<Expr> number(double value) {
    return std::make_unique<Expr>(Number{value});
}

std::unique_ptr<Expr> add(std::unique_ptr<Expr> left, std::unique_ptr<Expr> right) {
    return std::make_unique<Expr>(
        Addition{std::move(left), std::move(right)}
    );
}

std::unique_ptr<Expr> multiply(std::unique_ptr<Expr> left, std::unique_ptr<Expr> right) {
    return std::make_unique<Expr>(
        Multiplication{std::move(left), std::move(right)}
    );
}

std::unique_ptr<Expr> negate(std::unique_ptr<Expr> operand) {
    return std::make_unique<Expr>(Negation{std::move(operand)});
}

template<EvaluatesExpression Visitor>
double evaluate_with(const Expr& expr, Visitor&& visitor) {
    return std::visit(std::forward<Visitor>(visitor), expr.node);
}

double evaluate(const Expr& expr) {
    return evaluate_with(expr, Overloaded{
        [](const Number& number) {
            return number.value;
        },
        [](const Addition& addition) {
            return evaluate(*addition.left) + evaluate(*addition.right);
        },
        [](const Multiplication& multiplication) {
            return evaluate(*multiplication.left) * evaluate(*multiplication.right);
        },
        [](const Negation& negation) {
            return -evaluate(*negation.operand);
        }
    });
}

void print(const Expr& expr) {
    std::visit(Overloaded{
        [](const Number& number) {
            std::print("{}", number.value);
        },
        [](const Addition& addition) {
            std::print("(");
            print(*addition.left);
            std::print(" + ");
            print(*addition.right);
            std::print(")");
        },
        [](const Multiplication& multiplication) {
            std::print("(");
            print(*multiplication.left);
            std::print(" * ");
            print(*multiplication.right);
            std::print(")");
        },
        [](const Negation& negation) {
            std::print("-(");
            print(*negation.operand);
            std::print(")");
        }
    }, expr.node);
}

void demo() {
    std::println("=== C++20 Style: Constrained Visitors ===\n");

    auto expr = multiply(
        negate(add(number(2.0), number(3.0))),
        number(4.0)
    );

    std::print("Expression: ");
    print(*expr);
    std::println("\nResult: {}", evaluate(*expr));

    std::println("\nTradeoffs:");
    std::println("  - Concepts make visitor requirements part of the API");
    std::println("  - Missing cases fail at the call site with clearer diagnostics");
    std::println("  - The variant model is still best for closed sets of alternatives");
    std::println("");
}

} // namespace cpp20_style

// ============================================================================
// Main
// ============================================================================

int main() {
    cpp11_style::demo();
    cpp17_style::demo();
    cpp20_style::demo();

    return 0;
}
