"Маяк, якого не було" TITLE:
"QuestForth" AUTHOR:
"1.0" VERSION:

\ ============================================================
\ МАЯК, ЯКОГО НЕ БУЛО
\ Детективний квест з розгалуженнями, доказами та кількома фіналами.
\ ============================================================

0 "found_lens" !
0 "found_key" !
0 "found_log" !
0 "found_photo" !
0 "talked_keeper" !
0 "talked_doctor" !
0 "talked_sailor" !
0 "opened_lighthouse" !
0 "knows_signal" !
0 "knows_tunnel" !
0 "trust_keeper" !
0 "trust_sailor" !
0 "accused" !
0 "saved_before_finale" !

: checkpoint
  "checkpoint" SAVE
;

: has_all_clues
  "found_lens" @
  "found_key" @ AND
  "found_log" @ AND
  "found_photo" @ AND
  "knows_signal" @ AND
;

SCENE: boot
  GOTO: harbor
;SCENE

SCENE: harbor
  CLS
  "Старий порт" LOCATION:
  "Дощ стирає сліди з бруківки. Перед тобою затока, рибальські човни та чорний силует скелі." .
  "Три ночі поспіль кораблі бачили на скелі світло маяка." .
  "Проблема в одному: маяк на цій скелі зруйнували сорок років тому." .
  "Міський лікар попросив тебе з'ясувати, що відбувається до приходу нічного порома." .
  "Підійти до сторожки" CHOICE
  "Зайти в портовий архів" CHOICE
  "Піти до човняра" CHOICE
  "Оглянути старий причал" CHOICE
  WAIT_CHOICE
  dup 1 = IF drop GOTO: keeper THEN
  dup 2 = IF drop GOTO: archive THEN
  dup 3 = IF drop GOTO: sailor THEN
  drop GOTO: pier
;SCENE

SCENE: keeper
  CLS
  "Сторожка" LOCATION:
  "Сторож маяка, Гнат, сидить біля холодної печі. На столі лежить мокрий плащ." .
  "Гнат дивиться на тебе так, ніби вже знає, навіщо ти прийшов." .
  "Запитати про світло" CHOICE
  "Запитати про старий маяк" CHOICE
  "Оглянути сторожку" CHOICE
  "Повернутися в порт" CHOICE
  WAIT_CHOICE
  dup 1 = IF drop GOTO: keeper_light THEN
  dup 2 = IF drop GOTO: keeper_history THEN
  dup 3 = IF drop GOTO: keeper_room THEN
  drop GOTO: harbor
;SCENE

SCENE: keeper_light
  CLS
  "Сторожка" LOCATION:
  "— Світло? — перепитує Гнат. — Я нічого не бачив." .
  "Він говорить занадто швидко." .
  "Коли ти згадуєш корабель «Світанок», він раптом замовкає." .
  1 "talked_keeper" !
  1 "trust_keeper" !
  "Попросити Гната про карту" CHOICE
  "Повернутися" CHOICE
  WAIT_CHOICE
  1 = IF
    GOTO: map_room
  ELSE
    GOTO: keeper
  THEN
;SCENE

SCENE: keeper_history
  CLS
  "Сторожка" LOCATION:
  "Гнат розповідає, що маяк збудував інженер Северин Воронін." .
  "У 1986 році його син зник під час шторму. Після цього маяк закрили." .
  "— Але світло тоді було справжнім, — тихо додає Гнат." .
  1 "talked_keeper" !
  "Запитати, де ключ від маяка" CHOICE
  "Повернутися" CHOICE
  WAIT_CHOICE
  1 = IF
    GOTO: keeper_key
  ELSE
    GOTO: keeper
  THEN
;SCENE

SCENE: keeper_room
  CLS
  "Сторожка" LOCATION:
  "Під ліжком стоїть старий ящик. Усередині — карта острова та латунна лінза." .
  "На лінзі вигравірувано: S-17." .
  1 "found_lens" !
  1 "knows_signal" !
  1 "lens" ITEM+
  "Запитати Гната про S-17" CHOICE
  "Забрати лінзу й повернутися" CHOICE
  WAIT_CHOICE
  1 = IF
    GOTO: keeper_secret
  ELSE
    GOTO: harbor
  THEN
;SCENE

SCENE: keeper_secret
  CLS
  "Сторожка" LOCATION:
  "Гнат блідне, побачивши лінзу." .
  "— Цю річ не повинно було бути знайдено." .
  "Він визнає, що інколи сам бачив світло на скелі, але не знає, хто його вмикає." .
  "Запитати про підземний тунель" CHOICE
  "Нічого не казати про лінзу" CHOICE
  WAIT_CHOICE
  1 = IF
    GOTO: tunnel_hint
  ELSE
    GOTO: harbor
  THEN
