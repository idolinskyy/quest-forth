\
"Острів без карти" TITLE:
"QuestForth" AUTHOR:
"1.0" VERSION:

\ ============================================================
\ СЕРІЯ II — ОСТРІВ БЕЗ КАРТИ
\ Пряме продовження "Маяка, якого не було".
\ ============================================================

0 "found_compass" !
0 "found_stone" !
0 "found_diary" !
0 "found_key" !
0 "found_signal" !
0 "met_cartographer" !
0 "met_watchman" !
0 "met_girl" !
0 "entered_station" !
0 "opened_vault" !
0 "knows_north" !
0 "knows_station" !
0 "knows_watchers" !
0 "knows_origin" !
0 "trusted_cartographer" !
0 "trusted_watchman" !
0 "saved" !

: checkpoint
  "island_checkpoint" SAVE
;

: enough_evidence
  "found_compass" @
  "found_stone" @ AND
  "found_diary" @ AND
  "found_signal" @ AND
;

SCENE: boot
  GOTO: arrival
;SCENE

SCENE: arrival
  CLS
  "Море" LOCATION:
  "Три дні після подій біля маяка ти отримуєш координати, які передав S-17." .
  "Вони приводять у місце, де за картою має бути відкрите море." .
  "Але перед тобою виникає острів." .
  "Його немає ні на GPS, ні на морських картах." .
  "На березі стоїть кам'яний стовп із тим самим знаком S-17." .
  "Висадитися" CHOICE
  "Повернутися в море" CHOICE
  WAIT_CHOICE
  1 = IF
    GOTO: beach
  ELSE
    GOTO: retreat
  THEN
;SCENE

SCENE: beach
  CLS
  "Невідомий острів" LOCATION:
  "Пісок чорний, а вода біля берега неприродно спокійна." .
  "На піску видно три різні сліди: важкі чоботи, босі ноги та сліди старого колісного візка." .
  "Попереду — ліс. Ліворуч — скеля з вирізаним знаком. Праворуч — залишки пристані." .
  "Іти в ліс" CHOICE
  "Дослідити скелю" CHOICE
  "Оглянути пристань" CHOICE
  "Дослідити сліди" CHOICE
  WAIT_CHOICE
  dup 1 = IF drop GOTO: forest THEN
  dup 2 = IF drop GOTO: rock THEN
  dup 3 = IF drop GOTO: dock THEN
  drop GOTO: tracks
;SCENE

SCENE: tracks
  CLS
  "Берег" LOCATION:
  "Сліди босих ніг раптово зникають біля води." .
  "Сліди чобіт ведуть у ліс." .
  "Колеса ведуть до старої пристані." .
  "На одному з каменів ти знаходиш латунний уламок із номером 17." .
  1 "found_stone" !
  1 "found_signal" !
  1 "signal_piece" ITEM+
  "Піти за слідами чобіт" CHOICE
  "Піти за слідами коліс" CHOICE
  "Повернутися" CHOICE
  WAIT_CHOICE
  dup 1 = IF drop GOTO: forest THEN
  dup 2 = IF drop GOTO: dock THEN
  drop GOTO: beach
;SCENE

SCENE: rock
  CLS
  "Скеля" LOCATION:
  "На скелі вирізані сім концентричних кіл." .
  "У центрі — знак, схожий на око." .
  "Ти торкаєшся каменю, і десь під землею чути низький гул." .
  "На зворотному боці каменю є напис: «S-17 була сьомою»." .
  1 "knows_origin" !
  "Зламати камінь" CHOICE
  "Знайти інший шлях" CHOICE
  "Повернутися на берег" CHOICE
  WAIT_CHOICE
  dup 1 = IF drop GOTO: stone_chamber THEN
  dup 2 = IF drop GOTO: forest THEN
  drop GOTO: beach
;SCENE

SCENE: stone_chamber
  CLS
  "Під скелею" LOCATION:
  "За каменем відкривається вузька ніша." .
  "Усередині лежить старий компас. Стрілка не показує на північ." .
  "Вона спрямована прямо вниз." .
  1 "found_compass" !
  1 "strange_compass" ITEM+
  "Забрати компас" CHOICE
  "Залишити його" CHOICE
  WAIT_CHOICE
  1 = IF
    GOTO: beach
  ELSE
    GOTO: beach
  THEN
