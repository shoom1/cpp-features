/**
 * Example 2: Container Processing Evolution
 *
 * This example shows a realistic progression for data-processing code:
 * - C++11: explicit loops and named temporaries
 * - C++14: C++11 algorithms made terser with C++14 generic lambdas
 * - C++17: structured bindings for map entries and pair results
 * - C++20: lazy range pipelines
 * - C++23: range adaptors such as chunk, slide, chunk_by, enumerate, and zip
 *
 * Task: Process a small catalog of products for reporting.
 */

#include <algorithm>
#include <cmath>
#include <functional>
#include <iomanip>
#include <iostream>
#include <map>
#include <numeric>
#include <print>
#include <ranges>
#include <sstream>
#include <string>
#include <utility>
#include <vector>
#include <version>

struct Product {
    std::string name;
    double price;
    std::string category;
    int stock;
};

std::vector<Product> getProducts() {
    return {
        {"Laptop", 999.99, "Electronics", 5},
        {"Mouse", 25.50, "Electronics", 50},
        {"Keyboard", 75.00, "Electronics", 30},
        {"Monitor", 299.99, "Electronics", 15},
        {"Desk", 450.00, "Furniture", 10},
        {"Chair", 199.99, "Furniture", 20},
        {"Lamp", 45.00, "Furniture", 40},
        {"Notebook", 5.99, "Stationery", 100},
        {"Pen", 2.50, "Stationery", 200},
        {"Coffee Mug", 12.99, "Kitchen", 60}
    };
}

std::string money(double amount) {
    std::ostringstream out;
    out << "$" << std::fixed << std::setprecision(2) << amount;
    return out.str();
}

double stockValue(const Product& product) {
    return product.price * product.stock;
}

// ============================================================================
// C++11 Style: Manual loops and explicit temporaries
// ============================================================================

namespace cpp11_style {

void demo() {
    std::cout << "=== C++11 Style: Manual Loops ===\n\n";

    const std::vector<Product> products = getProducts();
    std::vector<std::string> discounted_labels;

    for (const Product& product : products) {
        if (product.price > 100.0) {
            const double discounted = product.price * 0.9;
            discounted_labels.push_back(
                product.name + " (" + money(discounted) + ")"
            );
        }
    }

    std::cout << "Expensive products after 10% discount:\n";
    for (const std::string& label : discounted_labels) {
        std::cout << "  " << label << "\n";
    }

    double electronics_total = 0.0;
    for (const Product& product : products) {
        if (product.category == "Electronics") {
            electronics_total += stockValue(product);
        }
    }

    std::cout << "\nElectronics stock value: "
              << money(electronics_total) << "\n\n";
}

} // namespace cpp11_style

// ============================================================================
// C++14 Style: C++11 algorithms with C++14 generic lambdas
// ============================================================================

namespace cpp14_style {

void demo() {
    std::cout << "=== C++14 Style: C++11 Algorithms with C++14 Generic Lambdas ===\n\n";

    const auto products = getProducts();

    std::vector<Product> expensive;
    std::copy_if(products.begin(), products.end(), std::back_inserter(expensive),
        [](const auto& product) {
            return product.price > 100.0;
        });

    const auto discounted_label = [](const auto& product, auto discount) {
        const auto discounted = product.price * (1.0 - discount);
        return product.name + " (" + money(discounted) + ")";
    };

    std::vector<std::string> labels;
    std::transform(expensive.begin(), expensive.end(), std::back_inserter(labels),
        [&](const auto& product) {
            return discounted_label(product, 0.10);
        });

    std::cout << "Expensive products after 10% discount:\n";
    for (const auto& label : labels) {
        std::cout << "  " << label << "\n";
    }

    const auto electronics_total = std::accumulate(
        products.begin(), products.end(), 0.0,
        [](auto total, const auto& product) {
            return product.category == "Electronics"
                ? total + stockValue(product)
                : total;
        });

    std::cout << "\nElectronics stock value: "
              << money(electronics_total) << "\n\n";
}

} // namespace cpp14_style

// ============================================================================
// C++17 Style: Structured bindings for map and pair results
// ============================================================================

namespace cpp17_style {

void demo() {
    std::cout << "=== C++17 Style: Structured Bindings for Map and Pair Results ===\n\n";

    const auto products = getProducts();
    std::map<std::string, double> totals_by_category;

    for (const auto& product : products) {
        totals_by_category[product.category] += stockValue(product);
    }

    std::cout << "Stock value by category:\n";
    for (const auto& [category, total] : totals_by_category) {
        std::cout << "  " << category << ": " << money(total) << "\n";
    }

    std::vector<std::pair<std::string, double>> electronics;
    for (const auto& product : products) {
        if (product.category == "Electronics") {
            electronics.emplace_back(product.name, stockValue(product));
        }
    }

    std::sort(electronics.begin(), electronics.end(),
        [](const auto& left, const auto& right) {
            return left.second > right.second;
        });

    std::cout << "\nTop electronics by stock value:\n";
    const auto limit = std::min<std::size_t>(3, electronics.size());
    for (std::size_t i = 0; i < limit; ++i) {
        const auto& [name, value] = electronics[i];
        std::cout << "  " << (i + 1) << ". " << name
                  << ": " << money(value) << "\n";
    }

    std::cout << "\n";
}

} // namespace cpp17_style

