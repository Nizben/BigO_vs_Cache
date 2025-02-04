#include <iostream>
#include <vector>
#include <unordered_set>
#include <chrono>
#include <random>
#include <algorithm>

// Generate a sorted array of unique integers
std::vector<int> generate_sorted_array(size_t size) {
    std::vector<int> array;
    array.reserve(size);
    for (size_t i = 0; i < size; ++i) {
        array.push_back(static_cast<int>(i));
    }
    // Shuffle to simulate random access patterns
    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(array.begin(), array.end(), g);
    // Sort for binary search
    std::sort(array.begin(), array.end());
    return array;
}

// Generate a hash set from the array
std::unordered_set<int> generate_hashset(const std::vector<int>& array) {
    return std::unordered_set<int>(array.begin(), array.end());
}

// Binary search on a sorted array
bool binary_search_array(const std::vector<int>& array, int key) {
    return std::binary_search(array.begin(), array.end(), key);
}

int main() {
    size_t N = 3'000'000; // 3 million elements to fit into L2 cache
    std::cout << "Generating sorted array with " << N << " elements...\n";
    auto array = generate_sorted_array(N);

    std::cout << "Generating hash set...\n";
    auto hashset = generate_hashset(array);

    // Prepare lookup keys (half existing, half non-existing)
    std::vector<int> keys;
    keys.reserve(N);
    for (size_t i = 0; i < N / 2; ++i) {
        keys.push_back(static_cast<int>(i)); // Existing keys
    }
    for (size_t i = N; i < N + (N / 2); ++i) {
        keys.push_back(static_cast<int>(i)); // Non-existing keys
    }

    // Shuffle keys to simulate random access
    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(keys.begin(), keys.end(), g);

    // Benchmark binary search on array
    std::cout << "Starting binary search on array...\n";
    auto start = std::chrono::high_resolution_clock::now();
    size_t found_array = 0;
    for (const auto& key : keys) {
        if (binary_search_array(array, key)) {
            found_array++;
        }
    }
    auto end = std::chrono::high_resolution_clock::now();
    auto duration_array = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    std::cout << "Array: Found " << found_array << " keys in " << duration_array << " ms.\n";

    // Benchmark hash set lookups
    std::cout << "Starting hash set lookups...\n";
    start = std::chrono::high_resolution_clock::now();
    size_t found_hashset = 0;
    for (const auto& key : keys) {
        if (hashset.find(key) != hashset.end()) {
            found_hashset++;
        }
    }
    end = std::chrono::high_resolution_clock::now();
    auto duration_hashset = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    std::cout << "Hash Set: Found " << found_hashset << " keys in " << duration_hashset << " ms.\n";

    // Verification
    if (found_array == found_hashset) {
        std::cout << "Both methods found the same number of keys.\n";
    } else {
        std::cout << "Discrepancy in found keys!\n";
    }

    return 0;
}
