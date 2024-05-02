#include "huffman.h"
#include <stdexcept>
#include <queue>
#include <vector>
#include <iostream>

using namespace huffman;

huffman_node::huffman_node(std::size_t frequency, char symbol) : frequency(frequency), symbol(symbol) {}

huffman_node::huffman_node(huffman_node* left, huffman_node* right) : left_child(left), right_child(right) {
    frequency = left->frequency + right->frequency;
}

huffman_node::~huffman_node() {
    delete left_child;
    delete right_child;
}

huffman_tree::huffman_tree(const std::map<char, std::size_t>& table) {
    auto cmp = [](const huffman_node* left, const huffman_node* right) {
            return left->frequency > right->frequency;
    };
    std::priority_queue<huffman_node*, std::vector<huffman_node*>, decltype(cmp)> symbols(cmp);

    for (const std::pair<const char, std::size_t>& pair: table)
        symbols.emplace(new huffman_node(pair.second, pair.first));

    while (symbols.size() > 1)
    {
        huffman_node* min1 = symbols.top();
        symbols.pop();
        huffman_node* min2 = symbols.top();
        symbols.pop();
        symbols.emplace(new huffman_node(min1, min2));
    }

    if (!symbols.empty()) 
        root = symbols.top();
}

huffman_tree::~huffman_tree() {
     delete root;
}

void huffman_tree::build_codes(huffman_node* current_node, const std::string& current_code, const std::string& mode) {
    if (!current_node)
        return;

    if (!current_node->left_child && !current_node->right_child) {
        if (current_node == root) {
            if (mode == "encode")
                symbol_to_code[current_node->symbol] = "0";
            else if (mode == "decode")
                code_to_symbol["0"] = current_node->symbol;
        }
        else {
            if (mode == "encode") 
                symbol_to_code[current_node->symbol] = current_code;
            else if (mode == "decode")
                code_to_symbol[current_code] = current_node->symbol;
        }
        return;
    }

    build_codes(current_node->left_child, current_code + "0", mode);
    build_codes(current_node->right_child, current_code + "1", mode);
}

std::map<char, std::string> huffman_tree::get_symbol_to_code() {
    if (symbol_to_code.empty())
        build_codes(root, "", "encode");  
    return symbol_to_code;
}

std::map<std::string, char> huffman_tree::get_code_to_symbol() {
    if (code_to_symbol.empty())
        build_codes(root, "", "decode");
    return code_to_symbol;
}

std::map<char, std::size_t> huffman_encoder::get_table(std::ifstream& file) {
    std::map<char, std::size_t> table;
    while (true) {
        char symbol;
        file.read(&symbol, 1);
        if (file.eof())
            break;
        if (table.find(symbol) == table.end())
            table[symbol] = 0;
        ++table[symbol];
    }
    return table;
}

std::string huffman_encoder::get_encoded_text(std::ifstream& file, std::map<char, std::string>& codes, std::size_t& size_of_file) {
    std::string final_text = "";
    file.clear();
    file.seekg(0);
    while (true) {
        char symbol;
        file.read(&symbol, 1);
        if (file.eof()) {
            break;
        }
        ++size_of_file;
        final_text += codes[symbol];
    }
    while (final_text.size() % BYTE_SIZE != 0)
        final_text += '0';
    file.close();
    return final_text;
}

std::size_t huffman_encoder::write_additional_information(std::ofstream& file, std::map<char, std::size_t>& table, std::size_t size_of_file) {
    std::size_t size_of_table = table.size(), additional_information = 0;
    file.write((char*)&size_of_table, sizeof(size_of_table));
    file.write((char*)&size_of_file, sizeof(size_of_file));
    additional_information += 2 * sizeof(std::size_t);
    for (std::pair<char, std::size_t> symbol : table) {
        file.write(&symbol.first, 1);
        ++additional_information;
        file.write((char*)&symbol.second, sizeof(symbol.second));
        additional_information += sizeof(std::size_t);
    }
    return additional_information;
}

