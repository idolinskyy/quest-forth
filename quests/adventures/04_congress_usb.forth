\ ============================================================================
\  ДОВГИЙ КВЕСТ: крадіжка флешки з даними на науковому конгресі.
\  Жанр: трилер / розслідування. Сучасність, без фантастичних гаджетів.
\

"Конгрес у «Глобусі»" TITLE:
"Ігрові квести" AUTHOR:
"1.0" VERSION:

0 "badge_ok" !
0 "saw_cctv" !
0 "has_usb" !
0 "talked_speaker" !
0 "talked_tech" !
0 "code_known" !
0 "usb_restored" !

SCENE: lobby
  CLS
  "Готель «Глобус», хол" LOCATION:
  "Ти — помічник оргкомітету. У доповідача зникла флешка з ембарго-даними клінічного дослідження." .
  "Прес вже нишпорить. Потрібно тихо повернути носій до вечірньої сесії." .
  "Реєстраційна стійка" CHOICE
  "Охорона / CCTV" CHOICE
  "Зал доповідей" CHOICE
  "Технічна кімната" CHOICE
  WAIT_CHOICE
  dup 1 = IF drop GOTO: reg THEN
  dup 2 = IF drop GOTO: cctv THEN
  dup 3 = IF drop GOTO: hall THEN
  drop GOTO: tech
;SCENE

SCENE: hub
  CLS
  "Фойє конгресу" LOCATION:
  "Реєстрація" CHOICE
  "CCTV" CHOICE
  "Зал A" CHOICE
  "Техкімната" CHOICE
  "Кав'ярня (кулуари)" CHOICE
  "Здати результат координатору" CHOICE
  WAIT_CHOICE
  dup 1 = IF drop GOTO: reg THEN
  dup 2 = IF drop GOTO: cctv THEN
  dup 3 = IF drop GOTO: hall THEN
  dup 4 = IF drop GOTO: tech THEN
  dup 5 = IF drop GOTO: cafe THEN
  drop GOTO: report
;SCENE

SCENE: reg
  CLS
  "Реєстрація" LOCATION:
  "Список бейджів: хтось клонував перепустку «PRESS-17» о 14:10." .
  1 "badge_ok" !
  1 "копія журналу бейджів" ITEM+
  "Назад" CHOICE
  WAIT_CHOICE
  drop GOTO: hub
;SCENE

SCENE: cctv
  CLS
  "Кімната охорони" LOCATION:
  "На записі: людина в синьому худі забирає флешку зі столу президії о 14:12." .
  "Обличчя в тіні, але на зап'ясті — червоний браслет учасника воркшопу." .
  1 "saw_cctv" !
  1 "флеш-кадр з CCTV" ITEM+
  "Назад" CHOICE
  WAIT_CHOICE
  drop GOTO: hub
;SCENE

SCENE: hall
  CLS
  "Зал A" LOCATION:
  1 "talked_speaker" !
  "Доповідачка Ірина: «Після обіду я лишила флешку в ноутбуці на хвилину»." .
  "Пароль архіву на флешці — дата старту дослідження, але навпаки: 9002-04-01." .
  1 "code_known" !
  "Назад" CHOICE
  WAIT_CHOICE
  drop GOTO: hub
;SCENE

SCENE: tech
  CLS
  "Технічна" LOCATION:
  1 "talked_tech" !
  "Звукорежисер: «Синій худі бачив біля кав'ярні. Там тимчасовий прес-бокс»." .
  "На полиці — забутий ноутбук преси з відкритим файловим менеджером." .
  "has_usb" @ IF
    "code_known" @ IF
      "Ти вводиш пароль 9002-04-01. Архів відкривається. Копію відновлено." .
      1 "usb_restored" !
      1 "робоча копія даних" ITEM+
    ELSE
      "Без пароля архів зашифрований. Потрібна підказка від доповідачки." .
    THEN
  ELSE
    "Флешки тут немає — лише кабелі." .
  THEN
  "Назад" CHOICE
  WAIT_CHOICE
  drop GOTO: hub
;SCENE

SCENE: cafe
  CLS
  "Кав'ярня" LOCATION:
  "Прес-бокс напіввідчинений. Усередині — синій худі й чорна флешка з наклейкою лаби." .
  "badge_ok" @ "saw_cctv" @ AND IF
    "Ти фіксуєш ланцюжок: клон бейджа → крадіжка → худі. Викликаєш охорону готелю." .
    1 "has_usb" !
    1 "флешка з даними" ITEM+
    "Носій у тебе. Дані ще треба відкрити." .
  ELSE
    "Без доказів охорона не дає відкрити бокс «чужого» преса." .
    "Залишається збирати свідчення." .
  THEN
  "Назад" CHOICE
  WAIT_CHOICE
  drop GOTO: hub
;SCENE

SCENE: report
  CLS
  "Офіс координатора" LOCATION:
  "usb_restored" @ "has_usb" @ AND IF
    "Координатор киває: сесію врятовано, інцидент задокументовано без витоку в мережу." .
    "Конгрес триває за розкладом." VICTORY
  ELSE
    "has_usb" @ IF
      "Флешка є, але архів не відкрито вчасно. Доповідачку знімають із програми." .
      "Частковий провал." DEFEAT
    ELSE
      "Носій не знайдено. Через годину анонімний телеграм-канал починає злив." .
      "Репутаційний удар." DEFEAT
    THEN
  THEN
;SCENE
