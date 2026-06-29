#ifndef SECURITY_HPP
#define SECURITY_HPP

#include <string>

// Utilitários de segurança para credenciais.
//
// As senhas NUNCA devem ser armazenadas em texto puro. Este módulo deriva um
// hash a partir da senha usando PBKDF2-HMAC-SHA256 com um "salt" aleatório por
// usuário e um número alto de iterações, dificultando ataques de força bruta e
// de tabela arco-íris. A implementação é autocontida (sem dependências
// externas) para manter o build simples.
//
// Formato serializado guardado no banco (campo Senha):
//
//     pbkdf2_sha256$<iteracoes>$<salt_hex>$<hash_hex>
//
namespace Security {

// Gera o registro de hash completo para uma senha em texto puro.
// Cada chamada usa um salt novo, então o resultado muda a cada execução.
std::string hashPassword(const std::string& plainPassword);

// Confere uma senha contra um registro previamente gerado por hashPassword().
// A comparação do hash é feita em tempo constante para evitar timing attacks.
bool verifyPassword(const std::string& plainPassword, const std::string& storedRecord);

// Indica se a string já está no formato de hash deste módulo.
// Útil para migrar contas legadas (senha em texto puro) de forma transparente.
bool isHashed(const std::string& stored);

// Primitivas expostas para testes/reuso.
std::string sha256Hex(const std::string& data);

} // namespace Security

#endif // SECURITY_HPP
