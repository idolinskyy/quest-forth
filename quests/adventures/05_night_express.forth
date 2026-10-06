\ ============================================================================
\  ДОВГИЙ КВЕСТ: нічний потяг, зниклі документи інженера.
\  Жанр: камерний детектив. Соціальні мотиви, без містики.
\

"Нічний експрес «Карпати»" TITLE:
"Ігрові квести" AUTHOR:
"1.0" VERSION:

0 "talked_conductor" !
0 "talked_neighbor" !
0 "talked_student" !
0 "saw_dining" !
0 "has_ticket_stub" !
0 "has_blueprint" !
0 "knows_switch" !

SCENE: coupe
  CLS
  "Купе №7" LOCATION:
  "Ти прокидаєшся від стуку. У сусіда по купе — розрив сумки. Зникли креслення мосту." .
  "Він інженер підряду. Каже: «Це не крадіжка грошей — це зрив експертизи»." .
  "Провідник" CHOICE
  "Сусіднє купе" CHOICE
  "Вагон-ресторан" CHOICE
  "Хвіст складу" CHOICE
  WAIT_CHOICE
  dup 1 = IF drop GOTO: conductor THEN
  dup 2 = IF drop GOTO: neighbor THEN
  dup 3 = IF drop GOTO: dining THEN
  drop GOTO: tail
;SCENE

SCENE: hub
  CLS
  "Коридор вагона" LOCATION:
  "Купе №7" CHOICE
  "Провідник" CHOICE
  "Сусіднє купе" CHOICE
  "Ресторан" CHOICE
  "Хвіст складу" CHOICE
  "Пред'явити здогадку" CHOICE
  WAIT_CHOICE
  dup 1 = IF drop GOTO: coupe_return THEN
  dup 2 = IF drop GOTO: conductor THEN
  dup 3 = IF drop GOTO: neighbor THEN
  dup 4 = IF drop GOTO: dining THEN
  dup 5 = IF drop GOTO: tail THEN
  drop GOTO: confront
;SCENE

SCENE: coupe_return
  CLS
  "Купе №7" LOCATION:
  "Інженер чекає. «Ну що?»" .
  "Продовжити пошук" CHOICE
  WAIT_CHOICE
  drop GOTO: hub
;SCENE

SCENE: conductor
  CLS
  "Службове купе" LOCATION:
  1 "talked_conductor" !
  "Провідник: між станціями X і Y хтось смикав стоп-кран — короткий зупинка о 01:12." .
  "У журналі — прізвище пасажира з купе №12, але підпис інший." .
  1 "knows_switch" !
  1 "копія журналу зупинки" ITEM+
  "Назад" CHOICE
  WAIT_CHOICE
  drop GOTO: hub
;SCENE

SCENE: neighbor
  CLS
  "Купе №8" LOCATION:
  1 "talked_neighbor" !
  "Жінка з ноутбуком: «Бачила молодого з рюкзаком біля тамбура після зупинки»." .
  "Він підняв щось із підніжки й пішов у хвіст." .
  "Назад" CHOICE
  WAIT_CHOICE
  drop GOTO: hub
;SCENE

SCENE: dining
  CLS
  "Вагон-ресторан" LOCATION:
  1 "saw_dining" !
  "Офіціант віддає забутий корінець квитка: купе №12, пересадка на ранковий автобус." .
  1 "has_ticket_stub" !
  1 "корінець квитка №12" ITEM+
  "Дивно: власник купе №12 мав їхати до кінцевої." .
  "Назад" CHOICE
  WAIT_CHOICE
  drop GOTO: hub
;SCENE

SCENE: tail
  CLS
  "Останній тамбур" LOCATION:
  "Під сидінням — тубус із кресленнями. Поруч — студентський квиток на ім'я Д. Мельник." .
  1 "has_blueprint" !
  1 "тубус із кресленнями" ITEM+
  1 "talked_student" !
  "knows_switch" @ IF
    "Стоп-кран використав як відволікання. Класика." .
  THEN
  "Покликати провідника й повернути тубус" CHOICE
  WAIT_CHOICE
  drop GOTO: confront
;SCENE

SCENE: confront
  CLS
  "Купе №12" LOCATION:
  \ Для перемоги потрібні креслення + корінець + факт стоп-крана
  "has_blueprint" @ "has_ticket_stub" @ AND "knows_switch" @ AND IF
    "Мельник зізнається: фірма-конкурент платила за затримку експертизи." .
    "Креслення цілі. Поліція чекає на наступній станції." .
    "Справедливість і графік мосту врятовано." VICTORY
  ELSE
    "has_blueprint" @ IF
      "Тубус є, але без зв'язки мотивів хлопець викручується «знайшов»." .
      "Справа розмивається." DEFEAT
    ELSE
      "Без креслень лишаються лише підозри. На кінцевій інженер отримує звільнення." .
      "Провал." DEFEAT
    THEN
  THEN
;SCENE
