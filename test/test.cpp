#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include "doctest.h"
#include "huffman.h"

using namespace huffman;

void compare_files(const std::string& filename_1, const std::string& filename_2) {
    std::ifstream file_1(filename_1);
    std::ifstream file_2(filename_2);
    while (true) {
        char a, b;
        file_1.read(&a, 1);
        file_2.read(&b, 1);
        if (file_1.eof() && file_2.eof()) {
            file_1.close();
            file_2.close();
            return;
        }
        else if (file_1.eof() || file_2.eof()) {
            file_1.close();
            file_2.close();
            CHECK(true == false);
            return;
        }
        CHECK(a == b);
    }
}

TEST_CASE("get_bit_0") {
    char a = 0;
    for (std::size_t j = 1; j <= BYTE_SIZE; ++j) {
        CHECK(huffman_decoder::get_bit(a, j) == '0');
    }
}

TEST_CASE("get_bit_255") {
    char b = 255;
    for (std::size_t j = 1; j <= BYTE_SIZE; ++j) {
        CHECK(huffman_decoder::get_bit(b, j) == '1');
    }
}

TEST_CASE("get_bit_64") {
    char c = 64;
    for (std::size_t j = 1; j <= BYTE_SIZE; ++j) {
        CHECK(huffman_decoder::get_bit(c, j) == (j == 2 ? '1' : '0'));
    }
}

TEST_CASE("get_bit_65") {
    char d = 65;
    for (std::size_t j = 1; j <= BYTE_SIZE; ++j) {
        CHECK(huffman_decoder::get_bit(d, j) == ((j == 2 || j == BYTE_SIZE) ? '1' : '0'));
    }
}

TEST_CASE("get_table_00-to-ff") {
    std::ifstream all_bytes("samples/00-to-ff.txt", std::ios::binary);
    std::map<char, std::size_t> all_bytes_table = huffman_encoder::get_table(all_bytes);
    CHECK(all_bytes_table.size() == 256);
    for (std::pair<char, std::size_t> pr : all_bytes_table) {
        CHECK(pr.second == 1);
    }
}

TEST_CASE("get_table_aaaabbbccd") {
    std::ifstream aaaabbbccd("samples/aaaabbbccd.txt", std::ios::binary);
    std::map<char, std::size_t> aaaabbbccd_table = huffman_encoder::get_table(aaaabbbccd);
    CHECK(aaaabbbccd_table.size() == 4);
    CHECK(aaaabbbccd_table['a'] == 4);
    CHECK(aaaabbbccd_table['b'] == 3);
    CHECK(aaaabbbccd_table['c'] == 2);
    CHECK(aaaabbbccd_table['d'] == 1);
}

TEST_CASE("get_table_abacaba") {
    std::ifstream abacaba("samples/abacaba.txt", std::ios::binary);
    std::map<char, std::size_t> abacaba_table = huffman_encoder::get_table(abacaba);
    CHECK(abacaba_table.size() == 3);
    CHECK(abacaba_table['a'] == 4);
    CHECK(abacaba_table['b'] == 2);
    CHECK(abacaba_table['c'] == 1);
}

TEST_CASE("get_table_empty") {
    std::ifstream empty("samples/empty.b", std::ios::binary);
    std::map<char, std::size_t> empty_table = huffman_encoder::get_table(empty);
    CHECK(empty_table.size() == 0);
}

TEST_CASE("get_table_one") {
    std::ifstream one("samples/one.txt", std::ios::binary);
    std::map<char, std::size_t> one_table = huffman_encoder::get_table(one);
    CHECK(one_table.size() == 1);
    CHECK(one_table['a'] == 1);
}

TEST_CASE("build_codes_00-to-ff") {
    std::ifstream all_bytes("samples/00-to-ff.txt", std::ios::binary);
    std::map<char, std::size_t> all_bytes_table = huffman_encoder::get_table(all_bytes);
    huffman_tree all_bytes_tree(all_bytes_table);
    std::map<std::string, char> all_bytes_code_to_symbol = all_bytes_tree.get_code_to_symbol();
    std::map<char, std::string> all_bytes_symbol_to_code = all_bytes_tree.get_symbol_to_code();
    CHECK(all_bytes_code_to_symbol.size() == 256);
    CHECK(all_bytes_symbol_to_code.size() == 256);
    for (std::pair<std::string, char> pr : all_bytes_code_to_symbol) {
        CHECK(pr.first.size() == BYTE_SIZE);
    }
    for (std::pair<char, std::string> pr : all_bytes_symbol_to_code) {
        CHECK(pr.second.size() == BYTE_SIZE);
    }
}

TEST_CASE("build_codes_aaaabbbccd") {
    std::ifstream aaaabbbccd("samples/aaaabbbccd.txt", std::ios::binary);
    std::map<char, std::size_t> aaaabbbccd_table = huffman_encoder::get_table(aaaabbbccd);
    huffman_tree aaaabbbccd_tree(aaaabbbccd_table);
    std::map<std::string, char> aaaabbbccd_code_to_symbol = aaaabbbccd_tree.get_code_to_symbol();
    std::map<char, std::string> aaaabbbccd_symbol_to_code = aaaabbbccd_tree.get_symbol_to_code();
    CHECK(aaaabbbccd_code_to_symbol.size() == 4);
    CHECK(aaaabbbccd_symbol_to_code.size() == 4);
    CHECK(aaaabbbccd_code_to_symbol["0"] == 'a');
    CHECK(aaaabbbccd_code_to_symbol["10"] == 'b');
    CHECK(aaaabbbccd_code_to_symbol["111"] == 'c');
    CHECK(aaaabbbccd_code_to_symbol["110"] == 'd');
    CHECK(aaaabbbccd_symbol_to_code['a'] == "0");
    CHECK(aaaabbbccd_symbol_to_code['b'] == "10");
    CHECK(aaaabbbccd_symbol_to_code['c'] == "111");
    CHECK(aaaabbbccd_symbol_to_code['d'] == "110");
}