void huffman_encoder::write_encoded_text(std::ofstream& file, const std::string& text) {
    for (std::size_t i = 0; i < text.size(); i += BYTE_SIZE) {
        std::string byte = text.substr(i, BYTE_SIZE);
        char number_byte = std::stoi(byte, nullptr, 2);
        file.write(&number_byte, 1);
    }

}

char huffman_decoder::get_bit(char& byte, std::size_t index) {
    if (index > BYTE_SIZE || index < 1)
        throw std::invalid_argument("1 <= index <= 8");
    return (byte & (1 << (BYTE_SIZE - index))) ? '1' : '0';
}

std::size_t huffman_decoder::get_additional_information(std::ifstream& file, std::map<char, std::size_t>& table, std::size_t& size_of_file) {
    std::size_t size_of_table, additional_information = 0;
    file.read((char*)&size_of_table, sizeof(std::size_t));
    file.read((char*)&size_of_file, sizeof(std::size_t));
    additional_information += 2 * sizeof(std::size_t);
    for (std::size_t i = 0; i < size_of_table; ++i) {
        char symbol;
        file.read(&symbol, 1);
        ++additional_information;
        std::size_t frequency;
        file.read((char*)&frequency, sizeof(frequency));
        additional_information += sizeof(std::size_t);
        table[symbol] = frequency;
    }
    return additional_information;
}

std::size_t huffman_decoder::write_decoded_text(std::ofstream& output_file, std::ifstream& input_file, std::map<std::string, char> codes, std::size_t size_of_file) {
    std::string current_code = "";
    std::size_t count_of_writed_symbols = 0, size_of_compressed_file = 0;
    while (true) {
        char byte;
        input_file.read(&byte, 1);
        if (input_file.eof()) {
            input_file.close();
            output_file.close();
            throw std::invalid_argument("file is corrupted");
        }
        ++size_of_compressed_file;
        for (std::size_t j = 1; j <= BYTE_SIZE; ++j) {
            current_code += get_bit(byte, j);
            if (codes.find(current_code) != codes.end()) {
                output_file << codes[current_code];
                current_code = "";
                if (++count_of_writed_symbols == size_of_file) {
                    input_file.close();
                    output_file.close();
                    return size_of_compressed_file;
                }
            }
        }
    }
}

void huffman_encoder::encode(const std::string& input_filename, const std::string& output_filename) {
    std::ifstream input_file(input_filename, std::ios::binary);
    if (!input_file.is_open())
        throw std::invalid_argument("no file");
    std::map<char, std::size_t> table = get_table(input_file);
    std::map<char, std::string> codes = huffman_tree(table).get_symbol_to_code();
    std::size_t size_of_file = 0;
    std::string final_text = get_encoded_text(input_file, codes, size_of_file);
    std::ofstream output_file(output_filename, std::ios::binary);
    std::size_t additional_information = write_additional_information(output_file, table, size_of_file);
    huffman_encoder::write_encoded_text(output_file, final_text);
    output_file.close();
    std::cout << size_of_file << std::endl << final_text.size() / BYTE_SIZE << std::endl << additional_information << std::endl;
}

void huffman_decoder::decode(const std::string& input_filename, const std::string& output_filename) {
    std::ifstream input_file(input_filename, std::ios::binary);
    if (!input_file.is_open())
        throw std::invalid_argument("no file");
    std::size_t size_of_file;
    std::map<char, std::size_t> table;
    std::size_t additional_information = get_additional_information(input_file, table, size_of_file);
    std::ofstream output_file(output_filename);
    if (size_of_file == 0) {
        output_file.close();
        std::cout << 0 << std::endl << size_of_file << std::endl << additional_information << std::endl;
        return;
    }
    std::map<std::string, char> codes = huffman_tree(table).get_code_to_symbol();
    std::size_t size_of_compressed_file = huffman_decoder::write_decoded_text(output_file, input_file, codes, size_of_file);
    std::cout << size_of_compressed_file << std::endl << size_of_file << std::endl << additional_information << std::endl;
}
