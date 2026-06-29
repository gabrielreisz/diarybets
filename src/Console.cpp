#include "../include/Console.hpp"

#include <iostream>
#include <iomanip>

namespace Console {

const char* RESET   = "\033[0m";
const char* BOLD    = "\033[1m";
const char* DIM     = "\033[2m";
const char* RED     = "\033[1;31m";
const char* GREEN   = "\033[1;32m";
const char* YELLOW  = "\033[1;33m";
const char* BLUE    = "\033[1;34m";
const char* CYAN    = "\033[1;36m";
const char* MAGENTA = "\033[1;35m";

std::string colorize(const std::string& text, const char* color) {
    return std::string(color) + text + RESET;
}

void banner(const std::string& title) {
    const int width = 60;
    std::string line(width, '=');
    int pad = (width - static_cast<int>(title.size())) / 2;
    if (pad < 0) pad = 0;

    std::cout << CYAN << line << RESET << "\n";
    std::cout << CYAN << std::string(pad, ' ') << BOLD << title << RESET << "\n";
    std::cout << CYAN << line << RESET << "\n";
}

void sectionHeader(const std::string& title) {
    std::cout << "\n" << BOLD << title << RESET << "\n";
    std::cout << DIM << std::string(title.size(), '-') << RESET << "\n";
}

void keyValue(const std::string& label, const std::string& value, const char* valueColor) {
    std::cout << "  " << std::left << std::setw(28) << (label + ":");
    if (valueColor) std::cout << valueColor;
    std::cout << value;
    if (valueColor) std::cout << RESET;
    std::cout << "\n";
}

void success(const std::string& msg) { std::cout << GREEN  << "[OK] "    << msg << RESET << "\n"; }
void warning(const std::string& msg) { std::cout << YELLOW << "[AVISO] " << msg << RESET << "\n"; }
void error(const std::string& msg)   { std::cout << RED    << "[ERRO] "  << msg << RESET << "\n"; }
void info(const std::string& msg)    { std::cout << BLUE   << "[INFO] "  << msg << RESET << "\n"; }

} // namespace Console
