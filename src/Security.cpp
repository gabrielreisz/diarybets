#include "../include/Security.hpp"

#include <array>
#include <cstdint>
#include <cstring>
#include <random>
#include <sstream>
#include <iomanip>
#include <stdexcept>
#include <vector>

// ============================================================================
// SHA-256 (FIPS 180-4) - implementação autocontida
// ============================================================================
namespace {

constexpr std::array<uint32_t, 64> kK = {
    0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1,
    0x923f82a4, 0xab1c5ed5, 0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3,
    0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174, 0xe49b69c1, 0xefbe4786,
    0x0fc19dc6, 0x240ca1cc, 0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
    0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147,
    0x06ca6351, 0x14292967, 0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13,
    0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85, 0xa2bfe8a1, 0xa81a664b,
    0xc24b8b70, 0xc76c51a3, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
    0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a,
    0x5b9cca4f, 0x682e6ff3, 0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208,
    0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2};

inline uint32_t rotr(uint32_t x, uint32_t n) { return (x >> n) | (x << (32 - n)); }

// Processa a mensagem e devolve o digest de 32 bytes.
std::array<uint8_t, 32> sha256Raw(const uint8_t* data, size_t len) {
    uint32_t h[8] = {0x6a09e667, 0xbb67ae85, 0x3c6ef372, 0xa54ff53a,
                     0x510e527f, 0x9b05688c, 0x1f83d9ab, 0x5be0cd19};

    // Padding (mensagem + 0x80 + zeros + tamanho em bits big-endian).
    std::vector<uint8_t> msg(data, data + len);
    uint64_t bitLen = static_cast<uint64_t>(len) * 8;
    msg.push_back(0x80);
    while (msg.size() % 64 != 56) msg.push_back(0x00);
    for (int i = 7; i >= 0; --i) msg.push_back(static_cast<uint8_t>(bitLen >> (i * 8)));

    for (size_t chunk = 0; chunk < msg.size(); chunk += 64) {
        uint32_t w[64];
        for (int i = 0; i < 16; ++i) {
            w[i] = (static_cast<uint32_t>(msg[chunk + i * 4]) << 24) |
                   (static_cast<uint32_t>(msg[chunk + i * 4 + 1]) << 16) |
                   (static_cast<uint32_t>(msg[chunk + i * 4 + 2]) << 8) |
                   (static_cast<uint32_t>(msg[chunk + i * 4 + 3]));
        }
        for (int i = 16; i < 64; ++i) {
            uint32_t s0 = rotr(w[i - 15], 7) ^ rotr(w[i - 15], 18) ^ (w[i - 15] >> 3);
            uint32_t s1 = rotr(w[i - 2], 17) ^ rotr(w[i - 2], 19) ^ (w[i - 2] >> 10);
            w[i] = w[i - 16] + s0 + w[i - 7] + s1;
        }

        uint32_t a = h[0], b = h[1], c = h[2], d = h[3];
        uint32_t e = h[4], f = h[5], g = h[6], hh = h[7];

        for (int i = 0; i < 64; ++i) {
            uint32_t S1 = rotr(e, 6) ^ rotr(e, 11) ^ rotr(e, 25);
            uint32_t ch = (e & f) ^ (~e & g);
            uint32_t t1 = hh + S1 + ch + kK[i] + w[i];
            uint32_t S0 = rotr(a, 2) ^ rotr(a, 13) ^ rotr(a, 22);
            uint32_t maj = (a & b) ^ (a & c) ^ (b & c);
            uint32_t t2 = S0 + maj;
            hh = g; g = f; f = e; e = d + t1;
            d = c; c = b; b = a; a = t1 + t2;
        }
        h[0] += a; h[1] += b; h[2] += c; h[3] += d;
        h[4] += e; h[5] += f; h[6] += g; h[7] += hh;
    }

    std::array<uint8_t, 32> digest{};
    for (int i = 0; i < 8; ++i) {
        digest[i * 4] = static_cast<uint8_t>(h[i] >> 24);
        digest[i * 4 + 1] = static_cast<uint8_t>(h[i] >> 16);
        digest[i * 4 + 2] = static_cast<uint8_t>(h[i] >> 8);
        digest[i * 4 + 3] = static_cast<uint8_t>(h[i]);
    }
    return digest;
}

constexpr size_t kBlock = 64;   // bloco SHA-256 (bytes)
constexpr size_t kHashLen = 32; // saída SHA-256 (bytes)

// HMAC-SHA256(key, message).
std::array<uint8_t, kHashLen> hmacSha256(const std::vector<uint8_t>& key,
                                         const std::vector<uint8_t>& message) {
    std::vector<uint8_t> k = key;
    if (k.size() > kBlock) {
        auto hk = sha256Raw(k.data(), k.size());
        k.assign(hk.begin(), hk.end());
    }
    k.resize(kBlock, 0x00);

    std::vector<uint8_t> ipad(kBlock), opad(kBlock);
    for (size_t i = 0; i < kBlock; ++i) {
        ipad[i] = k[i] ^ 0x36;
        opad[i] = k[i] ^ 0x5c;
    }

    std::vector<uint8_t> inner(ipad);
    inner.insert(inner.end(), message.begin(), message.end());
    auto innerHash = sha256Raw(inner.data(), inner.size());

    std::vector<uint8_t> outer(opad);
    outer.insert(outer.end(), innerHash.begin(), innerHash.end());
    return sha256Raw(outer.data(), outer.size());
}

// PBKDF2-HMAC-SHA256 produzindo dkLen bytes de chave derivada.
std::vector<uint8_t> pbkdf2(const std::string& password,
                            const std::vector<uint8_t>& salt,
                            uint32_t iterations, size_t dkLen) {
    std::vector<uint8_t> key(password.begin(), password.end());
    std::vector<uint8_t> derived;
    uint32_t blocks = static_cast<uint32_t>((dkLen + kHashLen - 1) / kHashLen);

    for (uint32_t block = 1; block <= blocks; ++block) {
        std::vector<uint8_t> saltBlock = salt;
        saltBlock.push_back(static_cast<uint8_t>(block >> 24));
        saltBlock.push_back(static_cast<uint8_t>(block >> 16));
        saltBlock.push_back(static_cast<uint8_t>(block >> 8));
        saltBlock.push_back(static_cast<uint8_t>(block));

        auto u = hmacSha256(key, saltBlock);
        auto t = u;
        for (uint32_t i = 1; i < iterations; ++i) {
            u = hmacSha256(key, std::vector<uint8_t>(u.begin(), u.end()));
            for (size_t j = 0; j < kHashLen; ++j) t[j] ^= u[j];
        }
        derived.insert(derived.end(), t.begin(), t.end());
    }
    derived.resize(dkLen);
    return derived;
}

std::string toHex(const uint8_t* data, size_t len) {
    std::ostringstream oss;
    oss << std::hex << std::setfill('0');
    for (size_t i = 0; i < len; ++i) oss << std::setw(2) << static_cast<int>(data[i]);
    return oss.str();
}

std::vector<uint8_t> fromHex(const std::string& hex) {
    if (hex.size() % 2 != 0) throw std::invalid_argument("hex length must be even");
    std::vector<uint8_t> out(hex.size() / 2);
    for (size_t i = 0; i < out.size(); ++i)
        out[i] = static_cast<uint8_t>(std::stoi(hex.substr(i * 2, 2), nullptr, 16));
    return out;
}

// Comparação em tempo constante (não retorna cedo ao primeiro byte diferente).
bool constantTimeEquals(const std::string& a, const std::string& b) {
    if (a.size() != b.size()) return false;
    unsigned char diff = 0;
    for (size_t i = 0; i < a.size(); ++i)
        diff |= static_cast<unsigned char>(a[i]) ^ static_cast<unsigned char>(b[i]);
    return diff == 0;
}

constexpr char kPrefix[] = "pbkdf2_sha256";
constexpr uint32_t kIterations = 120000;
constexpr size_t kSaltLen = 16;
constexpr size_t kDerivedLen = 32;

} // namespace

