\ ============================================================================
\  ДОВГИЙ КВЕСТ: спелеологічна експедиція. Геологія, логіка маршруту, безпека.
\  Без «духів печери» — лише фізика, вода, газ, карта.
\

"Нитка Аріадни" TITLE:
"Ігрові квести" AUTHOR:
"1.0" VERSION:

0 "has_map" !
0 "has_rope" !
0 "has_meter" !
0 "gas_checked" !
0 "marked_path" !
0 "partner_ok" !

SCENE: camp
  CLS
  "Базовий табір" LOCATION:
  "Вхід у систему ходів позначено червоним пікетом. Напарник уже в першому залу." .
  "Рація шипить: «Тяга повітря дивна. Перевір газоаналізатор»." .
  "Оглянути спорядження" CHOICE
  "Увійти до входу" CHOICE
  WAIT_CHOICE
  1 = IF GOTO: gear ELSE GOTO: entrance THEN
;SCENE

SCENE: hub
  CLS
  "Зал «Катов»" LOCATION:
  "Вологі стіни, три розвилки." .
  "Спорядження / табір" CHOICE
  "Північний хід (вузький)" CHOICE
  "Східний хід (шум води)" CHOICE
  "Західний хід (тепліше)" CHOICE
  "Спроба вивести напарника" CHOICE
  WAIT_CHOICE
  dup 1 = IF drop GOTO: gear THEN
  dup 2 = IF drop GOTO: north THEN
  dup 3 = IF drop GOTO: east THEN
  dup 4 = IF drop GOTO: west THEN
  drop GOTO: extract
;SCENE

SCENE: gear
  CLS
  "Табір біля входу" LOCATION:
  "На столі: карта минулої групи, динамічна мотузка, газоаналізатор." .
  "Взяти карту" CHOICE
  "Взяти мотузку" CHOICE
  "Взяти газоаналізатор" CHOICE
  "Повернутися в зал" CHOICE
  WAIT_CHOICE
  dup 4 = IF drop GOTO: hub THEN
  dup 1 = IF
    drop
    1 "has_map" !
    1 "карта ходів" ITEM+
    GOTO: gear
  THEN
  dup 2 = IF
    drop
    1 "has_rope" !
    1 "динамічна мотузка" ITEM+
    GOTO: gear
  THEN
  drop
  1 "has_meter" !
  1 "газоаналізатор" ITEM+
  GOTO: gear
;SCENE

SCENE: entrance
  CLS
  "Вхідний грот" LOCATION:
  "Повітря важке. Без виміру краще не йти глибоко." .
  "Йти в головний зал" CHOICE
  "Спочатку до спорядження" CHOICE
  WAIT_CHOICE
  1 = IF GOTO: hub ELSE GOTO: gear THEN
;SCENE

SCENE: north
  CLS
  "Вузький хід" LOCATION:
  "has_rope" @ IF
    "Мотузка тримає на гладкому уступі. Ти ставиш проміжну точку." .
    1 "marked_path" !
  ELSE
    "Нога ковзає. Без страховки далі небезпечно — ти повертаєшся." .
    "Назад" CHOICE
    WAIT_CHOICE
    drop GOTO: hub
    HALT
  THEN
  "has_map" @ IF
    "Карта підтверджує: цей хід веде до старої закладки." .
  THEN
  "Назад до залу" CHOICE
  WAIT_CHOICE
  drop GOTO: hub
;SCENE

SCENE: east
  CLS
  "Галерея з водою" LOCATION:
  "Шум потоку. Рівень піднявся після дощу на поверхні." .
  "has_meter" @ IF
    "Газоаналізатор: O2 в нормі, CO2 підвищений біля стелі — не затримуватись." .
    1 "gas_checked" !
  ELSE
    "Голова важка. Без приладу ти не розумієш чому — повертаєшся." .
    "Назад" CHOICE
    WAIT_CHOICE
    drop GOTO: hub
    HALT
  THEN
  "Тимчасовий брід ще прохідний." .
  "Назад" CHOICE
  WAIT_CHOICE
  drop GOTO: hub
;SCENE

SCENE: west
  CLS
  "Теплий хід" LOCATION:
  "Тяга з глибини. Тут ти знаходиш напарника: підвернув гомілку, сидить на виступі." .
  1 "partner_ok" !
  "Він шепоче: «Не ходи далі без мітки шляху — лабіринт»." .
  "Назад разом до залу" CHOICE
  WAIT_CHOICE
  drop GOTO: hub
;SCENE

SCENE: extract
  CLS
  "План підйому" LOCATION:
  "Щоб вивести напарника безпечно, потрібні мітка шляху, перевірка газу й карта." .
  "marked_path" @ "gas_checked" @ AND "has_map" @ AND "partner_ok" @ AND IF
    "Ви йдете розміченим маршрутом. На поверхні — світло й чай у термосі." .
    "Експедицію завершено без втрат." VICTORY
  ELSE
    "Ти форсуєш вихід. У тісняві губите орієнтир." .
    "Рятувальний загін витягує вас через добу — з переохолодженням." DEFEAT
  THEN
;SCENE
