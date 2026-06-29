# Guia de Contribuição

Obrigado por contribuir com o **DiaryBetes**! Este documento descreve o fluxo
de trabalho adotado pela equipe.

## Fluxo de Branches (Git Flow simplificado)

- `master` — branch estável. Só recebe código revisado via *merge* (`--no-ff`).
- `feat/<nome>` — novas funcionalidades.
- `fix/<nome>` — correção de bugs.
- `refactor/<nome>` — refatorações sem mudança de comportamento.
- `docs/<nome>` — documentação.
- `chore/<nome>` — tarefas de infraestrutura/build.

Sempre crie a branch a partir de `master` atualizada:

```bash
git checkout master && git pull
git checkout -b feat/minha-feature
```

## Padrão de Commits (Conventional Commits)

```
<tipo>(<escopo opcional>): <descrição no imperativo>

[corpo opcional explicando o "porquê"]
```

Tipos aceitos: `feat`, `fix`, `refactor`, `docs`, `test`, `chore`, `perf`, `style`.

Exemplos:

```
feat(analytics): adiciona índice de tempo no alvo (TIR) para glicose
fix(menu): corrige fall-through nos cases de glicose e plano alimentar
perf(analytics): calcula estatísticas em O(n) com passagem única
```

## Checklist antes do Pull Request

- [ ] `make rebuild` compila sem novos *warnings* (`-Wall -Wextra`).
- [ ] Entradas inválidas foram testadas (datas, números, strings vazias).
- [ ] Recursos do SQLite são liberados em todos os caminhos (inclusive erro).
- [ ] Nenhum dado sensível é exposto em log (senhas, hashes).
- [ ] `CHANGELOG.md` atualizado quando houver mudança visível ao usuário.

## Estilo de Código

- C++17, 4 espaços de indentação.
- Prepared statements **sempre** que houver entrada do usuário em SQL.
- Funções de banco devem ser exception-safe (RAII ou `try/catch` com cleanup).
