\ ============================================================================
\  Розвилка на туристичній стежці: погода й підготовка.
\

"Розвилка на перевалі" TITLE:
"QuestForth" AUTHOR:
"1.0" VERSION:

0 "has_map" !
0 "has_water" !

SCENE: trailhead
  CLS
  "Старт маршруту" LOCATION:
  "Перед тобою дві стежки. Небо сіріє. У наплічнику майже порожньо." .
  "Заглянути в інфо-щит" CHOICE
  "Наповнити пляшку в струмку" CHOICE
  "Одразу йти коротшим шляхом" CHOICE
  WAIT_CHOICE
  dup 1 = IF drop GOTO: info THEN
  dup 2 = IF drop GOTO: stream THEN
  drop GOTO: short_path
;SCENE

SCENE: info
  CLS
  "Інфо-щит" LOCATION:
  "Карта радить довший, але захищений лісом маршрут при негоді." .
  1 "has_map" !
  1 "паперова карта" ITEM+
  "Наповнити пляшку" CHOICE
  "Йти довгим маршрутом" CHOICE
  WAIT_CHOICE
  1 = IF GOTO: stream ELSE GOTO: long_path THEN
;SCENE

SCENE: stream
  CLS
  "Струмок" LOCATION:
  "Холодна вода. Пляшка повна." .
  1 "has_water" !
  1 "пляшка води" ITEM+
  "Йти довгим маршрутом" CHOICE
  "Ризикнути коротким" CHOICE
  WAIT_CHOICE
  1 = IF GOTO: long_path ELSE GOTO: short_path THEN
;SCENE

SCENE: long_path
  CLS
  "Лісовий серпантин" LOCATION:
  \ Успіх, якщо є хоча б карта АБО вода
  "has_map" @ "has_water" @ OR IF
    "Дощ застав тебе під кронами. Ти виходиш до притулку мокрий, але цілий." .
    "Перевал пройдено." VICTORY
  ELSE
    "Без орієнтирів і води ти збиваєшся зі стежки." .
    "Довелося викликати рятувальників." DEFEAT
  THEN
;SCENE

SCENE: short_path
  CLS
  "Відкритий хребет" LOCATION:
  "has_water" @ IF
    "Вітер рве куртку, але вода тримає сили. Ти доповзаєш до сідловини." .
    "Ризиковано, але вдалося." VICTORY
  ELSE
    "Сонце й вітер висушують швидко. Сил бракує." .
    "Маршрут зірвано." DEFEAT
  THEN
;SCENE
