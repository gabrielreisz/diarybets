#include "../include/GlucoseAnalytics.hpp"
#include "../include/Console.hpp"

#include <sqlite3.h>
#include <algorithm>
#include <cmath>
#include <cstdint>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>

namespace {

// Faixas de referência (mg/dL) - consenso clínico para tempo no alvo.
constexpr double kRangeLow = 70.0;
constexpr double kRangeHigh = 180.0;

// Converte "DD/MM/AAAA" + "HH:MM:SS" numa chave numérica AAAAMMDDHHMMSS,
// permitindo ordenação cronológica correta (ordenar a string DD/MM/AAAA
// diretamente daria resultado errado).
long long makeSortKey(const std::string& date, const std::string& time) {
    int d = 0, m = 0, y = 0, hh = 0, mm = 0, ss = 0;
    std::sscanf(date.c_str(), "%d/%d/%d", &d, &m, &y);
    std::sscanf(time.c_str(), "%d:%d:%d", &hh, &mm, &ss);
    return ((long long)y) * 10000000000LL + (long long)m * 100000000LL +
           (long long)d * 1000000LL + (long long)hh * 10000LL +
           (long long)mm * 100LL + ss;
}

std::string fmt(double v, int prec = 1) {
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(prec) << v;
    return oss.str();
}

// Barra horizontal proporcional (0..1) para o painel de Tempo no Alvo.
std::string bar(double fraction, int width, const char* color) {
    if (fraction < 0) fraction = 0;
    if (fraction > 1) fraction = 1;
    int filled = static_cast<int>(std::lround(fraction * width));
    std::string out = color;
    out += std::string(filled, '#');
    out += Console::RESET;
    out += Console::DIM;
    out += std::string(width - filled, '.');
    out += Console::RESET;
    return out;
}

} // namespace

std::vector<GlucoseReading> GlucoseAnalytics::loadReadings(int patientId) {
    std::vector<GlucoseReading> readings;
    sqlite3* db = nullptr;
    sqlite3_stmt* stmt = nullptr;

    try {
        if (sqlite3_open("database.db", &db) != SQLITE_OK) {
            Console::error(std::string("Erro ao abrir database: ") + sqlite3_errmsg(db));
            if (db) sqlite3_close(db);
            return readings;
        }

        const char* query =
            "SELECT rg.NivelGlicose, rg.Jejum, rs.Data, rs.Hora "
            "FROM RegistroGlicose rg "
            "JOIN RegistroSaude rs ON rg.RegistroSaude = rs.Id "
            "WHERE rs.Paciente = ?";

        if (sqlite3_prepare_v2(db, query, -1, &stmt, nullptr) != SQLITE_OK) {
            Console::error(std::string("Erro ao preparar consulta: ") + sqlite3_errmsg(db));
            sqlite3_close(db);
            return readings;
        }

        sqlite3_bind_int(stmt, 1, patientId);

        while (sqlite3_step(stmt) == SQLITE_ROW) {
            GlucoseReading r;
            r.level = sqlite3_column_double(stmt, 0);

            // Jejum guardado como BLOB (bool) ou, em dados antigos, INTEGER.
            r.fasting = false;
            if (sqlite3_column_type(stmt, 1) == SQLITE_BLOB) {
                const void* blob = sqlite3_column_blob(stmt, 1);
                if (blob && sqlite3_column_bytes(stmt, 1) >= static_cast<int>(sizeof(bool)))
                    r.fasting = *static_cast<const bool*>(blob);
            } else if (sqlite3_column_type(stmt, 1) == SQLITE_INTEGER) {
                r.fasting = sqlite3_column_int(stmt, 1) != 0;
            }

            const unsigned char* dataText = sqlite3_column_text(stmt, 2);
            const unsigned char* horaText = sqlite3_column_text(stmt, 3);
            r.date = dataText ? reinterpret_cast<const char*>(dataText) : "";
            r.time = horaText ? reinterpret_cast<const char*>(horaText) : "";
            r.sortKey = makeSortKey(r.date, r.time);

            readings.push_back(r);
        }
        sqlite3_finalize(stmt);
        sqlite3_close(db);
    } catch (const std::exception& e) {
        Console::error(std::string("Exceção ao carregar glicose: ") + e.what());
        if (stmt) sqlite3_finalize(stmt);
        if (db) sqlite3_close(db);
        return {};
    }

    // Ordena cronologicamente (ascendente) - O(n log n).
    std::sort(readings.begin(), readings.end(),
              [](const GlucoseReading& a, const GlucoseReading& b) { return a.sortKey < b.sortKey; });
    return readings;
}

