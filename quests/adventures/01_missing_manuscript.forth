\ ============================================================================
\  ДОВГИЙ КВЕСТ: розслідування крадіжки рукопису в міській бібліотеці.
\  Жанр: детектив / логіка. Без надприродного.
\  Механіки: локації, інвентар-докази, прапорці допитів, фінальний висновок.
\

"Справа про зниклий рукопис" TITLE:
"QuestForth" AUTHOR:
"1.0" VERSION:

0 "talked_archivist" !
0 "talked_student" !
0 "talked_guard" !
0 "saw_log" !
0 "has_glove" !
0 "has_keycard" !
0 "knows_window" !
0 "accusation_ready" !

SCENE: start
  CLS
  "Вестибюль бібліотеки" LOCATION:
  "Директор шепоче: зі спецсховища зникла коробка з рукописом XIX ст." .
  "Камери в підвалі не пишуть — «на обслуговуванні». Підозрюваних троє." .
  "Йти до архівіста" CHOICE
  "Шукати студентку-практикантку" CHOICE
  "Перевірити пост охорони" CHOICE
  "Оглянути спецсховище" CHOICE
  WAIT_CHOICE
  dup 1 = IF drop GOTO: archivist THEN
  dup 2 = IF drop GOTO: student THEN
  dup 3 = IF drop GOTO: guard THEN
  drop GOTO: vault
;SCENE

SCENE: hub
  CLS
  "Коридор бібліотеки" LOCATION:
  "Куди далі?" .
  "Архівіст" CHOICE
  "Практикантка" CHOICE
  "Охорона" CHOICE
  "Спецсховище" CHOICE
  "Читальна зала (логи)" CHOICE
  "Зробити висновок" CHOICE
  WAIT_CHOICE
  dup 1 = IF drop GOTO: archivist THEN
  dup 2 = IF drop GOTO: student THEN
  dup 3 = IF drop GOTO: guard THEN
  dup 4 = IF drop GOTO: vault THEN
  dup 5 = IF drop GOTO: reading_room THEN
  drop GOTO: accuse
;SCENE

SCENE: archivist
  CLS
  "Кабінет архівіста" LOCATION:
  1 "talked_archivist" !
  "Мирон Коваль нервує: «Я був у читальній залі з 18:00. Ключ від сховища лише в мене й у директора»." .
  "На столі — свіжий поріз від паперу й порожнє місце під рукавичкою." .
  "has_glove" @ IF
    "Ти кладеш знайдену рукавичку. Мирон блідне: «Це… з комплекту реставрації»." .
  ELSE
    "Варто пізніше зіставити з речовими доказами." .
  THEN
  "Повернутися в коридор" CHOICE
  WAIT_CHOICE
  drop GOTO: hub
;SCENE

SCENE: student
  CLS
  "Двір бібліотеки" LOCATION:
  1 "talked_student" !
  "Практикантка Лія: «Я виходила курити біля службового вікна підвалу близько 18:20»." .
  "Бачила силует із чорною сумкою. Обличчя не розгледіла — світло погасло." .
  1 "knows_window" !
  "Повернутися" CHOICE
  WAIT_CHOICE
  drop GOTO: hub
;SCENE

SCENE: guard
  CLS
  "Пост охорони" LOCATION:
  1 "talked_guard" !
  "Охоронець: «Журнал відвідувачів вела змінниця. Я обходив двір»." .
  "Відчиняє шухляду: зайва ключ-карта з міткою «РЕСТАВРАЦІЯ»." .
  "has_keycard" @ 0 = IF
    "Взяти ключ-карту як речовий доказ" CHOICE
    "Залишити" CHOICE
    WAIT_CHOICE
    1 = IF
      1 "has_keycard" !
      1 "ключ-карта реставрації" ITEM+
      GOTO: hub
    ELSE
      GOTO: hub
    THEN
  ELSE
    "Карта вже у тебе." .
    "Далі" CHOICE
    WAIT_CHOICE
    drop GOTO: hub
  THEN
;SCENE

SCENE: vault
  CLS
  "Спецсховище" LOCATION:
  "Металеві стелажі. Коробка №17 порожня. На підлозі — біла бавовняна рукавичка." .
  "has_glove" @ 0 = IF
    1 "has_glove" !
    1 "бавовняна рукавичка" ITEM+
    "Підібрано." .
  ELSE
    "Ти вже забрав рукавичку." .
  THEN
  "knows_window" @ IF
    "Біля вентиляційної решітки — свіжі подряпини: хтось відкривав її зсередини." .
  ELSE
    "Варто дізнатися, чи хтось бачив підвальне вікно ввечері." .
  THEN
  "has_keycard" @ IF
    "Замок картридера блимає: ця карта сюди підходить." .
  THEN
  "Назад" CHOICE
  WAIT_CHOICE
  drop GOTO: hub
;SCENE

SCENE: reading_room
  CLS
  "Читальна зала" LOCATION:
  "Комп'ютер чергового ще теплий. У логах — вхід карткою «РЕСТАВРАЦІЯ» о 18:17." .
  "Ім'я власника в системі: М. Коваль. Але директор казав, що Мирон був тут…" .
  1 "saw_log" !
  1 "роздрук логів" ITEM+
  \ Прапорець «готовий до звинувачення»: потрібні логи + рукавичка + карта
  "saw_log" @ "has_glove" @ AND "has_keycard" @ AND IF
    1 "accusation_ready" !
    "Картина складається." .
  THEN
  "Назад" CHOICE
  WAIT_CHOICE
  drop GOTO: hub
;SCENE

SCENE: accuse
  CLS
  "Кабінет директора" LOCATION:
  "Директор чекає. Кого звинувачуєш?" .
  "Архівіста Мирона" CHOICE
  "Практикантку Лію" CHOICE
  "Охоронця" CHOICE
  "Ще рано — повернутися" CHOICE
  WAIT_CHOICE
  dup 4 = IF drop GOTO: hub THEN
  dup 2 = IF drop GOTO: wrong_student THEN
  dup 3 = IF drop GOTO: wrong_guard THEN
  drop GOTO: accuse_archivist
;SCENE

SCENE: accuse_archivist
  CLS
  "Кабінет директора" LOCATION:
  \ Перемога лише з повним ланцюжком доказів
  "accusation_ready" @ IF
    "Ти викладаєш: карта реставрації, лог 18:17, рукавичка, вікно як шлях виносу." .
    "Мирон зізнається: борги й обіцянка «тихих» покупців рукопису." .
    "Рукопис знаходять у його шафці. Справу закрито." VICTORY
  ELSE
    "Директор хитає головою: «Замало доказів. Не можу відсторонити людину»." .
    "Розслідування буксує, рукопис іде в тінь." DEFEAT
  THEN
;SCENE

SCENE: wrong_student
  CLS
  "Кабінет директора" LOCATION:
  "Лія має алібі з камерами двору. Ти звинуватив невинну — довіру втрачено." .
  "Справу передають іншому слідчому." DEFEAT
;SCENE

SCENE: wrong_guard
  CLS
  "Кабінет директора" LOCATION:
  "У охоронця стабільне алібі з обходом. Ти лише створив шум." .
  "Справжній винуватець встигає замісти сліди." DEFEAT
;SCENE