;SCENE

SCENE: dock
  CLS
  "Стара пристань" LOCATION:
  "Дерево прогнило, але під водою видно металеву конструкцію." .
  "На причалі стоїть іржавий візок." .
  "У його ящику лежить порожня пляшка з написом «станція 3»." .
  "Знайти станцію" CHOICE
  "Оглянути візок" CHOICE
  "Повернутися на берег" CHOICE
  WAIT_CHOICE
  dup 1 = IF drop GOTO: forest THEN
  dup 2 = IF drop GOTO: cart THEN
  drop GOTO: beach
;SCENE

SCENE: cart
  CLS
  "Стара пристань" LOCATION:
  "Під візком захована металева пластина." .
  "На ній схема острова та сім точок, з'єднаних лініями." .
  "Одна точка позначена S-17. Інші шість стерті." .
  1 "found_stone" !
  1 "knows_origin" !
  "Взяти пластину" CHOICE
  "Залишити" CHOICE
  WAIT_CHOICE
  1 = IF
    1 "map_plate" ITEM+
  THEN
  GOTO: beach
;SCENE

SCENE: forest
  CLS
  "Ліс" LOCATION:
  "Дерева ростуть надто рівними рядами." .
  "Через кожні кілька десятків метрів стоять кам'яні стовпи." .
  "На одному є цифра 3." .
  "На іншому — 6." .
  "Ти знаходиш вузьку дорогу, яка веде вглиб острова." .
  "Йти дорогою" CHOICE
  "Дослідити стовпи" CHOICE
  "Покликати когось" CHOICE
  "Повернутися" CHOICE
  WAIT_CHOICE
  dup 1 = IF drop GOTO: station_road THEN
  dup 2 = IF drop GOTO: markers THEN
  dup 3 = IF drop GOTO: girl THEN
  drop GOTO: beach
;SCENE

SCENE: markers
  CLS
  "Лісові стовпи" LOCATION:
  "Цифри утворюють послідовність: 1, 3, 6, 10." .
  "Наступного стовпа бракує." .
  "На землі лежить кам'яний уламок із числом 15." .
  1 "found_stone" !
  1 "knows_north" !
  "Забрати уламок" CHOICE
  "Йти далі" CHOICE
  WAIT_CHOICE
  1 = IF
    1 "stone_marker" ITEM+
  THEN
  GOTO: station_road
;SCENE

SCENE: station_road
  CLS
  "Станція 3" LOCATION:
  "За деревами з'являється бетонна споруда." .
  "На фасаді: «Гідрометеорологічна станція №3»." .
  "Але будівля явно не працює десятки років." .
  "У вікні видно свіже світло." .
  "Увійти" CHOICE
  "Обійти будівлю" CHOICE
  "Спостерігати здалеку" CHOICE
  WAIT_CHOICE
  dup 1 = IF drop GOTO: station THEN
  dup 2 = IF drop GOTO: station_back THEN
  drop GOTO: station_watch
;SCENE

SCENE: station_watch
  CLS
  "Станція 3" LOCATION:
  "Ти спостерігаєш крізь дерева." .
  "У вікні з'являється людина." .
  "Вона дивиться прямо на тебе." .
  "Через секунду світло гасне." .
  "Увійти" CHOICE
  "Втекти" CHOICE
  WAIT_CHOICE
  1 = IF
    GOTO: station
  ELSE
    GOTO: beach
  THEN
;SCENE

SCENE: station_back
  CLS
  "Станція 3" LOCATION:
  "Позаду станції є вентиляційний люк." .
  "Люк відкритий." .
  "Звідти тягне холодним повітрям." .
  "Залізти всередину" CHOICE
  "Повернутися до входу" CHOICE
  WAIT_CHOICE
  1 = IF
    GOTO: station_duct
  ELSE
    GOTO: station
  THEN
;SCENE

SCENE: station
  CLS
  "Станція 3" LOCATION:
  "Усередині чистіше, ніж очікувалося." .
  "На столі працює лампа. Поруч лежать свіжі записи." .
  "Але нікого немає." .
  "На стіні висить карта острова." .
  "Знайти записи" CHOICE
  "Оглянути карту" CHOICE
  "Піти в коридор" CHOICE
  "Вийти" CHOICE
  WAIT_CHOICE
  dup 1 = IF drop GOTO: diary THEN
  dup 2 = IF drop GOTO: station_map THEN
  dup 3 = IF drop GOTO: corridor THEN
  drop GOTO: beach