GlucoseStats GlucoseAnalytics::compute(const std::vector<GlucoseReading>& readings) {
    GlucoseStats s;
    s.count = static_cast<int>(readings.size());
    if (s.count == 0) return s;

    // Passagem única para soma, min, max e contagem por faixa - O(n).
    double sum = 0.0;
    int below = 0, inRange = 0, above = 0;
    s.minValue = readings[0].level;
    s.maxValue = readings[0].level;
    for (const auto& r : readings) {
        sum += r.level;
        s.minValue = std::min(s.minValue, r.level);
        s.maxValue = std::max(s.maxValue, r.level);
        if (r.level < kRangeLow) ++below;
        else if (r.level > kRangeHigh) ++above;
        else ++inRange;
    }
    s.mean = sum / s.count;

    // Mediana (cópia ordenada) - O(n log n).
    std::vector<double> sorted;
    sorted.reserve(readings.size());
    for (const auto& r : readings) sorted.push_back(r.level);
    std::sort(sorted.begin(), sorted.end());
    if (s.count % 2 == 1) {
        s.median = sorted[s.count / 2];
    } else {
        s.median = (sorted[s.count / 2 - 1] + sorted[s.count / 2]) / 2.0;
    }

    // Desvio padrão amostral e coeficiente de variação.
    if (s.count > 1) {
        double sq = 0.0;
        for (const auto& r : readings) sq += (r.level - s.mean) * (r.level - s.mean);
        s.stddev = std::sqrt(sq / (s.count - 1));
        s.cv = (s.mean != 0.0) ? (s.stddev / s.mean) * 100.0 : 0.0;
    }

    // Faixas (percentuais).
    s.tirBelow = 100.0 * below / s.count;
    s.tirInRange = 100.0 * inRange / s.count;
    s.tirAbove = 100.0 * above / s.count;

    // Indicadores derivados da média.
    s.gmi = 3.31 + 0.02392 * s.mean;
    s.ea1c = (s.mean + 46.7) / 28.7;

    // Tendência: regressão linear (mínimos quadrados) de nível x índice
    // cronológico. slope = Σ(x-x̄)(y-ȳ) / Σ(x-x̄)². - O(n).
    if (s.count > 1) {
        double xMean = (s.count - 1) / 2.0;
        double numerator = 0.0, denominator = 0.0;
        for (int i = 0; i < s.count; ++i) {
            double dx = i - xMean;
            numerator += dx * (readings[i].level - s.mean);
            denominator += dx * dx;
        }
        s.trendSlope = (denominator != 0.0) ? numerator / denominator : 0.0;
    }

    return s;
}

std::string GlucoseAnalytics::sparkline(const std::vector<double>& values) {
    if (values.empty()) return "";
    static const char* blocks[] = {"▁", "▂", "▃", "▄",
                                   "▅", "▆", "▇", "█"};
    double lo = *std::min_element(values.begin(), values.end());
    double hi = *std::max_element(values.begin(), values.end());
    double range = hi - lo;
    std::string out;
    for (double v : values) {
        int idx = (range > 0) ? static_cast<int>((v - lo) / range * 7.0 + 0.5) : 0;
        if (idx < 0) idx = 0;
        if (idx > 7) idx = 7;
        out += blocks[idx];
    }
    return out;
}

std::string GlucoseAnalytics::classify(double level, bool fasting) {
    if (level < kRangeLow) return "Hipoglicemia";
    if (level > kRangeHigh) return "Hiperglicemia";
    // Dentro do alvo geral; refina para jejum (meta mais estrita).
    if (fasting && level > 130.0) return "Acima do alvo (jejum)";
    return "No alvo";
}