;SCENE

SCENE: keeper_key
  CLS
  "Сторожка" LOCATION:
  "Гнат довго мовчить, а потім дістає з кишені маленький залізний ключ." .
  "— Він від старих дверей унизу маяка. Але я туди не ходжу." .
  1 "found_key" !
  1 "lighthouse_key" ITEM+
  "Запитати, чому він боїться" CHOICE
  "Повернутися" CHOICE
  WAIT_CHOICE
  1 = IF
    GOTO: keeper_fear
  ELSE
    GOTO: harbor
  THEN
;SCENE

SCENE: keeper_fear
  CLS
  "Сторожка" LOCATION:
  "— Там є кімната без вікон. Іноді звідти чути кроки." .
  "— Я одного разу відкрив її. Побачив стіну, а за нею почув море." .
  1 "knows_tunnel" !
  "Запитати про фотографію" CHOICE
  "Повернутися в порт" CHOICE
  WAIT_CHOICE
  1 = IF
    GOTO: archive
  ELSE
    GOTO: harbor
  THEN
;SCENE

SCENE: archive
  CLS
  "Портовий архів" LOCATION:
  "Пил, мокрі журнали та коробки з документами. Архіваріус дозволяє тобі працювати самому." .
  "Знайти записи про маяк" CHOICE
  "Знайти записи про корабель «Світанок»" CHOICE
  "Знайти старі фотографії" CHOICE
  "Повернутися в порт" CHOICE
  WAIT_CHOICE
  dup 1 = IF drop GOTO: archive_lighthouse THEN
  dup 2 = IF drop GOTO: archive_ship THEN
  dup 3 = IF drop GOTO: archive_photo THEN
  drop GOTO: harbor
;SCENE

SCENE: archive_lighthouse
  CLS
  "Портовий архів" LOCATION:
  "У журналі 1986 року є запис: «Світловий механізм демонтовано. Оптика вилучена»." .
  "Але наступна сторінка вирвана." .
  "На полях хтось написав: S-17 / третій імпульс." .
  1 "knows_signal" !
  "Взяти запис із собою" CHOICE
  "Шукати далі" CHOICE
  WAIT_CHOICE
  1 = IF
    GOTO: archive_note
  ELSE
    GOTO: archive
  THEN
;SCENE

SCENE: archive_note
  CLS
  "Портовий архів" LOCATION:
  "Ти переписуєш координати та позначку S-17 у свій блокнот." .
  1 "found_log" !
  1 "logbook" ITEM+
  "Перевірити журнали після 1986 року" CHOICE
  "Повернутися" CHOICE
  WAIT_CHOICE
  1 = IF
    GOTO: archive_after
  ELSE
    GOTO: harbor
  THEN
;SCENE

SCENE: archive_after
  CLS
  "Портовий архів" LOCATION:
  "У записах за останні десять років немає жодного офіційного відвідування скелі." .
  "Але щомісяця хтось купував у порту двадцять літрів гасу." .
  "Покупець записаний як «С. Воронін»." .
  "Ім'я не мало б існувати: Северин Воронін помер у 1991 році." .
  1 "knows_tunnel" !
  "Повернутися в порт" CHOICE
  WAIT_CHOICE
  drop GOTO: harbor
;SCENE

SCENE: archive_ship
  CLS
  "Портовий архів" LOCATION:
  "Корабель «Світанок» зник у 1986 році. Офіційно — шторм." .
  "Капітан перед зникненням передав радіограму: «Світло не на скелі. Світло під нами»." .
  "Ти знаходиш другий запис: «Три короткі спалахи. Потім темрява»." .
  1 "knows_signal" !
  "Повернутися в порт" CHOICE
  WAIT_CHOICE
  drop GOTO: harbor
;SCENE

SCENE: archive_photo
  CLS
  "Портовий архів" LOCATION:
  "У старій коробці лежить фотографія маяка." .
  "На звороті дата: 17 жовтня 1986." .
  "На фотографії видно дві людини біля входу: Северин Воронін і невідомий чоловік." .
  "На рукаві невідомого — емблема міської лікарні." .
  1 "found_photo" !
  1 "photo" ITEM+
  "Піти до лікаря" CHOICE
  "Повернутися в порт" CHOICE
  WAIT_CHOICE
  1 = IF
    GOTO: doctor
  ELSE
    GOTO: harbor
  THEN
;SCENE

