#include <iostream>

#include "sms/io/io.hpp"


int main(int args, char *argv[]) {
    if (args < 3) {
        throw std::runtime_error(
                "Not enough arguments given.\nUsage:\n1 - type of check to do, either mc or bq\n2 - path to file to check");
    }

    std::string type = std::string(argv[1]);
    std::string file = std::string(argv[2]);

    if (type == "mc") {
        try {
            fileToMcObj(file);
            return 0;
        }
        catch (const std::exception &e) {
            std::cerr << "Error with file  " << file << std::endl;
            std::cerr << e.what() << std::endl;
            return 1;
        }
    } else if (type == "bq") {
        try {
            fileToBqObj(file);
            return 0;
        }
        catch (const std::exception &e) {
            std::cerr << "Error with file  " << file << std::endl;
            std::cerr << e.what() << std::endl;
            return 1;
        }
    } else {
        throw std::runtime_error("Unknown check type.");
    }
}