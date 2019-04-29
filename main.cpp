#include <iostream>
#include <sstream>

#include <cmath>
#include <gmpxx.h>

int zero_count;
mpq_class zero_frequency;

std::string encode(const std::string& input) {
    // calculate encoded interval
    mpq_class C(0), A(1);
    for (auto &c: input) {
        auto QeA = A * zero_frequency;
        if (c == '0') {
            A = QeA;
        } else if (c == '1') {
            C += QeA;
            A -= QeA;
        } else {
            std::cerr << "Invalid input " << input << std::endl;
            return "ERROR";
        }
    }

    // interval end
    mpq_class E = C + A;

#ifdef DEBUG
    std::cerr << "Result interval: [" << C.get_str() << ", " << E.get_str() << ")" << std::endl;
#endif

    // calculate shortest encoding
    mpq_class result(0);
    mpz_class dividend(1);

    while (!(C <= result && result < E)) {
        mpz_class divider = 1 + C.get_num() * dividend / C.get_den();
        result = mpq_class(divider, dividend);
        dividend = 2 * dividend;
    }

    auto result_bin = result.get_num().get_str(2);
    auto result_length = result.get_den().get_str(2).length() - 1;
    // corner case: result == 0
    if (result != 0) {
        result_bin.insert(0, result_length - result_bin.length(), '0');
    }

    auto compress_ratio = (double) result_bin.length() / input.length();

#ifdef DEBUG
    std::cerr << "Length: " << input.length() << " -> "
              << result_bin.length() << " with compress ratio " << compress_ratio << std::endl;
    auto zerof = zero_frequency.get_d();
    auto entropy = - zerof * log2(zerof) - (1 - zerof) * log2(1 - zerof);
    std::cerr << "Entropy: " << entropy << ", ideal length: " << input.length() * entropy << std::endl;
#endif

    return result_bin;
}

std::string decode(std::string input, int length) {
    // construct encoded value
    std::string dividend("1");
    dividend.insert(1, input.length(), '0');
    input += "/" + dividend;
    mpq_class C(input, 2), A(1);

    // recover orig value
    std::string result;
    for (int i = 0; i < length; ++i) {
        auto QeA = A * zero_frequency;
        if (0 <= C && C < QeA) {
            result.push_back('0');
            A = QeA;
        } else {
            result.push_back('1');
            C -= QeA;
            A -= QeA;
        }
    }

    return result;
}

int main(int argc, char* argv[]) {

    if (argc != 4) {
        std::cerr << "Usage: ./arithmetic_coding enc/dec INPUT_FILE OUTPUT_FILE" << std::endl;
        std::cerr << "Use - for standard input/output" << std::endl;
        exit(EXIT_FAILURE);
    }
    if (strcmp(argv[2], "-") != 0) {
        freopen(argv[2], "r", stdin);
    }
    if (strcmp(argv[3], "-") != 0) {
        freopen(argv[3], "w", stdout);
    }

    std::string input;

    if (strcmp(argv[1], "enc") == 0) {
        while(std::getline(std::cin, input)) {
            if(input.length() == 0) continue;
            auto length = input.length();
            zero_count = std::count(input.begin(), input.end(), '0');
            zero_frequency = mpq_class(zero_count, length);
            auto encoded = encode(input);
            std::cout << input.length() << " " << zero_count << " "<< encoded << std::endl;
        }
    } else if (strcmp(argv[1], "dec") == 0) {
        while(std::getline(std::cin, input)) {
            if(input.length() == 0) continue;
            auto ss = std::stringstream(input);
            int length;
            std::string encoded;
            ss >> length >> zero_count >> encoded;
            zero_frequency = mpq_class(zero_count, length);
            std::cout << decode(encoded, length) << std::endl;
        }
    } else {
        std::cerr << "Unsupported mode" << std::endl;
        exit(EXIT_FAILURE);
    }

    return 0;
}