;SCENE

SCENE: diary
  CLS
  "Станція 3" LOCATION:
  "У журналі записано: «Сьогодні знову з'явився сигнал із точки 0»." .
  "«Якщо сигнал повториться сім разів, вони прокинуться»." .
  "Останній запис зроблений сьогодні." .
  1 "found_diary" !
  1 "knows_watchers" !
  1 "diary" ITEM+
  "Читати далі" CHOICE
  "Повернутися" CHOICE
  WAIT_CHOICE
  1 = IF
    GOTO: diary_secret
  ELSE
    GOTO: station
  THEN
;SCENE

SCENE: diary_secret
  CLS
  "Станція 3" LOCATION:
  "На останній сторінці: «S-17 була сьомою. До неї були S-1...S-6.»" .
  "«Жодна з них не мала бути активною після 1986.»" .
  "«Але хтось на материку знову подає сигнал.»" .
  1 "knows_origin" !
  "Знайти автора журналу" CHOICE
  "Повернутися" CHOICE
  WAIT_CHOICE
  1 = IF
    GOTO: cartographer
  ELSE
    GOTO: station
  THEN
;SCENE

SCENE: station_map
  CLS
  "Станція 3" LOCATION:
  "Карта показує сім станцій навколо острова." .
  "Центр позначений символом ока." .
  "Під картою напис: «Північ — це не напрямок. Північ — це станція.»" .
  1 "knows_north" !
  1 "knows_station" !
  "Взяти карту" CHOICE
  "Піти в коридор" CHOICE
  WAIT_CHOICE
  1 = IF
    1 "station_map" ITEM+
  THEN
  GOTO: corridor
;SCENE

SCENE: corridor
  CLS
  "Станція 3" LOCATION:
  "Коридор веде вниз." .
  "На стіні сім дверей." .
  "Шість замкнені. Сьомі відкриті." .
  "Спуститися" CHOICE
  "Відкрити замкнені двері" CHOICE
  "Повернутися" CHOICE
  WAIT_CHOICE
  dup 1 = IF drop GOTO: underground THEN
  dup 2 = IF drop GOTO: locked_doors THEN
  drop GOTO: station
;SCENE

SCENE: locked_doors
  CLS
  "Станція 3" LOCATION:
  "На кожних дверях інша назва: S-1, S-2, S-3, S-4, S-5, S-6." .
  "На замку сьомих дверей напис: «Перші шість спостерігають. Сьома відповідає»." .
  "Ти чуєш кроки нагорі." .
  "Повернутися" CHOICE
  "Залишитися й чекати" CHOICE
  WAIT_CHOICE
  1 = IF
    GOTO: station
  ELSE
    GOTO: watcher
  THEN
;SCENE

SCENE: station_duct
  CLS
  "Вентиляційний тунель" LOCATION:
  "Тунель спускається під станцію." .
  "На стіні нанесені свіжі подряпини." .
  "Хтось регулярно проходить цим шляхом." .
  "Попереду світло." .
  "Йти до світла" CHOICE
  "Повернутися" CHOICE
  WAIT_CHOICE
  1 = IF
    GOTO: watcher
  ELSE
    GOTO: station
  THEN
;SCENE

SCENE: watcher
  CLS
  "Підземний тунель" LOCATION:
  "Ти бачиш чоловіка в сірому плащі." .
  "Він тримає старий радіоприймач." .
  "— Ти прийшов із маяка, — каже він." .
  "— Отже, S-17 справді відповіла." .
  1 "met_watchman" !
  "Запитати, хто він" CHOICE
  "Показати компас" CHOICE
  "Втекти" CHOICE
  WAIT_CHOICE
  dup 1 = IF drop GOTO: watchman THEN
  dup 2 = IF drop GOTO: watchman_compass THEN
  drop GOTO: beach
;SCENE

