#include <iostream>
#include <vector>
#include <chrono>
#include <random>
#include <cassert>

// Generate a random square matrix
std::vector<std::vector<int>> generate_matrix(size_t N) {
    std::vector<std::vector<int>> matrix(N, std::vector<int>(N));
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 10);
    for (size_t i = 0; i < N; ++i)
        for (size_t j = 0; j < N; ++j)
            matrix[i][j] = dis(gen);
    return matrix;
}

// Naive matrix multiplication
std::vector<std::vector<int>> naive_multiply(const std::vector<std::vector<int>>& A,
                                               const std::vector<std::vector<int>>& B) {
    size_t N = A.size();
    std::vector<std::vector<int>> C(N, std::vector<int>(N, 0));
    for (size_t i = 0; i < N; ++i)
        for (size_t j = 0; j < N; ++j)
            for (size_t k = 0; k < N; ++k)
                C[i][j] += A[i][k] * B[k][j];
    return C;
}

// Matrix addition
std::vector<std::vector<int>> add(const std::vector<std::vector<int>>& A,
                                  const std::vector<std::vector<int>>& B) {
    size_t N = A.size();
    std::vector<std::vector<int>> C(N, std::vector<int>(N, 0));
    for (size_t i = 0; i < N; ++i)
        for (size_t j = 0; j < N; ++j)
            C[i][j] = A[i][j] + B[i][j];
    return C;
}

// Matrix subtraction
std::vector<std::vector<int>> subtract(const std::vector<std::vector<int>>& A,
                                         const std::vector<std::vector<int>>& B) {
    size_t N = A.size();
    std::vector<std::vector<int>> C(N, std::vector<int>(N, 0));
    for (size_t i = 0; i < N; ++i)
        for (size_t j = 0; j < N; ++j)
            C[i][j] = A[i][j] - B[i][j];
    return C;
}

// Strassen's matrix multiplication
std::vector<std::vector<int>> strassen_multiply(const std::vector<std::vector<int>>& A,
                                                const std::vector<std::vector<int>>& B) {
    size_t N = A.size();
    if (N == 1) {
        return {{ A[0][0] * B[0][0] }};
    }

    size_t new_size = N / 2;
    // Initialize submatrices
    std::vector<std::vector<int>> A11(new_size, std::vector<int>(new_size)),
                                  A12(new_size, std::vector<int>(new_size)),
                                  A21(new_size, std::vector<int>(new_size)),
                                  A22(new_size, std::vector<int>(new_size)),
                                  B11(new_size, std::vector<int>(new_size)),
                                  B12(new_size, std::vector<int>(new_size)),
                                  B21(new_size, std::vector<int>(new_size)),
                                  B22(new_size, std::vector<int>(new_size));

    // Divide matrices into submatrices
    for (size_t i = 0; i < new_size; ++i) {
        for (size_t j = 0; j < new_size; ++j) {
            A11[i][j] = A[i][j];
            A12[i][j] = A[i][j + new_size];
            A21[i][j] = A[i + new_size][j];
            A22[i][j] = A[i + new_size][j + new_size];

            B11[i][j] = B[i][j];
            B12[i][j] = B[i][j + new_size];
            B21[i][j] = B[i + new_size][j];
            B22[i][j] = B[i + new_size][j + new_size];
        }
    }

    // Compute M1 to M7
    auto M1 = strassen_multiply(add(A11, A22), add(B11, B22));
    auto M2 = strassen_multiply(add(A21, A22), B11);
    auto M3 = strassen_multiply(A11, subtract(B12, B22));
    auto M4 = strassen_multiply(A22, subtract(B21, B11));
    auto M5 = strassen_multiply(add(A11, A12), B22);
    auto M6 = strassen_multiply(subtract(A21, A11), add(B11, B12));
    auto M7 = strassen_multiply(subtract(A12, A22), add(B21, B22));

    // Compute submatrices for result matrix C
    auto C11 = add(subtract(add(M1, M4), M5), M7);
    auto C12 = add(M3, M5);
    auto C21 = add(M2, M4);
    auto C22 = add(subtract(add(M1, M3), M2), M6);

    // Combine submatrices into the final matrix C
    std::vector<std::vector<int>> C(N, std::vector<int>(N, 0));
    for (size_t i = 0; i < new_size; ++i) {
        for (size_t j = 0; j < new_size; ++j) {
            C[i][j] = C11[i][j];
            C[i][j + new_size] = C12[i][j];
            C[i + new_size][j] = C21[i][j];
            C[i + new_size][j + new_size] = C22[i][j];
        }
    }

    return C;
}

int main() {
    size_t N = 128; // Use powers of 2 for simplicity (e.g., 256, 512, 1024)
    std::cout << "Generating two " << N << "x" << N << " matrices...\\n";
    auto A = generate_matrix(N);
    auto B = generate_matrix(N);

    // Benchmark Naive Multiplication
    std::cout << "Starting naive matrix multiplication...\\n";
    auto start = std::chrono::high_resolution_clock::now();
    auto C_naive = naive_multiply(A, B);
    auto end = std::chrono::high_resolution_clock::now();
    auto duration_naive = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    std::cout << "Naive Multiplication Time: " << duration_naive << " ms.\\n";

    // Benchmark Strassen's Multiplication
    std::cout << "Starting Strassen's matrix multiplication...\\n";
    start = std::chrono::high_resolution_clock::now();
    auto C_strassen = strassen_multiply(A, B);
    end = std::chrono::high_resolution_clock::now();
    auto duration_strassen = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    std::cout << "Strassen's Multiplication Time: " << duration_strassen << " ms.\\n";

    // Optional: Verify that both methods produce the same result
    bool correct = true;
    for (size_t i = 0; i < N && correct; ++i) {
        for (size_t j = 0; j < N && correct; ++j) {
            if (C_naive[i][j] != C_strassen[i][j]) {
                correct = false;
                std::cout << "Mismatch at (" << i << ", " << j << "): "
                          << C_naive[i][j] << " vs " << C_strassen[i][j] << "\\n";
            }
        }
    }
    if (correct) {
        std::cout << "Both multiplication methods produced the same result.\\n";
    } else {
        std::cout << "Discrepancy found between multiplication methods!\\n";
    }

    return 0;
}