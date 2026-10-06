\ ============================================================================
\  Коротка історія на вокзалі: обрати правильну колію.
\

"Остання електричка" TITLE:
"Ігрові квести" AUTHOR:
"1.0" VERSION:

SCENE: hall
  CLS
  "Вокзал" LOCATION:
  "Табло мерехтить. Твій поїзд — «В» о 21:40, колія 3. Або все-таки 5?" .
  "Дивитися табло уважніше" CHOICE
  "Спитати провідника" CHOICE
  "Бігти навмання на колію 5" CHOICE
  WAIT_CHOICE
  dup 1 = IF drop GOTO: board THEN
  dup 2 = IF drop GOTO: conductor THEN
  drop GOTO: track5
;SCENE

SCENE: board
  CLS
  "Табло" LOCATION:
  "Дрібний рядок: «Колія змінена → 3». Ти майже це пропустив." .
  1 "нотатка про колію" ITEM+
  "Йти на колію 3" CHOICE
  "Все одно перевірити колію 5" CHOICE
  WAIT_CHOICE
  1 = IF GOTO: track3 ELSE GOTO: track5 THEN
;SCENE

SCENE: conductor
  CLS
  "Платформа" LOCATION:
  "Провідник: «На п'яту вже подали інший склад. Вам на третю»." .
  "Подякувати й піти на 3" CHOICE
  WAIT_CHOICE
  drop GOTO: track3
;SCENE

SCENE: track3
  CLS
  "Колія 3" LOCATION:
  "Двері все ще відчинені. Ти стрибаєш у вагон за секунду до свистка." .
  "Доїхав додому без пригод." VICTORY
;SCENE

SCENE: track5
  CLS
  "Колія 5" LOCATION:
  "Склад іде в протилежний бік області. Контролер знизує плечима." .
  "Довелося ночувати на вокзалі." DEFEAT
;SCENE