SCENE: watchman
  CLS
  "Підземний тунель" LOCATION:
  "— Я наглядач." .
  "— Не охоронець. Наглядач." .
  "— Моє завдання — стежити, щоб сім станцій ніколи не працювали одночасно." .
  1 "knows_watchers" !
  "Запитати про шість станцій" CHOICE
  "Запитати про центр острова" CHOICE
  "Запитати про S-17" CHOICE
  WAIT_CHOICE
  dup 1 = IF drop GOTO: six_stations THEN
  dup 2 = IF drop GOTO: center_secret THEN
  drop GOTO: s17_secret
;SCENE

SCENE: watchman_compass
  CLS
  "Підземний тунель" LOCATION:
  "Наглядач бачить компас і різко блідне." .
  "— Де ти це знайшов?" .
  "— Цей компас був у центрі. Його не повинно існувати на поверхні." .
  1 "knows_origin" !
  "Показати компас" CHOICE
  "Сховати компас" CHOICE
  WAIT_CHOICE
  1 = IF
    GOTO: center_secret
  ELSE
    GOTO: watchman
  THEN
;SCENE

SCENE: six_stations
  CLS
  "Підземний тунель" LOCATION:
  "— S-1 до S-6 були побудовані задовго до Вороніна." .
  "— Він лише відновив S-7, коли знайшов її креслення." .
  "— Люди думали, що це навігаційна система." .
  "— Насправді вона визначає не положення кораблів." .
  "— Вона визначає положення чогось іншого." .
  1 "knows_origin" !
  "Запитати чого саме" CHOICE
  "Повернутися" CHOICE
  WAIT_CHOICE
  1 = IF
    GOTO: origin_truth
  ELSE
    GOTO: watchman
  THEN
;SCENE

SCENE: s17_secret
  CLS
  "Підземний тунель" LOCATION:
  "— S-17 була аварійною станцією." .
  "— Вона не повинна була приймати сигнал." .
  "— Її завдання — відповідати, коли прокидається центр." .
  1 "knows_station" !
  "Запитати, що таке центр" CHOICE
  "Попросити показати центр" CHOICE
  WAIT_CHOICE
  1 = IF
    GOTO: center_secret
  ELSE
    GOTO: center
  THEN
;SCENE

SCENE: center_secret
  CLS
  "Підземний комплекс" LOCATION:
  "Наглядач відчиняє сталеві двері." .
  "За ними — круглий зал." .
  "У центрі висить чорний металевий диск." .
  "Твій компас повертається до нього." .
  1 "entered_station" !
  1 "knows_station" !
  "Торкнутися диска" CHOICE
  "Оглянути стіни" CHOICE
  "Не чіпати" CHOICE
  WAIT_CHOICE
  dup 1 = IF drop GOTO: disk THEN
  dup 2 = IF drop GOTO: walls THEN
  drop GOTO: watchman
;SCENE

SCENE: walls
  CLS
  "Центральний зал" LOCATION:
  "На стінах ти бачиш тисячі маленьких символів." .
  "Серед них — назви портів, міст і островів." .
  "Деякі назви перекреслені." .
  "На самому верху напис: «Коли сім відповідають, восьмий відкривається»." .
  1 "knows_origin" !
  "Шукати восьмий знак" CHOICE
  "Повернутися" CHOICE
  WAIT_CHOICE
  1 = IF
    GOTO: eighth
  ELSE
    GOTO: center
  THEN
;SCENE

SCENE: disk
  CLS
  "Центральний зал" LOCATION:
  "Ти торкаєшся чорного диска." .
  2000 DELAY
  "Усі сім станцій острова одночасно видають короткий сигнал." .
  "Наглядач кричить: «Зупини це!»" .
  "На диску з'являється карта материка." .
  "На ній світиться одне місто." .
  1 "found_signal" !
  1 "knows_origin" !
  "Від'єднати живлення" CHOICE
  "Дочекатися продовження" CHOICE
  WAIT_CHOICE
  1 = IF
    GOTO: shutdown
  ELSE
    GOTO: awakening
  THEN
;SCENE

SCENE: eighth
  CLS
  "Центральний зал" LOCATION:
  "Восьмий знак знаходиться не на острові." .
  "Він вигравіруваний на твоєму компасі." .
  "Ти розумієш: компас — не прилад." .
  "Це ключ." .
  1 "found_key" !
  1 "knows_origin" !
  "Повернутися до наглядача" CHOICE
  "Вставити компас у диск" CHOICE
  WAIT_CHOICE
  1 = IF
    GOTO: watchman
  ELSE
    GOTO: awakening
  THEN