SCENE: sailor
  CLS
  "Човнярська пристань" LOCATION:
  "Старий Левко лагодить мотор. Він не дивиться тобі в очі." .
  "Запитати про світло" CHOICE
  "Запитати про скелю" CHOICE
  "Запитати про «Світанок»" CHOICE
  "Попросити човен" CHOICE
  WAIT_CHOICE
  dup 1 = IF drop GOTO: sailor_light THEN
  dup 2 = IF drop GOTO: sailor_rock THEN
  dup 3 = IF drop GOTO: sailor_ship THEN
  drop GOTO: sailor_boat
;SCENE

SCENE: sailor_light
  CLS
  "Пристань" LOCATION:
  "— Світло бачив, — каже Левко. — Три спалахи, пауза, два спалахи." .
  "— Це не маяк. Так подають сигнал із моря." .
  1 "talked_sailor" !
  1 "knows_signal" !
  1 "trust_sailor" !
  "Запитати, хто подає сигнал" CHOICE
  "Повернутися" CHOICE
  WAIT_CHOICE
  1 = IF
    GOTO: sailor_signal
  ELSE
    GOTO: harbor
  THEN
;SCENE

SCENE: sailor_rock
  CLS
  "Пристань" LOCATION:
  "— Під скелею є печера. На старих картах її нема." .
  "— Вхід відкривається тільки під час відпливу." .
  1 "knows_tunnel" !
  "Запитати про човен" CHOICE
  "Повернутися" CHOICE
  WAIT_CHOICE
  1 = IF
    GOTO: sailor_boat
  ELSE
    GOTO: harbor
  THEN
;SCENE

SCENE: sailor_ship
  CLS
  "Пристань" LOCATION:
  "Левко нарешті зізнається: у ніч зникнення «Світанку» він чув удари по металу з боку скелі." .
  "Наступного ранку бачив у воді уламок латуні." .
  1 "found_lens" !
  "Повернутися" CHOICE
  WAIT_CHOICE
  drop GOTO: harbor
;SCENE

SCENE: sailor_signal
  CLS
  "Пристань" LOCATION:
  "— Я не знаю, хто подає сигнал, — каже Левко. — Але він означає: «Не наближайтесь»." .
  "— А минулого тижня я побачив людину на скелі. Вона була в білому халаті." .
  1 "knows_signal" !
  "Піти до лікаря" CHOICE
  "Повернутися" CHOICE
  WAIT_CHOICE
  1 = IF
    GOTO: doctor
  ELSE
    GOTO: harbor
  THEN
;SCENE

SCENE: sailor_boat
  CLS
  "Пристань" LOCATION:
  "Левко погоджується відвезти тебе до скелі, але лише вночі." .
  "— Якщо побачиш три спалахи, повертайся." .
  1 "talked_sailor" !
  "Плисти зараз" CHOICE
  "Спочатку дослідити причал" CHOICE
  WAIT_CHOICE
  1 = IF
    GOTO: night_boat
  ELSE
    GOTO: pier
  THEN
;SCENE

SCENE: pier
  CLS
  "Старий причал" LOCATION:
  "Під дошками ти знаходиш металеву скриньку, притиснуту до палі." .
  "Замок старий, але ключ від сторожки підходить." .
  "Відкрити скриньку" CHOICE
  "Залишити її" CHOICE
  WAIT_CHOICE
  1 = IF
    GOTO: box
  ELSE
    GOTO: harbor
  THEN
;SCENE

SCENE: box
  CLS
  "Старий причал" LOCATION:
  "У скриньці — половина корабельного журналу та маленька фотографія." .
  "На фото зображений молодий Северин Воронін поруч із тим самим чоловіком у халаті." .
  "На звороті: «Якщо вони знайдуть нас — сигнал S-17»." .
  1 "found_log" !
  1 "found_photo" !
  1 "knows_signal" !
  1 "logbook" ITEM+
  1 "photo" ITEM+
  "Повернутися в порт" CHOICE
  "Піти до лікаря" CHOICE
  WAIT_CHOICE
  1 = IF
    GOTO: harbor
  ELSE
    GOTO: doctor
  THEN
;SCENE

SCENE: doctor
  CLS
  "Кабінет лікаря" LOCATION:
  "Лікар Марко зустрічає тебе надто привітно." .
  "На його столі лежить та сама емблема, що на старій фотографії." .
  "Запитати про маяк" CHOICE
  "Показати фотографію" CHOICE
  "Запитати про Северина" CHOICE
  "Вийти" CHOICE
  WAIT_CHOICE
  dup 1 = IF drop GOTO: doctor_lighthouse THEN
  dup 2 = IF drop GOTO: doctor_photo THEN
  dup 3 = IF drop GOTO: doctor_voronin THEN
  drop GOTO: harbor
;SCENE