// ============================================================================
// C++20 Style: Ranges and lazy views
// ============================================================================

namespace cpp20_style {

void demo() {
    std::println("=== C++20 Style: Ranges and Views ===\n");

    const auto products = getProducts();

    auto expensive_discounted = products
        | std::views::filter([](const Product& product) {
              return product.price > 100.0;
          })
        | std::views::transform([](const Product& product) {
              return product.name + " (" + money(product.price * 0.9) + ")";
          });

    std::println("Expensive products after 10% discount:");
    for (const auto& label : expensive_discounted) {
        std::println("  {}", label);
    }

    auto electronics = products
        | std::views::filter([](const Product& product) {
              return product.category == "Electronics";
          })
        | std::views::transform([](const Product& product) {
              return std::pair{product.name, stockValue(product)};
          });

    std::vector<std::pair<std::string, double>> ranked(
        electronics.begin(), electronics.end()
    );
    std::ranges::sort(ranked, std::greater{}, &std::pair<std::string, double>::second);

    std::println("\nTop electronics by stock value:");
    for (const auto& [name, value] : ranked | std::views::take(3)) {
        std::println("  {}: {}", name, money(value));
    }

    int inspected = 0;
    auto first_two_expensive = products
        | std::views::filter([&](const Product& product) {
              ++inspected;
              return product.price > 100.0;
          })
        | std::views::take(2);

    std::println("\nFirst two expensive products:");
    for (const auto& product : first_two_expensive) {
        std::println("  {}", product.name);
    }
    std::println("Products inspected lazily: {}", inspected);

    std::println("");
}

} // namespace cpp20_style

// ============================================================================
// C++23 Style: Enhanced range adaptors where available
// ============================================================================

namespace cpp23_style {

#if defined(__cpp_lib_ranges_chunk) && defined(__cpp_lib_ranges_slide) && \
    defined(__cpp_lib_ranges_chunk_by) && defined(__cpp_lib_ranges_enumerate) && \
    defined(__cpp_lib_ranges_zip)
constexpr bool has_cxx23_range_adaptors = true;
#else
constexpr bool has_cxx23_range_adaptors = false;
#endif

void demo() {
    std::println("=== C++23 Style: Enhanced Ranges ===\n");

    auto products = getProducts();

    if constexpr (has_cxx23_range_adaptors) {
#if defined(__cpp_lib_ranges_chunk) && defined(__cpp_lib_ranges_slide) && \
    defined(__cpp_lib_ranges_chunk_by) && defined(__cpp_lib_ranges_enumerate) && \
    defined(__cpp_lib_ranges_zip)
        std::println("Products in groups of 3:");
        for (auto chunk : products | std::views::chunk(3)) {
            std::print("  Group: ");
            std::string separator;
            for (const auto& product : chunk) {
                std::print("{}{}", separator, product.name);
                separator = ", ";
            }
            std::println("");
        }

        std::println("\nPrice comparisons:");
        auto prices = products | std::views::transform(&Product::price);
        for (auto window : prices | std::views::slide(2)) {
            auto it = window.begin();
            const auto first = *it++;
            const auto second = *it;
            std::println("  {} -> {} (diff {})",
                money(first), money(second), money(std::abs(second - first)));
        }

        std::ranges::sort(products, {}, &Product::category);
        std::println("\nProducts grouped by category:");
        for (auto group : products | std::views::chunk_by(
                 [](const Product& left, const Product& right) {
                     return left.category == right.category;
                 })) {
            const auto& first = *group.begin();
            std::println("  {}:", first.category);
            for (const auto& product : group) {
                std::println("    - {}", product.name);
            }
        }

        std::println("\nRanked expensive products:");
        auto expensive = getProducts()
            | std::views::filter([](const Product& product) {
                  return product.price > 100.0;
              });
        for (const auto [index, product] : std::views::enumerate(expensive)) {
            std::println("  #{}: {} ({})",
                index + 1, product.name, money(product.price));
        }

        const std::vector<int> ids = {101, 102, 103, 104, 105};
        auto first_five = getProducts() | std::views::take(5);

        std::println("\nProducts with IDs:");
        for (const auto [id, product] : std::views::zip(ids, first_five)) {
            std::println("  ID {}: {}", id, product.name);
        }
#endif
    } else {
        std::println("This standard library does not expose all C++23 range adaptors yet.");
        std::println("The C++23 version of this report would use:");
        std::println("  - std::views::chunk for fixed-size groups");
        std::println("  - std::views::slide for adjacent comparisons");
        std::println("  - std::views::chunk_by for consecutive category groups");
        std::println("  - std::views::enumerate for ranking");
        std::println("  - std::views::zip for joining product IDs to products");
    }

    std::println("");
}

} // namespace cpp23_style

// ============================================================================
// Main
// ============================================================================

int main() {
    cpp11_style::demo();
    cpp14_style::demo();
    cpp17_style::demo();
    cpp20_style::demo();
    cpp23_style::demo();

    return 0;
}