;SCENE

SCENE: origin_truth
  CLS
  "Центральний тунель" LOCATION:
  "— Система визначає не кораблі, — каже наглядач." .
  "— Вона визначає місця, де простір поводиться неправильно." .
  "— Сім станцій утворюють мережу." .
  "— А S-17 — її аварійний вузол." .
  "— Якщо всі сім станцій відповідають одночасно, мережа відкриває восьмий вузол." .
  1 "knows_origin" !
  "Запитати, де восьмий вузол" CHOICE
  "Зупинити систему" CHOICE
  WAIT_CHOICE
  1 = IF
    GOTO: eighth_location
  ELSE
    GOTO: shutdown
  THEN
;SCENE

SCENE: eighth_location
  CLS
  "Центральний зал" LOCATION:
  "Наглядач показує на карту." .
  "Восьмий вузол знаходиться там, де ти почав пошук: біля старого маяка." .
  "— S-17 була не сьомою." .
  "— Вона була першою, яку ми випадково активували." .
  "Ти розумієш, що події першої серії були не випадковістю." .
  "Зберегти інформацію" CHOICE
  "Вимкнути систему" CHOICE
  WAIT_CHOICE
  1 = IF
    GOTO: island_finale
  ELSE
    GOTO: shutdown
  THEN
;SCENE

SCENE: cartographer
  CLS
  "Станція 3" LOCATION:
  "Ти знаходиш людину за картою." .
  "Вона представляється: Олена, картограф." .
  "— Я приїхала сюди десять років тому." .
  "— Відтоді острів змінює положення." .
  1 "met_cartographer" !
  "Запитати про острів" CHOICE
  "Запитати про сім станцій" CHOICE
  "Попросити карту" CHOICE
  WAIT_CHOICE
  dup 1 = IF drop GOTO: cartographer_island THEN
  dup 2 = IF drop GOTO: cartographer_stations THEN
  drop GOTO: cartographer_map
;SCENE

SCENE: cartographer_island
  CLS
  "Станція 3" LOCATION:
  "— Острів не рухається морем." .
  "— Рухається система координат навколо нього." .
  "— Саме тому старі карти суперечать одна одній." .
  1 "knows_north" !
  "Запитати, хто створив систему" CHOICE
  "Попросити допомоги" CHOICE
  WAIT_CHOICE
  1 = IF
    GOTO: origin_truth
  ELSE
    1 "trusted_cartographer" !
    GOTO: cartographer_map
  THEN
;SCENE

SCENE: cartographer_stations
  CLS
  "Станція 3" LOCATION:
  "Олена показує сім точок." .
  "— Вони не утворюють коло." .
  "— Вони утворюють стрілку." .
  "Стрілка спрямована на материк." .
  1 "knows_station" !
  "Запитати, куди саме" CHOICE
  "Взяти карту" CHOICE
  WAIT_CHOICE
  1 = IF
    GOTO: mainland_signal
  ELSE
    GOTO: cartographer_map
  THEN
;SCENE

SCENE: cartographer_map
  CLS
  "Станція 3" LOCATION:
  "Олена дає тобі карту." .
  "На ній є одна точка, якої немає на жодній іншій карті." .
  "Підпис: «Вузол 0»." .
  1 "found_diary" !
  1 "map_node" ITEM+
  1 "knows_origin" !
  "Піти до вузла 0" CHOICE
  "Повернутися" CHOICE
  WAIT_CHOICE
  1 = IF
    GOTO: node_zero
  ELSE
    GOTO: station
  THEN
;SCENE

SCENE: node_zero
  CLS
  "Вузол 0" LOCATION:
  "У центрі острова стоїть кругла бетонна плита." .
  "На ній немає дверей, але компас починає обертатися." .
  "Під плитою чути голос." .
  "— Ви знову прийшли." .
  "Ви — хто?" CHOICE
  "Мовчати" CHOICE
  WAIT_CHOICE
  1 = IF
    GOTO: voice
  ELSE
    GOTO: voice
  THEN
;SCENE