SCENE: doctor_lighthouse
  CLS
  "Кабінет лікаря" LOCATION:
  "— Старі історії, — каже Марко. — Вам краще зайнятися чимось кориснішим." .
  "Він нервово прибирає зі столу ключ." .
  1 "talked_doctor" !
  "Попросити показати ключ" CHOICE
  "Вийти" CHOICE
  WAIT_CHOICE
  1 = IF
    GOTO: doctor_key
  ELSE
    GOTO: harbor
  THEN
;SCENE

SCENE: doctor_photo
  CLS
  "Кабінет лікаря" LOCATION:
  "Марко бачить фотографію і мовчить." .
  "— Це не те, що ви думаєте." .
  "Він визнає, що знав Северина, але стверджує, що той загинув." .
  1 "talked_doctor" !
  "Запитати про Світанок" CHOICE
  "Вийти" CHOICE
  WAIT_CHOICE
  1 = IF
    GOTO: doctor_ship
  ELSE
    GOTO: harbor
  THEN
;SCENE

SCENE: doctor_voronin
  CLS
  "Кабінет лікаря" LOCATION:
  "— Северин був генієм, — каже Марко. — Він боявся, що його винахід використають не ті люди." .
  "Ти питаєш про винахід." .
  "Марко відповідає: «Система сигналів. Вона могла передавати координати крізь товщу води»." .
  1 "knows_signal" !
  1 "talked_doctor" !
  "Запитати, де система" CHOICE
  "Вийти" CHOICE
  WAIT_CHOICE
  1 = IF
    GOTO: doctor_system
  ELSE
    GOTO: harbor
  THEN
;SCENE

SCENE: doctor_key
  CLS
  "Кабінет лікаря" LOCATION:
  "— Цей ключ нічого не означає." .
  "Але на ключі видно такий самий знак S-17." .
  "Ти розумієш: Марко приховує щось важливе." .
  1 "found_key" !
  "Піти до маяка" CHOICE
  "Повернутися в порт" CHOICE
  WAIT_CHOICE
  1 = IF
    GOTO: lighthouse
  ELSE
    GOTO: harbor
  THEN
;SCENE

SCENE: doctor_ship
  CLS
  "Кабінет лікаря" LOCATION:
  "Марко різко змінюється." .
  "— «Світанок» не затонув." .
  "— Він був потрібен тим, хто шукав систему Вороніна." .
  "— А тепер вони шукають її знову." .
  1 "knows_tunnel" !
  "Поставити пряме питання" CHOICE
  "Піти до маяка" CHOICE
  WAIT_CHOICE
  1 = IF
    GOTO: doctor_confession
  ELSE
    GOTO: lighthouse
  THEN
;SCENE

SCENE: doctor_system
  CLS
  "Кабінет лікаря" LOCATION:
  "Марко шепоче: «Під маяком. Але якщо відкриєш нижні двері, система запуститься автоматично»." .
  "— І тоді хтось у морі зрозуміє, що ти її знайшов." .
  1 "knows_tunnel" !
  "Піти до маяка" CHOICE
  "Зберегти інформацію й піти" CHOICE
  WAIT_CHOICE
  1 = IF
    GOTO: lighthouse
  ELSE
    1 "saved_before_finale" !
    checkpoint
    GOTO: harbor
  THEN
;SCENE

SCENE: doctor_confession
  CLS
  "Кабінет лікаря" LOCATION:
  "Марко зізнається: він допоміг Вороніну сховати систему." .
  "Але після смерті Вороніна сам почав використовувати сигнали, щоб відганяти кораблі від скелі." .
  "— Я думав, що захищаю її." .
  1 "talked_doctor" !
  1 "trust_keeper" !
  "Повірити Марку" CHOICE
  "Звинуватити Марка" CHOICE
  WAIT_CHOICE
  1 = IF
    GOTO: lighthouse
  ELSE
    1 "accused" !
    GOTO: lighthouse
  THEN
;SCENE

SCENE: map_room
  CLS
  "Сторожка" LOCATION:
  "На карті, яку дав Гнат, видно дивний квадрат під фундаментом маяка." .
  "Позначка S-17 стоїть саме там." .
  1 "knows_tunnel" !
  "Піти до маяка" CHOICE
  "Повернутися в порт" CHOICE
  WAIT_CHOICE
  1 = IF
    GOTO: lighthouse
  ELSE
    GOTO: harbor
  THEN
;SCENE

SCENE: tunnel_hint
  CLS
  "Сторожка" LOCATION:
  "Гнат показує на стару карту." .
  "— Тунель починається біля третьої опори причалу й виходить під маяком." .
  1 "knows_tunnel" !
  "Піти до причалу" CHOICE
  "Піти до маяка" CHOICE
  WAIT_CHOICE
  1 = IF
    GOTO: pier
  ELSE
    GOTO: lighthouse
  THEN
