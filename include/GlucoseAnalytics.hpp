#ifndef GLUCOSE_ANALYTICS_HPP
#define GLUCOSE_ANALYTICS_HPP

#include <string>
#include <vector>

// Análise estatística e clínica dos registros de glicose de um paciente.
//
// As métricas seguem referências reconhecidas no manejo do diabetes:
//   - Time in Range (TIR): % de leituras na faixa 70-180 mg/dL (consenso ADA).
//   - GMI (Glucose Management Indicator): 3.31 + 0.02392 * média (Bergenstal, 2018).
//   - A1c estimada (eA1c): (média + 46.7) / 28.7 (estudo ADAG, Nathan, 2008).
//   - CV (coeficiente de variação): variabilidade glicêmica; alvo <= 36%.
//
// Aviso: estes valores são informativos e não substituem avaliação médica.

struct GlucoseReading {
    std::string date;   // DD/MM/AAAA
    std::string time;   // HH:MM:SS
    double level;       // mg/dL
    bool fasting;
    long long sortKey;  // AAAAMMDDHHMMSS para ordenação cronológica
};

struct GlucoseStats {
    int count = 0;
    double mean = 0.0;
    double median = 0.0;
    double stddev = 0.0;       // desvio padrão amostral
    double cv = 0.0;           // coeficiente de variação (%)
    double minValue = 0.0;
    double maxValue = 0.0;
    double gmi = 0.0;          // Glucose Management Indicator (%)
    double ea1c = 0.0;         // A1c estimada (%)
    double tirInRange = 0.0;   // % na faixa 70-180
    double tirBelow = 0.0;     // % < 70 (hipoglicemia)
    double tirAbove = 0.0;     // % > 180 (hiperglicemia)
    double trendSlope = 0.0;   // mg/dL por leitura (regressão linear)
};

class GlucoseAnalytics {
public:
    // Carrega as leituras do paciente já ordenadas cronologicamente (ascendente).
    static std::vector<GlucoseReading> loadReadings(int patientId);

    // Calcula todas as métricas. Para vetor vazio, devolve stats zerado.
    static GlucoseStats compute(const std::vector<GlucoseReading>& readings);

    // Imprime um painel completo (estatísticas + TIR + tendência + sparkline).
    static void printReport(int patientId);

    // Exporta as leituras para um arquivo CSV. Retorna true em caso de sucesso.
    static bool exportCsv(int patientId, const std::string& path);

    // Gera um sparkline (mini-gráfico) a partir de uma série de valores.
    static std::string sparkline(const std::vector<double>& values);

    // Classifica uma leitura individual (texto legível).
    static std::string classify(double level, bool fasting);
};

#endif // GLUCOSE_ANALYTICS_HPP
