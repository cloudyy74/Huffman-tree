#pragma once

#include <fstream>
#include <map>
#include <string>

namespace huffman {
    const std::size_t BYTE_SIZE = 8;


    class huffman_encoder {
    public:
        static void encode(const std::string& input_filename, const std::string& output_filename);
        static std::map<char, std::size_t> get_table(std::ifstream& file);
        static std::string get_encoded_text(std::ifstream& file, std::map<char, std::string>& codes, std::size_t& size_of_file);
        static std::size_t write_additional_information(std::ofstream& file, std::map<char, std::size_t>& table, std::size_t size_of_file);
        static void write_encoded_text(std::ofstream& file, const std::string& text);
    };

    class huffman_decoder {
    public:
        static void decode(const std::string& input_filename, const std::string& output_filename);
        static char get_bit(char& byte, std::size_t index);
        static std::size_t get_additional_information(std::ifstream& file, std::map<char, std::size_t>& table, std::size_t& size_of_file);
        static std::size_t write_decoded_text(std::ofstream& output_file, std::ifstream& input_file, std::map<std::string, char> codes, std::size_t size_of_file);
    };
    
    class huffman_node {
    public:
        huffman_node(std::size_t frequency, char symbol);
        huffman_node(huffman_node* left, huffman_node* right);
        ~huffman_node();
        huffman_node* left_child = nullptr;
        huffman_node* right_child = nullptr;
        std::size_t frequency;
        char symbol;
    }; 

    class huffman_tree {
    public:
        huffman_tree(const std::map<char, std::size_t>& table);
        ~huffman_tree();

        std::map<char, std::string> get_symbol_to_code();
        std::map<std::string, char> get_code_to_symbol();
    private:
        huffman_node* root = nullptr;
        std::map<char, std::string> symbol_to_code;

        std::map<std::string, char> code_to_symbol;
        void build_codes(huffman_node* current_node, const std::string& current_code, const std::string& mode);
    };
}