SCENE: voice
  CLS
  "Вузол 0" LOCATION:
  "— Неважливо, хто ви." .
  "— Важливо, що S-17 відповіла." .
  "— Попередні шість відповідей були втрачені." .
  "— Восьма відповідь належить вам." .
  1 "knows_origin" !
  "Запитати, що означає відповідь" CHOICE
  "Відмовитися" CHOICE
  WAIT_CHOICE
  1 = IF
    GOTO: awakening
  ELSE
    GOTO: shutdown
  THEN
;SCENE

SCENE: mainland_signal
  CLS
  "Кімната картографа" LOCATION:
  "Олена показує на місто на материку." .
  "— Сигнал іде звідти." .
  "Ти впізнаєш назву." .
  "Це те саме місто, де жив Марко." .
  1 "knows_watchers" !
  "Повернутися на материк" CHOICE
  "Залишитися на острові" CHOICE
  WAIT_CHOICE
  1 = IF
    GOTO: mainland
  ELSE
    GOTO: island_finale
  THEN
;SCENE

SCENE: mainland
  CLS
  "Повернення" LOCATION:
  "Корабель повертає тебе на материк." .
  "Перед висадкою радіоприймач оживає." .
  "Три короткі сигнали. Два. Один." .
  "Потім голос Марка: «Якщо ти це чуєш, острів уже не є головною проблемою»." .
  "Повернутися до міста" CHOICE
  "Знищити радіоприймач" CHOICE
  WAIT_CHOICE
  1 = IF
    GOTO: city
  ELSE
    GOTO: shutdown
  THEN
;SCENE

SCENE: city
  CLS
  "Місто" LOCATION:
  "Кабінет Марка порожній." .
  "На столі лежить карта семи станцій." .
  "На ній уже намальована восьма." .
  "Поруч записка: «Ти запізнився. Вони прокинулися»." .
  1 "knows_origin" !
  1 "found_diary" !
  "Піти до архіву" CHOICE
  "Піти до старого маяка" CHOICE
  "Зателефонувати Марку" CHOICE
  WAIT_CHOICE
  dup 1 = IF drop GOTO: city_archive THEN
  dup 2 = IF drop GOTO: lighthouse_return THEN
  drop GOTO: marc_call
;SCENE

SCENE: city_archive
  CLS
  "Міський архів" LOCATION:
  "Ти знаходиш креслення 1952 року." .
  "На першій сторінці: «Проєкт Око. Доступ заборонений»." .
  "Автор — невідомий." .
  "На останній сторінці стоїть підпис: S-0." .
  1 "found_diary" !
  1 "knows_origin" !
  "Забрати креслення" CHOICE
  "Повернутися" CHOICE
  WAIT_CHOICE
  1 = IF
    1 "blueprint" ITEM+
  THEN
  GOTO: lighthouse_return
;SCENE

SCENE: lighthouse_return
  CLS
  "Старий маяк" LOCATION:
  "Ти повертаєшся туди, звідки все почалося." .
  "Фундамент маяка тепер має нові двері." .
  "На них напис: S-0." .
  "Відкрити двері" CHOICE
  "Знищити двері" CHOICE
  "Піти" CHOICE
  WAIT_CHOICE
  dup 1 = IF drop GOTO: s0 THEN
  dup 2 = IF drop GOTO: destroy_s0 THEN
  drop GOTO: retreat
;SCENE

SCENE: marc_call
  CLS
  "Місто" LOCATION:
  "Телефон дзвонить у відповідь одразу." .
  "— Не відкривай S-0, — каже Марко." .
  "— Я вже відкрив її одного разу." .
  "— Після цього Воронін зник." .
  "Ти питаєш, що всередині." .
  "— Не кімната. Пам'ять." .
  1 "knows_origin" !
  "Повернутися до маяка" CHOICE
  "Повірити Марку" CHOICE
  WAIT_CHOICE
  1 = IF
    GOTO: lighthouse_return
  ELSE
    1 "trusted_cartographer" !
    GOTO: mainland
  THEN
;SCENE

SCENE: s0
  CLS
  "Станція S-0" LOCATION:
  "За дверима немає кімнати." .
  "Є лише біле світло." .
  "Ти робиш крок — і чуєш десятки голосів." .
  "Серед них голос Вороніна, Марка і власний." .
  "Ти бачиш усі можливі версії подій біля маяка." .
  1 "opened_vault" !
  "Почути правду" CHOICE
  "Закрити S-0" CHOICE
  WAIT_CHOICE
  1 = IF
    GOTO: truth_s0
  ELSE
    GOTO: close_s0
  THEN
