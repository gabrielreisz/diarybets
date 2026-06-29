# Changelog

Todas as mudanças relevantes deste projeto são documentadas aqui.
O formato segue [Keep a Changelog](https://keepachangelog.com/pt-BR/1.1.0/)
e o projeto adota [Versionamento Semântico](https://semver.org/lang/pt-BR/).

## [Unreleased]

### Added
- Módulo de **análise de glicose** (`GlucoseAnalytics`): média, mediana,
  desvio padrão, coeficiente de variação, mínimo/máximo, Tempo no Alvo (TIR),
  Indicador de Gestão de Glicose (GMI) e A1c estimada, além de detecção de
  tendência por regressão linear e visualização *sparkline* no terminal.
- Exportação dos registros de glicose para **CSV** para análise externa.
- Módulo de **segurança** (`Security`): hashing de senhas com PBKDF2-HMAC-SHA256
  e *salt* aleatório por usuário, com comparação em tempo constante.
- Utilitário de **console** (`Console`): cores ANSI, banners e cabeçalhos para
  uma interface de terminal mais legível.
- `.gitignore`, `LICENSE` (MIT), `CONTRIBUTING.md` e este `CHANGELOG.md`.

### Changed
- Senhas passam a ser armazenadas como *hash* com *salt*; contas legadas em
  texto puro são migradas de forma transparente no primeiro login bem-sucedido.
- README reescrito com padrão profissional.

### Fixed
- `fall-through` nos cases de **registro de glicose** e **plano alimentar** no
  menu principal (faltava `break`).
- *Null dereference* quando o login carregava um paciente inexistente.

### Security
- Eliminado o armazenamento de senhas em texto puro.