// ============================================================================
// API pública
// ============================================================================
namespace Security {

std::string sha256Hex(const std::string& data) {
    auto digest = sha256Raw(reinterpret_cast<const uint8_t*>(data.data()), data.size());
    return toHex(digest.data(), digest.size());
}

std::string hashPassword(const std::string& plainPassword) {
    // Salt criptograficamente aleatório por usuário.
    std::random_device rd;
    std::vector<uint8_t> salt(kSaltLen);
    for (auto& byte : salt) byte = static_cast<uint8_t>(rd() & 0xff);

    auto derived = pbkdf2(plainPassword, salt, kIterations, kDerivedLen);

    std::ostringstream oss;
    oss << kPrefix << '$' << kIterations << '$'
        << toHex(salt.data(), salt.size()) << '$'
        << toHex(derived.data(), derived.size());
    return oss.str();
}

bool isHashed(const std::string& stored) {
    return stored.rfind(std::string(kPrefix) + "$", 0) == 0;
}

bool verifyPassword(const std::string& plainPassword, const std::string& storedRecord) {
    if (!isHashed(storedRecord)) return false;

    // pbkdf2_sha256$<iter>$<salt_hex>$<hash_hex>
    std::vector<std::string> parts;
    std::stringstream ss(storedRecord);
    std::string item;
    while (std::getline(ss, item, '$')) parts.push_back(item);
    if (parts.size() != 4) return false;

    try {
        uint32_t iterations = static_cast<uint32_t>(std::stoul(parts[1]));
        std::vector<uint8_t> salt = fromHex(parts[2]);
        const std::string& expectedHex = parts[3];

        auto derived = pbkdf2(plainPassword, salt, iterations, expectedHex.size() / 2);
        std::string computedHex = toHex(derived.data(), derived.size());
        return constantTimeEquals(computedHex, expectedHex);
    } catch (const std::exception&) {
        return false;
    }
}

} // namespace Security