void GlucoseAnalytics::printReport(int patientId) {
    auto readings = loadReadings(patientId);

    Console::banner("ANALISE DE GLICOSE");

    if (readings.empty()) {
        Console::warning("Nenhum registro de glicose encontrado para gerar a analise.");
        return;
    }

    GlucoseStats s = compute(readings);

    Console::sectionHeader("Estatisticas descritivas (mg/dL)");
    Console::keyValue("Leituras", std::to_string(s.count));
    Console::keyValue("Media", fmt(s.mean));
    Console::keyValue("Mediana", fmt(s.median));
    Console::keyValue("Desvio padrao", fmt(s.stddev));
    Console::keyValue("Minimo / Maximo", fmt(s.minValue) + " / " + fmt(s.maxValue));

    // Variabilidade: alvo clínico de CV <= 36%.
    const char* cvColor = (s.cv <= 36.0) ? Console::GREEN : Console::YELLOW;
    Console::keyValue("Variabilidade (CV)", fmt(s.cv) + "%", cvColor);

    Console::sectionHeader("Controle glicemico estimado");
    // GMI/eA1c: < 7% costuma ser a meta para muitos adultos.
    const char* a1cColor = (s.gmi < 7.0) ? Console::GREEN : Console::YELLOW;
    Console::keyValue("GMI (indicador de gestao)", fmt(s.gmi, 2) + "%", a1cColor);
    Console::keyValue("A1c estimada (eA1c)", fmt(s.ea1c, 2) + "%", a1cColor);

    Console::sectionHeader("Tempo no alvo (TIR)");
    std::cout << "  " << std::left << std::setw(14) << "No alvo"
              << bar(s.tirInRange / 100.0, 24, Console::GREEN) << " " << fmt(s.tirInRange) << "%\n";
    std::cout << "  " << std::left << std::setw(14) << "Abaixo (<70)"
              << bar(s.tirBelow / 100.0, 24, Console::RED) << " " << fmt(s.tirBelow) << "%\n";
    std::cout << "  " << std::left << std::setw(14) << "Acima (>180)"
              << bar(s.tirAbove / 100.0, 24, Console::YELLOW) << " " << fmt(s.tirAbove) << "%\n";
    if (s.tirInRange >= 70.0)
        Console::success("Tempo no alvo dentro da meta recomendada (>= 70%).");
    else
        Console::warning("Tempo no alvo abaixo da meta recomendada (>= 70%).");

    Console::sectionHeader("Tendencia e serie temporal");
    std::string dir;
    const char* trendColor;
    if (s.trendSlope > 0.5)      { dir = "subindo";   trendColor = Console::YELLOW; }
    else if (s.trendSlope < -0.5) { dir = "descendo";  trendColor = Console::GREEN; }
    else                          { dir = "estavel";   trendColor = Console::CYAN; }
    Console::keyValue("Tendencia", dir + " (" + fmt(s.trendSlope, 2) + " mg/dL por leitura)", trendColor);

    std::vector<double> series;
    for (const auto& r : readings) series.push_back(r.level);
    std::cout << "  " << std::left << std::setw(28) << "Serie (antiga -> recente):"
              << Console::CYAN << sparkline(series) << Console::RESET << "\n";

    std::cout << "\n" << Console::DIM
              << "Referencias: TIR 70-180 mg/dL; GMI (Bergenstal 2018); eA1c (ADAG 2008).\n"
              << "Valores informativos - nao substituem avaliacao medica." << Console::RESET << "\n";
}

bool GlucoseAnalytics::exportCsv(int patientId, const std::string& path) {
    auto readings = loadReadings(patientId);
    if (readings.empty()) {
        Console::warning("Nenhum registro de glicose para exportar.");
        return false;
    }

    std::ofstream out(path);
    if (!out.is_open()) {
        Console::error("Nao foi possivel abrir o arquivo para escrita: " + path);
        return false;
    }

    out << "data,hora,glicose_mg_dl,jejum,classificacao\n";
    for (const auto& r : readings) {
        out << r.date << ',' << r.time << ',' << r.level << ','
            << (r.fasting ? "sim" : "nao") << ','
            << classify(r.level, r.fasting) << '\n';
    }
    out.close();

    Console::success("Exportados " + std::to_string(readings.size()) +
                     " registros para '" + path + "'.");
    return true;
}