;SCENE

SCENE: lighthouse
  CLS
  "Зруйнований маяк" LOCATION:
  "Скеля зустрічає тебе ревом хвиль. Зі старої вежі видно весь порт." .
  "У фундаменті є двоє дверей: верхні іржаві, нижні майже нові." .
  "Відкрити верхні двері" CHOICE
  "Відкрити нижні двері" CHOICE
  "Обійти фундамент" CHOICE
  "Повернутися до човна" CHOICE
  WAIT_CHOICE
  dup 1 = IF drop GOTO: lighthouse_top THEN
  dup 2 = IF drop GOTO: lighthouse_bottom THEN
  dup 3 = IF drop GOTO: lighthouse_back THEN
  drop GOTO: night_boat
;SCENE

SCENE: lighthouse_top
  CLS
  "Вежа" LOCATION:
  "Нагорі немає лампи. Лише порожній металевий кожух і дзеркальний механізм." .
  "Ти вставляєш знайдену лінзу." .
  "Механізм повертається сам і спрямовує промінь у море." .
  "На воді з'являється відповідь: три короткі спалахи." .
  1 "knows_signal" !
  "Зняти лінзу" CHOICE
  "Залишити механізм працювати" CHOICE
  WAIT_CHOICE
  1 = IF
    GOTO: lighthouse_back
  ELSE
    GOTO: signal_finale
  THEN
;SCENE

SCENE: lighthouse_bottom
  CLS
  "Підземна кімната" LOCATION:
  "Нижні двері ведуть у круглу бетонну кімнату." .
  "У центрі стоїть апарат із котушкою, акумулятором і старим радіопередавачем." .
  "На стіні напис: S-17." .
  1 "opened_lighthouse" !
  1 "knows_tunnel" !
  "Увімкнути апарат" CHOICE
  "Оглянути документи" CHOICE
  "Зачинити двері" CHOICE
  WAIT_CHOICE
  dup 1 = IF drop GOTO: machine THEN
  dup 2 = IF drop GOTO: documents THEN
  drop GOTO: lighthouse
;SCENE

SCENE: lighthouse_back
  CLS
  "Під фундаментом" LOCATION:
  "За маяком є вузький прохід до моря. На камені — свіжі сліди чобіт." .
  "Хтось був тут зовсім недавно." .
  "Йти за слідами" CHOICE
  "Повернутися до входу" CHOICE
  WAIT_CHOICE
  1 = IF
    GOTO: cave
  ELSE
    GOTO: lighthouse
  THEN
;SCENE

SCENE: machine
  CLS
  "Підземна кімната" LOCATION:
  "Ти вмикаєш апарат." .
  3000 DELAY
  "Глибоко під водою щось відповідає металевим гулом." .
  "Потім із передавача чути голос: «S-17 активовано. Хто ти?»" .
  1 "knows_signal" !
  "Відповісти чесно" CHOICE
  "Відповісти кодом S-17" CHOICE
  "Вимкнути апарат" CHOICE
  WAIT_CHOICE
  dup 1 = IF drop GOTO: machine_truth THEN
  dup 2 = IF drop GOTO: machine_code THEN
  drop GOTO: lighthouse
;SCENE

SCENE: documents
  CLS
  "Підземна кімната" LOCATION:
  "Документи розповідають правду." .
  "Воронін створив систему навігаційних маяків, здатну передавати сигнали через воду." .
  "Він сховав головний передавач, коли зрозумів, що його хочуть використати для військових цілей." .
  "У документах є ім'я людини, яка допомагала йому: Марко Левицький." .
  1 "found_log" !
  1 "knows_signal" !
  "Повернутися" CHOICE
  "Взяти документи" CHOICE
  WAIT_CHOICE
  1 = IF
    GOTO: lighthouse
  ELSE
    1 "logbook" ITEM+
    GOTO: lighthouse
  THEN
;SCENE

SCENE: cave
  CLS
  "Морська печера" LOCATION:
  "У печері горить ліхтар." .
  "Поруч стоїть старий човен і лежить ящик із гасом." .
  "На стіні — схема сигналів S-17." .
  "Тут хтось живе." .
  "Покликати" CHOICE
  "Сховатися й чекати" CHOICE
  "Повернутися до маяка" CHOICE
  WAIT_CHOICE
  dup 1 = IF drop GOTO: stranger THEN
  dup 2 = IF drop GOTO: cave_wait THEN
  drop GOTO: lighthouse
;SCENE

