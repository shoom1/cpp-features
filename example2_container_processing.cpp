/**
 * Example 2: Container Processing Evolution
 *
 * This example shows how data processing has evolved from C++11 to C++20/23.
 * We implement common data transformations using different approaches.
 *
 * Task: Process a list of products to find expensive items, apply discount,
 *       and get their names.
 */

#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <numeric>
#include <ranges>      // C++20
#include <print>       // C++23

struct Product {
    std::string name;
    double price;
    std::string category;
    int stock;
};

// Sample data
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

// ============================================================================
// C++11 Style: Manual loops and temporary containers
// ============================================================================

namespace cpp11_style {

void demo() {
    std::cout << "=== C++11 Style: Manual Loops ===\n\n";

    auto products = getProducts();

    // Task: Get names of products > $100 with 10% discount applied
    std::vector<std::string> result;

    for (const auto& product : products) {
        if (product.price > 100.0) {
            double discounted = product.price * 0.9;
            result.push_back(product.name + " ($" +
                           std::to_string(discounted) + ")");
        }
    }

    std::cout << "Expensive products with discount:\n";
    for (const auto& item : result) {
        std::cout << "  " << item << "\n";
    }

    // Calculate total stock value for electronics
    double total = 0.0;
    for (const auto& product : products) {
        if (product.category == "Electronics") {
            total += product.price * product.stock;
        }
    }
    std::cout << "\nTotal electronics value: $" << total << "\n";

    // Problems:
    // - Lots of temporary containers
    // - Manual iteration boilerplate
    // - Less composable
    // - Harder to parallelize
    // - More opportunities for bugs

    std::cout << "\n";
}

} // namespace cpp11_style

// ============================================================================
// C++14/17 Style: Standard algorithms
// ============================================================================

namespace cpp17_style {

void demo() {
    std::cout << "=== C++17 Style: Standard Algorithms ===\n\n";

    auto products = getProducts();

    // Using copy_if and transform
    std::vector<Product> expensive;
    std::copy_if(products.begin(), products.end(),
                std::back_inserter(expensive),
                [](const Product& p) { return p.price > 100.0; });

    std::vector<std::string> result;
    std::transform(expensive.begin(), expensive.end(),
                  std::back_inserter(result),
                  [](const Product& p) {
                      double discounted = p.price * 0.9;
                      return p.name + " ($" + std::to_string(discounted) + ")";
                  });

    std::cout << "Expensive products with discount:\n";
    for (const auto& item : result) {
        std::cout << "  " << item << "\n";
    }

    // Using accumulate with lambda
    double total = std::accumulate(products.begin(), products.end(), 0.0,
        [](double sum, const Product& p) {
            if (p.category == "Electronics") {
                return sum + (p.price * p.stock);
            }
            return sum;
        });

    std::cout << "\nTotal electronics value: $" << total << "\n";

    // Better than raw loops:
    // - Standard vocabulary (copy_if, transform, accumulate)
    // - Less error-prone
    // - Potentially optimized by compiler
    //
    // But still:
    // - Still need temporary containers
    // - Iterator pairs are verbose
    // - Not as composable as it could be

    std::cout << "\n";
}

} // namespace cpp17_style

// ============================================================================
// C++20 Style: Ranges and Views
// ============================================================================

namespace cpp20_style {

void demo() {
    std::println("=== C++20 Style: Ranges and Views ===\n");

    auto products = getProducts();

    // Lazy evaluation with views - no temporary containers!
    auto expensive_discounted = products
        | std::views::filter([](const Product& p) { return p.price > 100.0; })
        | std::views::transform([](const Product& p) {
              double discounted = p.price * 0.9;
              return p.name + " ($" + std::to_string(discounted) + ")";
          });

    std::println("Expensive products with discount:");
    for (const auto& item : expensive_discounted) {
        std::println("  {}", item);
    }

    // More complex pipeline: Top 3 electronics by stock value
    auto top_electronics = products
        | std::views::filter([](const Product& p) {
              return p.category == "Electronics";
          })
        | std::views::transform([](const Product& p) {
              return std::pair{p.name, p.price * p.stock};
          })
        | std::views::take(5);  // Take first 5

    std::println("\nTop electronics by stock value:");
    for (const auto& [name, value] : top_electronics) {
        std::println("  {}: ${:.2f}", name, value);
    }

    // Multiple transformations in one pipeline
    auto cheap_furniture_names = products
        | std::views::filter([](const Product& p) {
              return p.category == "Furniture" && p.price < 300.0;
          })
        | std::views::transform([](const Product& p) { return p.name; });

    std::println("\nAffordable furniture:");
    for (const auto& name : cheap_furniture_names) {
        std::println("  {}", name);
    }

    // Create indices and zip with elements
    auto indexed_expensive = products
        | std::views::filter([](const Product& p) { return p.price > 100.0; })
        | std::views::enumerate;  // C++23

    std::println("\nIndexed expensive products:");
    for (const auto& [idx, product] : indexed_expensive) {
        std::println("  {}: {} - ${}", idx, product.name, product.price);
    }

    // Advantages:
    // - No temporary containers (lazy evaluation)
    // - Composable pipelines
    // - Readable, declarative style
    // - Can short-circuit (take, take_while)
    // - Efficient: only computes what's needed

    std::println("");
}

} // namespace cpp20_style

