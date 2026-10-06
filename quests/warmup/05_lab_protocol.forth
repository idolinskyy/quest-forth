\ ============================================================================
\  Коротка лабораторна сцена: дотриматись протоколу безпеки.
\  Науковий антураж без фантастики.
\

"Протокол безпеки" TITLE:
"Ігрові квести" AUTHOR:
"1.0" VERSION:

0 "gloves" !
0 "labeled" !

SCENE: lab
  CLS
  "Навчальна лабораторія" LOCATION:
  "На столі — відкритий флакон із реагентом. Зміна майже закінчилась." .
  "Одягнути рукавички" CHOICE
  "Одразу перелити рідину" CHOICE
  "Підписати порожню ємність" CHOICE
  WAIT_CHOICE
  dup 1 = IF drop GOTO: put_gloves THEN
  dup 2 = IF drop GOTO: pour_raw THEN
  drop GOTO: label_first
;SCENE

SCENE: put_gloves
  CLS
  "Стіл" LOCATION:
  "Нітрилові рукавички сідають щільно." .
  1 "gloves" !
  1 "рукавички" ITEM+
  "Підписати ємність" CHOICE
  "Перелити зараз" CHOICE
  WAIT_CHOICE
  1 = IF GOTO: label_first ELSE GOTO: pour_check THEN
;SCENE

SCENE: label_first
  CLS
  "Маркування" LOCATION:
  "Ти пишеш назву, дату й ініціали. Протокол задоволений." .
  1 "labeled" !
  1 "підписана банка" ITEM+
  "Перелити реагент" CHOICE
  WAIT_CHOICE
  drop GOTO: pour_check
;SCENE

SCENE: pour_check
  CLS
  "Переливання" LOCATION:
  \ Потрібні І рукавички, І етикетка
  "gloves" @ "labeled" @ AND IF
    "Крапля не впала повз. Журнал робіт підписано." .
    "Зміну закрито без інцидентів." VICTORY
  ELSE
    "Інструктор зупиняє тебе: протокол порушено." .
    "Допуск на наступне заняття під питанням." DEFEAT
  THEN
;SCENE

SCENE: pour_raw
  CLS
  "Стіл" LOCATION:
  "Крапля потрапляє на зап'ясток. Нічого критичного, але журнал фіксує порушення." .
  "Поспіх коштував оцінки." DEFEAT
;SCENE