SCENE: cave_wait
  CLS
  "Морська печера" LOCATION:
  "Через кілька хвилин у печеру заходить чоловік у мокрому пальті." .
  "Ти впізнаєш його з фотографії." .
  "Це Северин Воронін." .
  "Але він мав померти тридцять п'ять років тому." .
  "Він помічає тебе." .
  "Вийти зі схованки" CHOICE
  "Втекти" CHOICE
  WAIT_CHOICE
  1 = IF
    GOTO: voronin
  ELSE
    GOTO: escape
  THEN
;SCENE

SCENE: stranger
  CLS
  "Морська печера" LOCATION:
  "— Не кричи, — каже чоловік із темряви." .
  "Ти вмикаєш ліхтар і бачиш Северина Вороніна." .
  "Він старий, але живий." .
  "— Тепер ти розумієш, чому маяка не існує." .
  1 "talked_keeper" !
  "Попросити пояснення" CHOICE
  "Запитати про Марка" CHOICE
  "Піти" CHOICE
  WAIT_CHOICE
  dup 1 = IF drop GOTO: voronin THEN
  dup 2 = IF drop GOTO: voronin_marc THEN
  drop GOTO: escape
;SCENE

SCENE: voronin
  CLS
  "Морська печера" LOCATION:
  "Воронін розповідає, що інсценував свою смерть, щоб заховати систему." .
  "«Світанок» теж не затонув. Його екіпаж допоміг перевезти обладнання." .
  "— Але тепер система знову активується. Хтось використовує мої старі сигнали." .
  "Запитати, хто саме" CHOICE
  "Запропонувати допомогу" CHOICE
  "Повернутися до маяка" CHOICE
  WAIT_CHOICE
  dup 1 = IF drop GOTO: culprit THEN
  dup 2 = IF drop GOTO: alliance THEN
  drop GOTO: lighthouse
;SCENE

SCENE: voronin_marc
  CLS
  "Морська печера" LOCATION:
  "— Марко врятував мене, — каже Воронін. — Але потім вирішив, що має право вирішувати, хто може користуватися морем." .
  "— Він подає сигнали, щоб контролювати маршрут кораблів." .
  1 "accused" !
  "Зустрітися з Марком" CHOICE
  "Допомогти Вороніну" CHOICE
  WAIT_CHOICE
  1 = IF
    GOTO: confrontation
  ELSE
    GOTO: alliance
  THEN
;SCENE

SCENE: culprit
  CLS
  "Морська печера" LOCATION:
  "Воронін називає ім'я: Марко." .
  "Але в його голосі є сумнів." .
  "— Він не єдиний. У порту є людина, яка працює на тих, хто шукає систему." .
  "Ти розумієш: доказів усе ще недостатньо." .
  "Повернутися до порту" CHOICE
  "Залишитися з Вороніним" CHOICE
  WAIT_CHOICE
  1 = IF
    GOTO: harbor
  ELSE
    GOTO: alliance
  THEN
;SCENE

SCENE: alliance
  CLS
  "Морська печера" LOCATION:
  "Воронін показує тобі останній документ." .
  "У ньому — схема системи та спосіб її остаточного вимкнення." .
  "Для цього треба активувати три перемикачі в правильній послідовності: лінза, ключ, сигнал." .
  "Ти маєш усе необхідне, якщо зібрав докази." .
  1 "knows_signal" !
  1 "knows_tunnel" !
  "Піти до підземної кімнати" CHOICE
  "Повернутися в порт" CHOICE
  WAIT_CHOICE
  1 = IF
    GOTO: final_machine
  ELSE
    GOTO: harbor
  THEN
;SCENE

SCENE: confrontation
  CLS
  "Кабінет лікаря" LOCATION:
  "Марко зустрічає тебе спокійно." .
  "— Отже, ти знайшов його." .
  "Він не заперечує." .
  "— Воронін сховав систему, але я не дозволю, щоб вона знову потрапила в чужі руки." .
  "Змусити Марка вимкнути систему" CHOICE
  "Попросити Марка показати документи" CHOICE
  "Звинувачувати його" CHOICE
  WAIT_CHOICE
  dup 1 = IF drop GOTO: marc_shutdown THEN
  dup 2 = IF drop GOTO: marc_documents THEN
  drop GOTO: marc_accused
;SCENE

SCENE: marc_shutdown
  CLS
  "Кабінет лікаря" LOCATION:
  "Марко дістає старий передавач." .
  "— Я можу вимкнути її. Але тоді всі дізнаються, що вона існує." .
  "Ти просиш його зробити це." .
  1 "accused" !
  "Завершити історію" CHOICE
  "Попросити правду про «Світанок»" CHOICE
  WAIT_CHOICE
  1 = IF
    GOTO: ending_truth
  ELSE
    GOTO: doctor_ship
  THEN
;SCENE

