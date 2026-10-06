\ ============================================================================
\  ДОВГИЙ КВЕСТ: інцидент у біотех-лабораторії (саботаж холодильника зразків).
\  Жанр: science procedural. Науковий антураж, етика, логіка доступу.
\

"Холодний контур" TITLE:
"QuestForth" AUTHOR:
"1.0" VERSION:

0 "log_access" !
0 "temp_graph" !
0 "badge_anna" !
0 "badge_oleg" !
0 "sample_moved" !
0 "motive_debt" !
0 "case_ready" !

: check_ready
  \ Службове слово: виставляє case_ready, якщо зібрано ключові факти
  "log_access" @ "temp_graph" @ AND "motive_debt" @ AND "sample_moved" @ AND IF
    1 "case_ready" !
  THEN
;

SCENE: lobby
  CLS
  "КПП біотех-парку" LOCATION:
  "Уночі впав холодильник −80°C. Партія зразків клінічного етапу зіпсована." .
  "Керівництво підозрює саботаж: конкуренція за грант." .
  "Журнал доступів" CHOICE
  "Кімната холоду" CHOICE
  "Кабінет бухгалтерії" CHOICE
  "Відкритий lab-space" CHOICE
  WAIT_CHOICE
  dup 1 = IF drop GOTO: access THEN
  dup 2 = IF drop GOTO: cold THEN
  dup 3 = IF drop GOTO: finance THEN
  drop GOTO: openlab
;SCENE

SCENE: hub
  CLS
  "Коридор корпусу B" LOCATION:
  "Журнал доступів" CHOICE
  "Кімната холоду" CHOICE
  "Бухгалтерія" CHOICE
  "Open lab" CHOICE
  "Серверна телеметрії" CHOICE
  "Доповідь комісії" CHOICE
  WAIT_CHOICE
  dup 1 = IF drop GOTO: access THEN
  dup 2 = IF drop GOTO: cold THEN
  dup 3 = IF drop GOTO: finance THEN
  dup 4 = IF drop GOTO: openlab THEN
  dup 5 = IF drop GOTO: telemetry THEN
  drop GOTO: commission
;SCENE

SCENE: access
  CLS
  "Стійка СКУД" LOCATION:
  "О 03:11 картка «О. Бондар» відкрила холодну зону. О 03:18 — картка «А. Сергієнко»." .
  1 "log_access" !
  1 "badge_oleg" !
  1 "badge_anna" !
  1 "роздрук СКУД" ITEM+
  check_ready
  "Назад" CHOICE
  WAIT_CHOICE
  drop GOTO: hub
;SCENE

SCENE: cold
  CLS
  "Кімната −80" LOCATION:
  "Двері не зламані. На підлозі — іній і слід транспортного контейнера." .
  "Зразки перекладали: частина етикеток у смітнику з іншою датою." .
  1 "sample_moved" !
  1 "фото етикеток" ITEM+
  check_ready
  "Назад" CHOICE
  WAIT_CHOICE
  drop GOTO: hub
;SCENE

SCENE: finance
  CLS
  "Бухгалтерія" LOCATION:
  "У теці Бондара — лист про прострочену позику й чернетка резюме в компанію-конкурента." .
  1 "motive_debt" !
  1 "копія листування" ITEM+
  check_ready
  "Назад" CHOICE
  WAIT_CHOICE
  drop GOTO: hub
;SCENE

SCENE: openlab
  CLS
  "Open lab" LOCATION:
  "Анна Сергієнко: «Я прийшла о 03:18, бо датчик надіслав SMS про температуру»." .
  "Показує телефон: сповіщення справжнє. Її прихід — реакція, не старт інциденту." .
  "Це послаблює підозру проти неї, якщо логи збігаються." .
  "Назад" CHOICE
  WAIT_CHOICE
  drop GOTO: hub
;SCENE

SCENE: telemetry
  CLS
  "Серверна" LOCATION:
  "Графік температури: різкий стрибок після відкриття дверей о 03:11, не через збій компресора." .
  1 "temp_graph" !
  1 "графік телеметрії" ITEM+
  check_ready
  "Назад" CHOICE
  WAIT_CHOICE
  drop GOTO: hub
;SCENE

SCENE: commission
  CLS
  "Зала засідань" LOCATION:
  "Комісія чекає прізвища." .
  "Олег Бондар" CHOICE
  "Анна Сергієнко" CHOICE
  "Випадковий технічний збій" CHOICE
  "Ще зібрати факти" CHOICE
  WAIT_CHOICE
  dup 4 = IF drop GOTO: hub THEN
  dup 2 = IF drop GOTO: wrong_anna THEN
  dup 3 = IF drop GOTO: wrong_tech THEN
  drop GOTO: accuse_oleg
;SCENE

SCENE: accuse_oleg
  CLS
  "Зала засідань" LOCATION:
  "case_ready" @ IF
    "Ти зшиваєш: перший доступ 03:11, графік не аварійний, переміщення етикеток, мотив боргу." .
    "Бондар зізнається в спробі зірвати етап конкурентам. Зразки-двійники знаходять у його авто." .
    "Розслідування закрито грамотно." VICTORY
  ELSE
    "Фактів бракує. Юристи Бондара розносять вашу версію." .
    "Компроміс із конкурентом уже підписано." DEFEAT
  THEN
;SCENE

SCENE: wrong_anna
  CLS
  "Зала засідань" LOCATION:
  "SMS і другий час доступу підтверджують її слова. Ти звинуватив людину, яка рятувала зразки." .
  "Скандал у колективі. Справжній винуватець іде з компанії." DEFEAT
;SCENE

SCENE: wrong_tech
  CLS
  "Зала засідань" LOCATION:
  "Телеметрія противоречить версії «сам зламався». Розслідування дискредитовано." .
  "Провал." DEFEAT
;SCENE
