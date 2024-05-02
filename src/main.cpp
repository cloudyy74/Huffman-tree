#include "huffman.h"

int main(int argc, char* argv[]) {
	if (argc != 6) {
		exit(1);
	}

	std::string input_filename, output_filename, type_flag;
	for (int i = 1; i < 6; ++i) {
		std::string flag = std::string(argv[i]);
		if (flag == "-c" || flag == "-u") {
			type_flag = flag;
		}	
		else if (flag == "-f" || flag == "--file") {
			input_filename = std::string(argv[++i]);
		}
		else if (flag == "-o" || flag == "--output") {
			output_filename = std::string(argv[++i]);
		}
		else {
			exit(1);
		}
	}

	try {
		if (type_flag == "-c") {
			huffman::huffman_encoder::encode(input_filename, output_filename);
		}
		else if (type_flag == "-u") {
			huffman::huffman_decoder::decode(input_filename, output_filename);
		}
	}
	catch(...) {
		exit(1);
	}

	return 0;
}