SCENE: marc_documents
  CLS
  "Кабінет лікаря" LOCATION:
  "Марко показує документи." .
  "Стає ясно: він справді намагався захистити систему, але приховав частину правди." .
  "У документах є координати затонулого сховища." .
  1 "found_log" !
  1 "knows_tunnel" !
  "Попросити координати" CHOICE
  "Піти" CHOICE
  WAIT_CHOICE
  1 = IF
    GOTO: warehouse
  ELSE
    GOTO: harbor
  THEN
;SCENE

SCENE: marc_accused
  CLS
  "Кабінет лікаря" LOCATION:
  "Ти звинувачуєш Марка в підробці сигналів." .
  "Він не заперечує." .
  "— Я хотів зберегти систему. Але ти не розумієш, що станеться, якщо її знайдуть." .
  1 "accused" !
  "Забрати передавач" CHOICE
  "Піти до маяка" CHOICE
  WAIT_CHOICE
  1 = IF
    GOTO: stolen_transmitter
  ELSE
    GOTO: lighthouse
  THEN
;SCENE

SCENE: warehouse
  CLS
  "Підводне сховище" LOCATION:
  "Координати приводять тебе до старого бетонного резервуара біля затоки." .
  "Усередині — обладнання, журнали та коробка з емблемою S-17." .
  "Тут зберігали деталі системи після аварії «Світанку»." .
  1 "found_log" !
  1 "found_photo" !
  "Забрати документи" CHOICE
  "Повернутися" CHOICE
  WAIT_CHOICE
  1 = IF
    GOTO: final_check
  ELSE
    GOTO: harbor
  THEN
;SCENE

SCENE: night_boat
  CLS
  "Нічний човен" LOCATION:
  "Левко мовчки везе тебе до скелі." .
  "У темряві раптом спалахує світло." .
  "Три короткі спалахи. Пауза. Два спалахи." .
  "Левко розвертає човен." .
  "— Це попередження." .
  "Наполягти на продовженні" CHOICE
  "Повернутися" CHOICE
  WAIT_CHOICE
  1 = IF
    GOTO: lighthouse
  ELSE
    GOTO: harbor
  THEN
;SCENE

SCENE: machine_truth
  CLS
  "Підземна кімната" LOCATION:
  "— Я дослідник, — відповідаєш ти." .
  "У відповідь чути: «Тоді шукай не маяк. Шукай того, хто його запалює»." .
  "Передавач вимикається." .
  1 "knows_signal" !
  "Ти розумієш: справжня загадка — не система, а людина, яка керує нею." .
  "Повернутися в порт" CHOICE
  WAIT_CHOICE
  drop GOTO: final_check
;SCENE

SCENE: machine_code
  CLS
  "Підземна кімната" LOCATION:
  "Ти відповідаєш: S-17." .
  "Після довгої паузи голос каже: «Отже, Воронін тобі довірився»." .
  "Система відкриває прихований контейнер." .
  "Усередині — оригінальний журнал 1986 року." .
  1 "found_log" !
  1 "found_photo" !
  1 "logbook" ITEM+
  1 "photo" ITEM+
  "Повернутися" CHOICE
  WAIT_CHOICE
  drop GOTO: final_check
;SCENE

SCENE: stolen_transmitter
  CLS
  "Стара пристань" LOCATION:
  "Ти забираєш передавач Марка." .
  "Але щойно ти виходиш, у морі спалахує світло." .
  "Три короткі сигнали. Потім два." .
  "Хтось знає, що передавач у тебе." .
  "Повернутися до маяка" CHOICE
  "Сховати передавач" CHOICE
  WAIT_CHOICE
  1 = IF
    GOTO: final_check
  ELSE
    GOTO: escape
  THEN
;SCENE

SCENE: escape
  CLS
  "Нічний порт" LOCATION:
  "Ти залишаєш скелю." .
  "За спиною маяк спалахує востаннє." .
  "Наступного ранку в газеті пишуть про загадкову навігаційну аварію." .
  "Ти так і не дізнався, хто керував сигналом." .
  "Сигнал лишився загадкою. Справу не розкрито." DEFEAT
;SCENE

SCENE: final_check
  CLS
  "Остання ніч" LOCATION:
  "У тебе є лише кілька годин до приходу порома." .
  "Потрібно вирішити, що робити із системою S-17." .
  "Перевірити докази" CHOICE
  "Знищити систему" CHOICE
  "Залишити систему Вороніну" CHOICE
  "Передати все владі" CHOICE
  WAIT_CHOICE
  dup 1 = IF drop GOTO: evidence THEN
  dup 2 = IF drop GOTO: destroy THEN
  dup 3 = IF drop GOTO: protect THEN
  drop GOTO: authorities
;SCENE