// ============================================================================
// C++23 Style: Enhanced Ranges
// ============================================================================

namespace cpp23_style {

void demo() {
    std::println("=== C++23 Style: Enhanced Ranges ===\n");

    auto products = getProducts();

    // C++23: views::chunk - process in groups
    std::println("Products in groups of 3:");
    for (const auto& chunk : products | std::views::chunk(3)) {
        std::print("  Group: ");
        for (const auto& p : chunk) {
            std::print("{}, ", p.name);
        }
        std::println("");
    }

    // C++23: views::slide - sliding window
    std::println("\nPrice comparisons (sliding window):");
    auto prices = products
        | std::views::transform([](const Product& p) { return p.price; });

    for (const auto& window : prices | std::views::slide(2)) {
        auto it = window.begin();
        double first = *it++;
        double second = *it;
        std::println("  ${:.2f} -> ${:.2f} (diff: ${:.2f})",
                    first, second, std::abs(second - first));
    }

    // C++23: views::chunk_by - group by predicate
    // Group consecutive products by category
    auto sorted = products;
    std::ranges::sort(sorted, {}, &Product::category);

    std::println("\nProducts grouped by category:");
    for (const auto& group : sorted | std::views::chunk_by(
            [](const Product& a, const Product& b) {
                return a.category == b.category;
            })) {
        auto first = *group.begin();
        std::println("  {}:", first.category);
        for (const auto& p : group) {
            std::println("    - {}", p.name);
        }
    }

    // C++23: views::enumerate - with index
    auto expensive = products
        | std::views::filter([](const Product& p) { return p.price > 100.0; });

    std::println("\nExpensive items with ranking:");
    for (const auto& [rank, product] : expensive | std::views::enumerate) {
        std::println("  #{}: {} (${:.2f})",
                    rank + 1, product.name, product.price);
    }

    // C++23: views::zip - combine multiple ranges
    std::vector<int> ids = {101, 102, 103, 104, 105};
    auto first_five = products | std::views::take(5);

    std::println("\nProducts with IDs:");
    for (const auto& [id, product] : std::views::zip(ids, first_five)) {
        std::println("  ID {}: {}", id, product.name);
    }

    // Advanced: Multiple transformations with zip_transform (C++23)
    std::vector<double> discounts = {0.1, 0.15, 0.2, 0.05, 0.25};

    std::println("\nProducts with variable discounts:");
    for (const auto& [product, discount] : std::views::zip(first_five, discounts)) {
        double final_price = product.price * (1.0 - discount);
        std::println("  {}: ${:.2f} ({}% off)",
                    product.name, final_price, static_cast<int>(discount * 100));
    }

    std::println("");
}

} // namespace cpp23_style

// ============================================================================
// Performance Comparison
// ============================================================================

namespace performance {

void demo() {
    std::println("=== Performance Characteristics ===\n");

    auto products = getProducts();

    // C++11: Creates intermediate containers
    std::println("C++11 approach:");
    std::println("  - Creates 2 temporary vectors");
    std::println("  - Processes all elements even if only need first few");
    std::println("  - Memory allocations for each container");

    // C++17: Still creates intermediates
    std::println("\nC++17 approach:");
    std::println("  - Creates 1-2 temporary vectors");
    std::println("  - Still processes everything");
    std::println("  - Slightly better with algorithm composition");

    // C++20/23: Lazy evaluation
    std::println("\nC++20/23 approach:");
    std::println("  - Zero intermediate containers (views)");
    std::println("  - Lazy evaluation - only compute what's needed");
    std::println("  - Can short-circuit with take()");
    std::println("  - Composable without overhead");

    // Demonstrate lazy evaluation
    std::println("\nLazy evaluation example:");
    int count = 0;
    auto lazy_view = products
        | std::views::filter([&count](const Product& p) {
              count++;
              std::println("  Checking product #{}: {}", count, p.name);
              return p.price > 100.0;
          })
        | std::views::take(3);  // Only need 3 items

    std::println("\nConsuming view (will stop after finding 3 matches):");
    for (const auto& p : lazy_view) {
        std::println("  Found: {}", p.name);
    }

    std::println("\nTotal products checked: {} out of {}",
                count, products.size());
    std::println("(Stopped early thanks to lazy evaluation!)");
}

} // namespace performance

// ============================================================================
// Main
// ============================================================================

int main() {
    cpp11_style::demo();
    cpp17_style::demo();
    cpp20_style::demo();
    cpp23_style::demo();
    performance::demo();

    return 0;
}
