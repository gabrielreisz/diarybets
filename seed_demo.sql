-- seed_demo.sql
-- Popula uma série temporal de glicose para o paciente de Id = 1 (João Silva),
-- útil para demonstrar o painel de análise (opção 12) e a exportação CSV.
--
-- Uso:
--   sqlite3 database.db < seed_demo.sql
--
-- Observação: "Jejum" é gravado como INTEGER (1/0); o leitor de análise e a
-- listagem tratam tanto INTEGER quanto BLOB, mantendo compatibilidade.

BEGIN TRANSACTION;

INSERT INTO RegistroSaude (Paciente, Data, Hora) VALUES (1, '02/06/2026', '07:15:00');
INSERT INTO RegistroGlicose (RegistroSaude, NivelGlicose, Jejum) VALUES (last_insert_rowid(), 142, 1);

INSERT INTO RegistroSaude (Paciente, Data, Hora) VALUES (1, '03/06/2026', '07:20:00');
INSERT INTO RegistroGlicose (RegistroSaude, NivelGlicose, Jejum) VALUES (last_insert_rowid(), 158, 1);

INSERT INTO RegistroSaude (Paciente, Data, Hora) VALUES (1, '04/06/2026', '12:40:00');
INSERT INTO RegistroGlicose (RegistroSaude, NivelGlicose, Jejum) VALUES (last_insert_rowid(), 201, 0);

INSERT INTO RegistroSaude (Paciente, Data, Hora) VALUES (1, '05/06/2026', '07:05:00');
INSERT INTO RegistroGlicose (RegistroSaude, NivelGlicose, Jejum) VALUES (last_insert_rowid(), 119, 1);

INSERT INTO RegistroSaude (Paciente, Data, Hora) VALUES (1, '06/06/2026', '22:30:00');
INSERT INTO RegistroGlicose (RegistroSaude, NivelGlicose, Jejum) VALUES (last_insert_rowid(), 64, 0);

INSERT INTO RegistroSaude (Paciente, Data, Hora) VALUES (1, '07/06/2026', '07:10:00');
INSERT INTO RegistroGlicose (RegistroSaude, NivelGlicose, Jejum) VALUES (last_insert_rowid(), 134, 1);

INSERT INTO RegistroSaude (Paciente, Data, Hora) VALUES (1, '08/06/2026', '13:00:00');
INSERT INTO RegistroGlicose (RegistroSaude, NivelGlicose, Jejum) VALUES (last_insert_rowid(), 176, 0);

INSERT INTO RegistroSaude (Paciente, Data, Hora) VALUES (1, '09/06/2026', '07:25:00');
INSERT INTO RegistroGlicose (RegistroSaude, NivelGlicose, Jejum) VALUES (last_insert_rowid(), 111, 1);

INSERT INTO RegistroSaude (Paciente, Data, Hora) VALUES (1, '10/06/2026', '19:45:00');
INSERT INTO RegistroGlicose (RegistroSaude, NivelGlicose, Jejum) VALUES (last_insert_rowid(), 188, 0);

INSERT INTO RegistroSaude (Paciente, Data, Hora) VALUES (1, '11/06/2026', '07:00:00');
INSERT INTO RegistroGlicose (RegistroSaude, NivelGlicose, Jejum) VALUES (last_insert_rowid(), 108, 1);

INSERT INTO RegistroSaude (Paciente, Data, Hora) VALUES (1, '12/06/2026', '12:15:00');
INSERT INTO RegistroGlicose (RegistroSaude, NivelGlicose, Jejum) VALUES (last_insert_rowid(), 150, 0);

INSERT INTO RegistroSaude (Paciente, Data, Hora) VALUES (1, '13/06/2026', '07:30:00');
INSERT INTO RegistroGlicose (RegistroSaude, NivelGlicose, Jejum) VALUES (last_insert_rowid(), 102, 1);

INSERT INTO RegistroSaude (Paciente, Data, Hora) VALUES (1, '14/06/2026', '21:10:00');
INSERT INTO RegistroGlicose (RegistroSaude, NivelGlicose, Jejum) VALUES (last_insert_rowid(), 128, 0);

INSERT INTO RegistroSaude (Paciente, Data, Hora) VALUES (1, '15/06/2026', '07:05:00');
INSERT INTO RegistroGlicose (RegistroSaude, NivelGlicose, Jejum) VALUES (last_insert_rowid(), 96, 1);

COMMIT;