SCENE: evidence
  CLS
  "Кімната доказів" LOCATION:
  "Ти розкладаєш знайдені речі." .
  "Лінза, ключ, журнал і фотографія складаються в одну картину." .
  "Система існувала. Воронін живий. Марко приховував правду." .
  "Але мотиви кожного досі неоднозначні." .
  has_all_clues IF
    "Ти зібрав ключові докази." .
    1 "saved_before_finale" !
    checkpoint
    GOTO: true_finale
  ELSE
    "Тобі бракує частини доказів." .
    "Повернися до порту й пошукай ще." .
    "Повернутися" CHOICE
    WAIT_CHOICE
    drop GOTO: harbor
  THEN
;SCENE

SCENE: destroy
  CLS
  "Підземна кімната" LOCATION:
  "Ти вмикаєш аварійний режим." .
  "Система починає перегріватися." .
  "Воронін кричить із коридору: «Не роби цього!»" .
  "Ти витягуєш головний кабель." .
  "Передавач гасне назавжди." .
  "Маяк, якого не було, більше ніколи не засвітиться." .
  "Систему знищено. Море знову мовчить." VICTORY
;SCENE

SCENE: protect
  CLS
  "Морська печера" LOCATION:
  "Ти вирішуєш залишити систему Вороніну." .
  "Він обіцяє ніколи більше не використовувати її для навігації кораблів." .
  "Але ти розумієш: тепер секрет належить не одній людині." .
  "Через рік у затоці знову бачать три спалахи." .
  "Таємницю збережено — але море пам'ятає." VICTORY
;SCENE

SCENE: authorities
  CLS
  "Портова адміністрація" LOCATION:
  "Ти передаєш докази владі." .
  "Наступного дня скелю оточують військові." .
  "Систему вилучають." .
  "Офіційна версія називає її старим навігаційним прототипом." .
  "Але всі документи про «Світанок» зникають із архіву." .
  "Ти виграв справу, але не дізнався всієї правди." .
  "Влада забрала систему. Повна правда зникла з архівів." FINISH
;SCENE

SCENE: true_finale
  CLS
  "Маяк, якого не було" LOCATION:
  "Усі докази перед тобою." .
  "Тепер ти можеш скласти останню частину загадки." .
  "Система була створена Вороніним, схована після 1986 року, а Марко підтримував її роботу." .
  "Але людина в печері — Воронін — доводить, що офіційна історія була брехнею." .
  "Що ти зробиш?" .
  "Опублікувати всю правду" CHOICE
  "Знищити систему" CHOICE
  "Зберегти таємницю" CHOICE
  WAIT_CHOICE
  dup 1 = IF drop GOTO: ending_truth THEN
  dup 2 = IF drop GOTO: destroy THEN
  drop GOTO: protect
;SCENE

SCENE: ending_truth
  CLS
  "Світанок" LOCATION:
  "Ти публікуєш журнал, фотографії та свідчення." .
  "Ім'я Вороніна знову з'являється в газетах." .
  "Влада заперечує все, але архіви вже неможливо приховати." .
  "Марко зникає до початку розслідування." .
  "Через місяць у порту знаходять уламки старого корабля «Світанок»." .
  "На його корпусі вигравірувано: S-17." .
  "Ти розкрив таємницю, але тепер знаєш: вона була лише частиною більшої історії." .
  "Правду опубліковано. «Світанок» знайдено." VICTORY
;SCENE

SCENE: final_machine
  CLS
  "Серце маяка" LOCATION:
  "Ти повертаєшся до підземної кімнати разом із Вороніним." .
  "Він показує три перемикачі." .
  "— Лінза. Ключ. Сигнал. Саме так система розуміє, що ми не вороги." .
  "Ти активуєш їх послідовно." .
  1000 DELAY
  "Гул припиняється." .
  "У морі спалахує останній сигнал — один довгий промінь." .
  "Система переходить у сплячий режим." .
  "Але перед вимкненням передавач передає координати ще одного об'єкта далеко в морі." .
  "Воронін дивиться на тебе." .
  "— Оце ми вже точно не повинні були знаходити." .
  "Продовжити дослідження" CHOICE
  "Знищити систему" CHOICE
  WAIT_CHOICE
  1 = IF
    GOTO: deep_mystery
  ELSE
    GOTO: destroy
  THEN
;SCENE

SCENE: deep_mystery
  CLS
  "Нові координати" LOCATION:
  "Координати ведуть не до затонулого корабля." .
  "Вони ведуть до острова, якого немає на жодній карті." .
  "Воронін усміхається вперше." .
  "— Схоже, наш маяк був не першим." .
  "Ти дивишся на море й розумієш, що ця історія тільки починається." .
  "Систему зупинено — і відкрито шлях до нових координат." VICTORY
;SCENE

