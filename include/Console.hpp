#ifndef CONSOLE_HPP
#define CONSOLE_HPP

#include <string>

// Pequena camada de apresentação para o terminal: cores ANSI e elementos de
// layout (banners, cabeçalhos, linhas chave/valor). Centralizar isto evita
// espalhar códigos de escape pelo código e mantém a UI consistente.
namespace Console {

// Códigos ANSI (constantes para reuso direto em std::cout).
extern const char* RESET;
extern const char* BOLD;
extern const char* DIM;
extern const char* RED;
extern const char* GREEN;
extern const char* YELLOW;
extern const char* BLUE;
extern const char* CYAN;
extern const char* MAGENTA;

// Envolve um texto com uma cor e o RESET.
std::string colorize(const std::string& text, const char* color);

// Banner em destaque (título centralizado entre molduras).
void banner(const std::string& title);

// Cabeçalho de seção sublinhado.
void sectionHeader(const std::string& title);

// Linha "rótulo: valor" alinhada.
void keyValue(const std::string& label, const std::string& value, const char* valueColor = nullptr);

// Mensagens semânticas.
void success(const std::string& msg);
void warning(const std::string& msg);
void error(const std::string& msg);
void info(const std::string& msg);

} // namespace Console

#endif // CONSOLE_HPP