TEST_CASE("build_codes_abacaba") {
    std::ifstream abacaba("samples/abacaba.txt", std::ios::binary);
    std::map<char, std::size_t> abacaba_table = huffman_encoder::get_table(abacaba);
    huffman_tree abacaba_tree(abacaba_table);
    std::map<std::string, char> abacaba_code_to_symbol = abacaba_tree.get_code_to_symbol();
    std::map<char, std::string> abacaba_symbol_to_code = abacaba_tree.get_symbol_to_code();
    CHECK(abacaba_code_to_symbol.size() == 3);
    CHECK(abacaba_symbol_to_code.size() == 3);
    CHECK(abacaba_code_to_symbol["1"] == 'a');
    CHECK(abacaba_code_to_symbol["01"] == 'b');
    CHECK(abacaba_code_to_symbol["00"] == 'c');
    CHECK(abacaba_symbol_to_code['a'] == "1");
    CHECK(abacaba_symbol_to_code['b'] == "01");
    CHECK(abacaba_symbol_to_code['c'] == "00");
}

TEST_CASE("build_codes_empty") {
    std::ifstream empty("samples/empty.b", std::ios::binary);
    std::map<char, std::size_t> empty_table = huffman_encoder::get_table(empty);
    huffman_tree empty_tree(empty_table);
    std::map<std::string, char> empty_code_to_symbol = empty_tree.get_code_to_symbol();
    std::map<char, std::string> empty_symbol_to_code = empty_tree.get_symbol_to_code();
    CHECK(empty_code_to_symbol.size() == 0);
    CHECK(empty_symbol_to_code.size() == 0);
}

TEST_CASE("build_codes_one") {
    std::ifstream one("samples/one.txt", std::ios::binary);
    std::map<char, std::size_t> one_table = huffman_encoder::get_table(one);
    huffman_tree one_tree(one_table);
    std::map<std::string, char> one_code_to_symbol = one_tree.get_code_to_symbol();
    std::map<char, std::string> one_symbol_to_code = one_tree.get_symbol_to_code();
    CHECK(one_code_to_symbol.size() == 1);
    CHECK(one_symbol_to_code.size() == 1);
    CHECK(one_code_to_symbol["0"] == 'a');
    CHECK(one_symbol_to_code['a'] == "0");
}

TEST_CASE("encode/decode_00-to-ff") {
    huffman_encoder::encode("samples/00-to-ff.txt", "samples/00-to-ff_compressed.txt");
    huffman_decoder::decode("samples/00-to-ff_compressed.txt", "samples/00-to-ff_decompressed.txt");
    compare_files("samples/00-to-ff.txt", "samples/00-to-ff_decompressed.txt");
    std::remove("samples/00-to-ff_compressed.txt");
    std::remove("samples/00-to-ff_decompressed.txt");
}

TEST_CASE("encode/decode_aaaabbbccd") {
    huffman_encoder::encode("samples/aaaabbbccd.txt", "samples/aaaabbbccd_compressed.txt");
    huffman_decoder::decode("samples/aaaabbbccd_compressed.txt", "samples/aaaabbbccd_decompressed.txt");
    compare_files("samples/aaaabbbccd.txt", "samples/aaaabbbccd_decompressed.txt");
    std::remove("samples/aaaabbbccd_compressed.txt");
    std::remove("samples/aaaabbbccd_decompressed.txt");
}

TEST_CASE("encode/decode_abacaba") {
    huffman_encoder::encode("samples/abacaba.txt", "samples/abacaba_compressed.txt");
    huffman_decoder::decode("samples/abacaba_compressed.txt", "samples/abacaba_decompressed.txt");
    compare_files("samples/abacaba.txt", "samples/abacaba_decompressed.txt");
    std::remove("samples/abacaba_compressed.txt");
    std::remove("samples/abacaba_decompressed.txt");
}

TEST_CASE("encode/decode_empty") {
    huffman_encoder::encode("samples/empty.b", "samples/empty_compressed.b");
    huffman_decoder::decode("samples/empty_compressed.b", "samples/empty_decompressed.b");
    compare_files("samples/empty.b", "samples/empty_decompressed.b");
    std::remove("samples/empty_compressed.b");
    std::remove("samples/empty_decompressed.b");
}

TEST_CASE("encode/decode_one") {
    huffman_encoder::encode("samples/one.txt", "samples/one_compressed.txt");
    huffman_decoder::decode("samples/one_compressed.txt", "samples/one_decompressed.txt");
    compare_files("samples/one.txt", "samples/one_decompressed.txt");
    std::remove("samples/one_compressed.txt");
    std::remove("samples/one_decompressed.txt");
}

TEST_CASE("encode/decode_ran") {
    huffman_encoder::encode("samples/ran.txt", "samples/ran_compressed.txt");
    huffman_decoder::decode("samples/ran_compressed.txt", "samples/ran_decompressed.txt");
    compare_files("samples/ran.txt", "samples/ran_decompressed.txt");
    std::remove("samples/ran_compressed.txt");
    std::remove("samples/ran_decompressed.txt");
}

TEST_CASE("encode/decode_vim") {
    huffman_encoder::encode("samples/vim.txt", "samples/vim_compressed.txt");
    huffman_decoder::decode("samples/vim_compressed.txt", "samples/vim_decompressed.txt");
    compare_files("samples/vim.txt", "samples/vim_decompressed.txt");
    std::remove("samples/vim_compressed.txt");
    std::remove("samples/vim_decompressed.txt");
}
