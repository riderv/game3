TODO LIST:

Задача 1 
Сделать базовую отрисовку карты, генерацию, сохранение, загрузку
1) Создать таки тест для SQLite 
2) Создать тест генерации, сохранения и загрузки карты.
3) GenerateMap чтоб наконец запоминал хотя бы путь.
+3.1) Или кнопку рядом OpenFolderDialog
+4) скролинг не правильно работает.
5) отрисовывать раз в 1/30 секунды
+6) Поправить Main Menu / Load Map
7) Сделать в классах нормальные конструкторы копирования или ноно-копибальными!!!
8) Выход из редактора в главное меню.
9) При загрузке если файл битый переход в корневое меню.


Задача 2
1)Сделать анимированного перса, чтобы можно было побегать по карте 
	1.1 нарисовать idle 4стороны, ходьба


Доп Задачи
Пути файлов сделать относительно полного пути к ехе.
Сделать отрисовку в 30фпс.

ПО РЕДАКТОРУ КАРТЫ:
1) меню с (сохранить, загрузить)
2) выбор тайла-кисти и отображение под мышкой
3) редактирование

Главное меню - играть.
Поместить персонажа на карту в точку спауна отмеченной в редакторе или в 0;0.
Ходьба (перемещать тайл перса только по земле).
Анимация перса (стоит, идёт).

========================================================================
    WIN32 APPLICATION : single Project Overview
========================================================================

AppWizard has created this single application for you.

This file contains a summary of what you will find in each of the files that
make up your single application.


single.vcxproj
    This is the main project file for VC++ projects generated using an Application Wizard.
    It contains information about the version of Visual C++ that generated the file, and
    information about the platforms, configurations, and project features selected with the
    Application Wizard.

single.vcxproj.filters
    This is the filters file for VC++ projects generated using an Application Wizard. 
    It contains information about the association between the files in your project 
    and the filters. This association is used in the IDE to show grouping of files with
    similar extensions under a specific node (for e.g. ".cpp" files are associated with the
    "Source Files" filter).

single.cpp
    This is the main application source file.

/////////////////////////////////////////////////////////////////////////////
AppWizard has created the following resources:

single.rc
    This is a listing of all of the Microsoft Windows resources that the
    program uses.  It includes the icons, bitmaps, and cursors that are stored
    in the RES subdirectory.  This file can be directly edited in Microsoft
    Visual C++.

Resource.h
    This is the standard header file, which defines new resource IDs.
    Microsoft Visual C++ reads and updates this file.

single.ico
    This is an icon file, which is used as the application's icon (32x32).
    This icon is included by the main resource file single.rc.

small.ico
    This is an icon file, which contains a smaller version (16x16)
    of the application's icon. This icon is included by the main resource
    file single.rc.

/////////////////////////////////////////////////////////////////////////////
Other standard files:

StdAfx.h, StdAfx.cpp
    These files are used to build a precompiled header (PCH) file
    named single.pch and a precompiled types file named StdAfx.obj.

/////////////////////////////////////////////////////////////////////////////
Other notes:

AppWizard uses "TODO:" comments to indicate parts of the source code you
should add to or customize.

/////////////////////////////////////////////////////////////////////////////