;SCENE

SCENE: truth_s0
  CLS
  "S-0" LOCATION:
  "Тепер ти розумієш: S-1...S-7 були експериментом." .
  "S-0 була прототипом." .
  "Її завдання — не передавати сигнали." .
  "Вона зберігає інформацію про всі сигнали, які коли-небудь отримала мережа." .
  "Саме тому вона пам'ятає людей, які давно померли." .
  "Ти бачиш останній запис." .
  "Дата — сьогодні." .
  1 "found_signal" !
  "Продовжити" CHOICE
  WAIT_CHOICE
  drop GOTO: season2_finale
;SCENE

SCENE: close_s0
  CLS
  "Станція S-0" LOCATION:
  "Ти закриваєш двері." .
  "Світло гасне." .
  "У кишені компас раптом стає теплим." .
  "На його кришці з'являється новий знак." .
  "Три кола." .
  "Ти не знаєш, що він означає." .
  "Острів закрито. На компасі з'явився новий знак." VICTORY
;SCENE

SCENE: destroy_s0
  CLS
  "Старий маяк" LOCATION:
  "Ти руйнуєш замок і підриваєш механізм дверей." .
  "S-0 більше ніхто не відкриє." .
  "Але тієї ж ночі на горизонті спалахує сім світел." .
  "Система продовжує працювати без тебе." .
  "S-0 знищено, але сім станцій уже прокинулися." DEFEAT
;SCENE

SCENE: shutdown
  CLS
  "Центральний вузол" LOCATION:
  "Ти вимикаєш живлення." .
  "Один за одним згасають сигнали семи станцій." .
  "Острів зникає з карти." .
  "Через годину навколо тебе знову лише море." .
  "Ти зберіг світ від чогось, чого ніхто не розумів." .
  "Острів зник. Мережу зупинено." VICTORY
;SCENE

SCENE: awakening
  CLS
  "Центральний вузол" LOCATION:
  "Ти не вимикаєш систему." .
  "Сім станцій відповідають одночасно." .
  "Чорний диск відкривається." .
  "На карті з'являються сотні точок по всій планеті." .
  "Одна за одною вони починають світитися." .
  "Наглядач шепоче: «Це неможливо. Вони прокидаються.»" .
  "Острів здригається." .
  "Мережа прокинулась по всій планеті." VICTORY
;SCENE

SCENE: island_finale
  CLS
  "Острів без карти" LOCATION:
  "Ти стоїш на березі перед світанком." .
  "Острів повільно зникає в тумані." .
  "На воді залишається тільки знак S-17." .
  "Ти знаєш тепер: це була не станція." .
  "Це був ключ." .
  "І хтось на материку вже використав його." .
  "Продовжити пошуки" CHOICE
  "Залишити все в минулому" CHOICE
  WAIT_CHOICE
  1 = IF
    GOTO: season2_finale
  ELSE
    GOTO: close_s0
  THEN
;SCENE

SCENE: season2_finale
  CLS
  "Кінець другої серії" LOCATION:
  "Через сім днів ти отримуєш посилку без адреси відправника." .
  "Усередині — фотографія старого маяка." .
  "На фотографії немає маяка." .
  "Є лише чорна точка над морем." .
  "На звороті написано:" .
  "«S-0 не зберігає минуле. Вона зберігає те, що ще не сталося.»" .
  "У цей момент компас у твоїй кишені повертається на північ." .
  "А потім — на схід." .
  "Ти дивишся на карту." .
  "Там, де має бути схід, з'являється нова точка." .
  "S-8." .
  "Друга серія завершена. На карті з'явилась точка S-8." VICTORY
;SCENE

SCENE: retreat
  CLS
  "Море" LOCATION:
  "Ти залишаєш острів." .
  "Через кілька годин він зникає з горизонту." .
  "Ти перевіряєш координати." .
  "Там знову відкрите море." .
  "Але на радарі залишається один короткий сигнал." .
  "Три короткі. Два. Один." .
  "Ти пішов від острова — але сигнал лишився на радарі." FINISH
;SCENE
