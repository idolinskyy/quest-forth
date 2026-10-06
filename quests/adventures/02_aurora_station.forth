\ ============================================================================
\  ДОВГИЙ КВЕСТ: аварія на полярній дослідницькій станції.
\  Жанр: наукова пригода / виживання. Реалістична техніка, без «магії холоду».
\

"Станція «Аврора»" TITLE:
"Ігрові квести" AUTHOR:
"1.0" VERSION:

0 "gen_ok" !
0 "radio_ok" !
0 "med_ok" !
0 "knows_short" !
0 "fuel_moved" !
0 "team_safe" !

SCENE: wake
  CLS
  "Житловий модуль" LOCATION:
  "Сирена. Температура в коридорі падає. На екрані: збій дизель-генератора." .
  "Зв'язок із материком мовчить. У команді п'ятеро, один з підвищеною температурою." .
  "Йти до генераторної" CHOICE
  "Перевірити радіорубку" CHOICE
  "Заглянути в медпункт" CHOICE
  WAIT_CHOICE
  dup 1 = IF drop GOTO: generator THEN
  dup 2 = IF drop GOTO: radio THEN
  drop GOTO: medbay
;SCENE

SCENE: hub
  CLS
  "Центральний коридор" LOCATION:
  "Системи станції:" .
  "Генераторна" CHOICE
  "Радіорубка" CHOICE
  "Медпункт" CHOICE
  "Склад палива" CHOICE
  "Спроба евакуації / фінал" CHOICE
  WAIT_CHOICE
  dup 1 = IF drop GOTO: generator THEN
  dup 2 = IF drop GOTO: radio THEN
  dup 3 = IF drop GOTO: medbay THEN
  dup 4 = IF drop GOTO: fuel THEN
  drop GOTO: finale
;SCENE

SCENE: generator
  CLS
  "Генераторна" LOCATION:
  "Дизель захлинається: у фільтрі — лід і бруд з невдалого доливу." .
  "knows_short" @ IF
    "Ти пам'ятаєш про короткий контур у щиті — спочатку безпека." .
    "Відключити аварійну лінію, потім чистити фільтр" CHOICE
    "Ігнорувати щит і чистити навмання" CHOICE
    WAIT_CHOICE
    1 = IF
      1 "gen_ok" !
      "Генератор оживає. Тепло повертається в модулі." .
      GOTO: hub
    ELSE
      "Іскра. Автоматика глушить усе. Стає ще холодніше." .
      "Каскадна відмова систем." DEFEAT
    THEN
  ELSE
    "Потрібна підказка з радіожурналів про відомий дефект щита." .
    "Назад у коридор" CHOICE
    WAIT_CHOICE
    drop GOTO: hub
  THEN
;SCENE

SCENE: radio
  CLS
  "Радіорубка" LOCATION:
  "Журнал змін: «Не вмикати лінію B при вологому фільтрі — ризик КЗ»." .
  1 "knows_short" !
  1 "виписка з журналу" ITEM+
  "Антена обмерзла. Можна спробувати прогрів від акумуляторів." .
  "gen_ok" @ IF
    "З живленням прогрів миттєвий. Ефір чистий." .
    1 "radio_ok" !
    1 "підтвердження зв'язку" ITEM+
  ELSE
    "Без генератора акумулятори сідають за хвилини. Зв'язок нестабільний." .
  THEN
  "Назад" CHOICE
  WAIT_CHOICE
  drop GOTO: hub
;SCENE

SCENE: medbay
  CLS
  "Медпункт" LOCATION:
  "Інженер Катя — озноб, але стабільна. Потрібен теплий контур і регідратація." .
  "gen_ok" @ IF
    "Обігрівач працює. Ти стабілізуєш стан за протоколом." .
    1 "med_ok" !
    1 "медичний звіт" ITEM+
  ELSE
    "У холоді симптоми наростають. Спочатку тепло." .
  THEN
  "Назад" CHOICE
  WAIT_CHOICE
  drop GOTO: hub
;SCENE

SCENE: fuel
  CLS
  "Склад палива" LOCATION:
  "Бочки стоять біля зовнішніх дверей — небезпечно при перепаді тиску." .
  "fuel_moved" @ IF
    "Ти вже переніс резерв у внутрішній тамбур." .
  ELSE
    "Перенести дві бочки ближче до генераторної?" .
    "Так, зробити це обережно" CHOICE
    "Пізніше" CHOICE
    WAIT_CHOICE
    1 = IF
      1 "fuel_moved" !
      1 "мітка резерву палива" ITEM+
      "Запас у безпечній зоні." .
      GOTO: hub
    ELSE
      GOTO: hub
    THEN
  THEN
  "Назад" CHOICE
  WAIT_CHOICE
  drop GOTO: hub
;SCENE

SCENE: finale
  CLS
  "Шлюз" LOCATION:
  "Погода дає вікно в 40 хвилин. Рішення?" .
  "Чекати рятувальний борт (потрібен зв'язок і стабільні системи)" CHOICE
  "Йти пішки до маяка (14 км)" CHOICE
  "Ще підготуватися" CHOICE
  WAIT_CHOICE
  dup 3 = IF drop GOTO: hub THEN
  dup 2 = IF drop GOTO: hike THEN
  drop GOTO: wait_rescue
;SCENE

SCENE: wait_rescue
  CLS
  "Злітна бочка / площадка" LOCATION:
  \ Потрібні: генератор, радіо, медстабілізація, паливо в безпеці
  "gen_ok" @ "radio_ok" @ AND "med_ok" @ AND "fuel_moved" @ AND IF
    1 "team_safe" !
    "Борт сідає. Усі п'ятеро на борту. Дані експедиції врятовано." .
    "Евакуація успішна." VICTORY
  ELSE
    "Диспетчер не підтверджує посадку: умови на станції нестабільні." .
    "Вікно закривається. Доводиться імпровізувати — і це погано кінчається." DEFEAT
  THEN
;SCENE

SCENE: hike
  CLS
  "Крижане плато" LOCATION:
  "med_ok" @ "gen_ok" @ AND IF
    "Команда в термокомбінезонах доходить до маяка. Важко, але всі живі." .
    "Піший відхід вдався." VICTORY
  ELSE
    "Без тепла й стабільного стану пораненої шлях стає пасткою." .
    "Рятувальники знаходять вас надто пізно." DEFEAT
  THEN
;SCENE
