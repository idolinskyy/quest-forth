\ ============================================================================
\  Квест для розігріву: знайти перепустку в офісі.
\  Жанр: повсякденна пригода / мікро-детектив.
\

"Загублена перепустка" TITLE:
"QuestForth" AUTHOR:
"1.0" VERSION:

0 "has_badge" !          \ прапорець: чи вже підібрали перепустку

SCENE: lobby
  CLS
  "Ресепшн" LOCATION:
  "Ранок понеділка. Охоронець дивиться на порожній лоток для перепусток." .
  "Ти розумієш: без бейджа на поверх не пустять." .
  "Поговорити з охоронцем" CHOICE
  "Заглянути під стійку" CHOICE
  "Піти до кав'ярні в холі" CHOICE
  WAIT_CHOICE
  dup 1 = IF drop GOTO: guard THEN
  dup 2 = IF drop GOTO: under_desk THEN
  drop GOTO: cafe
;SCENE

SCENE: guard
  CLS
  "Ресепшн" LOCATION:
  "Охоронець знизує плечима: «Може, у кав'ярні забули? Там завжди метушня»." .
  "Повернутися до холу" CHOICE
  WAIT_CHOICE
  drop GOTO: lobby
;SCENE

SCENE: under_desk
  CLS
  "Під стійкою" LOCATION:
  "Порожньо. Лише зім'ятий чек з кав'ярні на ім'я «О. Коваль»." .
  1 "чек з кав'ярні" ITEM+
  "Повернутися" CHOICE
  WAIT_CHOICE
  drop GOTO: lobby
;SCENE

SCENE: cafe
  CLS
  "Кав'ярня" LOCATION:
  "Бариста киває на загублений куточок біля цукорниць." .
  "has_badge" @ IF
    "Ти вже маєш перепустку. Час на ліфт." .
    "Йти до турнікета" CHOICE
    WAIT_CHOICE
    drop GOTO: gate
  ELSE
    "Серед серветок блищить пластиковий бейдж." .
    "Взяти перепустку" CHOICE
    "Повернутися без неї" CHOICE
    WAIT_CHOICE
    1 = IF
      1 "has_badge" !
      1 "перепустка" ITEM+
      "Є! Ім'я на бейджі збігається з чеком." .
      "Йти до турнікета" CHOICE
      WAIT_CHOICE
      drop GOTO: gate
    ELSE
      GOTO: lobby
    THEN
  THEN
;SCENE

SCENE: gate
  CLS
  "Турнікет" LOCATION:
  "has_badge" @ IF
    "Турнікет пискнув зеленим. Робочий день врятовано." .
    "Ти встиг на планерку." VICTORY
  ELSE
    "Без перепустки двері лишаються зачиненими." .
    "Довелося пояснювати HR чому ти спізнився." DEFEAT
  THEN
;SCENE
