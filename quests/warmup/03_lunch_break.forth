\ ============================================================================
\  Легка сцена в кафе: бюджет і вибір страви.
\  Показує змінні та порівняння.
\

"Обідна перерва" TITLE:
"Ігрові квести" AUTHOR:
"1.0" VERSION:

120 "money" !            \ гривень у гаманці

SCENE: menu
  CLS
  "Кафе «Південь»" LOCATION:
  "У гаманці 120 грн. Суп — 60, сендвіч — 90, десерт — 50." .
  "Взяти суп" CHOICE
  "Взяти сендвіч" CHOICE
  "Взяти лише десерт" CHOICE
  WAIT_CHOICE
  dup 1 = IF drop 60 "cost" ! GOTO: pay THEN
  dup 2 = IF drop 90 "cost" ! GOTO: pay THEN
  drop 50 "cost" ! GOTO: pay
;SCENE

SCENE: pay
  CLS
  "Каса" LOCATION:
  \ Перевірка: чи вистачає грошей (money >= cost)
  "money" @ "cost" @ >= IF
    "money" @ "cost" @ - "money" !
    "Ти поїв і ще встиг на зустріч. У гаманці лишилось: " .
    "money" @ .
    " грн." .
    "Ситий і вчасно." VICTORY
  ELSE
    "Касир качає головою: не вистачає." .
    "Довелося йти голодним." DEFEAT
  THEN
;SCENE
