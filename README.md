emGUI v1.0.0
=====

C GUI Library for Arduino. 

Some photos 
![Main menu](https://github.com/libEmGUI/emGUI-arduino/blob/master/Pictures/fa314fcf-e02c-4a30-bb53-9c6628b62733.jpg?raw=true)
![Another photo](https://github.com/libEmGUI/emGUI-arduino/blob/master/Pictures/af27046c-2f33-44af-a220-e1cf2456dcb5.jpg?raw=true)


This lib could work not only with Arduino, so we made an example with visual studio (will be submited soon)
1[Example on PC: main menu](https://github.com/libEmGUI/emGUI-arduino/blob/master/Pictures/main%20menu.PNG?raw=true)
1[Example on PC: modal dialog](https://github.com/libEmGUI/emGUI-arduino/blob/master/Pictures/modal%20dialog.PNG?raw=true)
1[Example on PC: Big colorful image](https://github.com/libEmGUI/emGUI-arduino/blob/master/Pictures/Image.PNG?raw=true)
1[Example on PC: Multiline textbox with left allign](https://github.com/libEmGUI/emGUI-arduino/blob/master/Pictures/Text%20box.PNG?raw=true)

// Russian 

Библиотека делает ГУЙ. Главная сущность в ней - виджет (Widget). 
Это базовая струткура, отражающая логическую единицу интерфейса, содержащая такую информацию
как расположение, размер, обработчик дейсвтия при нажатии. Она служит предком для
всех остальных сущностей: Window, Label, Button, StatusBar и т.д. 
(Вы можете описать свои сущности, если в этом есть необходимость)

Виджеты вкладываются друг в друга, образуя иерархию. Главным виджетом выступает 
едиснвтенный экземпляр структуры типа Interface, который заполняется окнами (Windows)
Окна создаются в момент вызыва функции `pxWindowCreate(eWindow)`. Функция принимает в качестве 
аргумента название окна (типа enum). Далее окно становится невидимм и вызывается функцией 
`vInterfaceOpenWindow(eWindow)`

В библиотеке реализован стек окон. В разумных пределах можно открывать сколько
угодно окон и не бояться за порядок их откртия/закрытя

Вызов функции

`bool bInterfaceCheckTouchScreenEvent(xTouchEvent *pxTouchScreenEv);` 

Приведет к вызову обработчика, который вы можете назначить на любой виджет. 

В библиотеке реализовано закрытие активного окна при нажатии на крестик в статусбаре. 
Обработчик может задаваться как в конструкторе, так и отдельной функцией (подробнее смотрите в исходном коде к библиотеке)

Getting started

Для начала работы необходимо определить виртуальные функции отрисовки примитовов:
 - Прямоугольник 
 - Вертикальная линия
 - Горизонтальная линия 
 - Символ(в библиотеке пристуствуют два шрифта)
 - Изображение 
 
В примере вы можете ознакомиться с тем, как определить эти функции
 
Вы можете также переопределить формат хранения изображений. В этом случае 
вам необходимо (!) переопределить изображения, которые используются в 
библиотеке.  А так же переопределить методы для нахождения высоты и 
ширины изображения (см файл Draw.h)

Файл opts.h опредляет значения макросов по-умолчанию (НЕ МЕНЯЙТЕ ЭТОТ ФАЙЛ!)
Для определения своих значений используйте файл emGUI_opts.h

В этом файле можно определить основные константы (таткие как размер экрана)
А так же переопределить цвета, шрифты, изображения, типы изображений  


