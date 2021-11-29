_Read this in other languages:_ [Русский](README.ru.md) [English](README.md)

:ru: Родительский проект: Goxel
=====

Версия 0.10.7

Автор кода Guillaume Chereau <guillaume@noctua-software.com>

[![Build Status](
    https://travis-ci.org/guillaumechereau/goxel.svg?branch=master)](
    https://travis-ci.org/guillaumechereau/goxel)
[![DebianBadge](https://badges.debian.net/badges/debian/unstable/goxel/version.svg)](https://packages.debian.org/unstable/goxel)

Официальный веб-сайт: https://goxel.xyz

О проекте
-----

Вы можете использовать goxel для воксельной графики (3D изображения на основе кубов - объёмных пикселей). Подходит для Linux, BSD, Windows и macOS.


Загрузка
--------

Актуальный релиз можно скачать [здесь](
https://github.com/guillaumechereau/goxel/releases/latest).

Goxel также доступен для [iOS](
https://itunes.apple.com/us/app/goxel-3d-voxel-editor/id1259097826) и
[Android](
https://play.google.com/store/apps/details?id=com.noctuasoftware.goxel).


![goxel screenshot 0](https://goxel.xyz/gallery/thibault-fisherman-house.jpg)
Домик рыбака, арт на Goxel от
[Thibault Simar](https://www.artstation.com/exm)


Лицензия
-------

Goxel выпускается под лицензией GNU GPL3.


Возможности
--------

- 24-битные RGB цвета.
- Безлимитный размер сцены.
- Безлимитный бумер отмен.
- 3D-слои.
- Marching Cube рендеринг.
- Процедурный рендеринг.
- Экспорт в форматы obj, pyl, png, magica voxel, qubicle.
- Трассировка лучей.
- Экспорт в формат vxl, что делает программу пригодной для создания карт для Ace of Spades.


Использование
-----

- ЛКМ: применить выбранный инструмент.
- СКМ: повернуть сцену.
- ПКМ: переместить сцену.
- Левая/правая стрелки: повернуть сцену.
- Колёсико мыши: приблизить/отдалить.


Компиляция
--------

Проект использует scons. Вы можете компилировать для дебага с 'scons' и для релиза с 'scons mode=release'. На Windows в данный момент возможна компиляция с помощью
[msys2](https://www.msys2.org/), в качестве альтернативы можно использовать прекомпилированный пакет [goxel](https://packages.msys2.org/base/mingw-w64-goxel) напрямую.
Код написан на C99 с использованием некоторых расширений gnu, так что он не компилируется с помощью msvc.

# Linux/BSD

Установите зависимости вашим менеджером пакетов.  На Debian/Ubuntu:

    - scons
    - pkg-config
    - libglfw3-dev
    - libgtk-3-dev

Затем для компиляции введите команду:

    make release

# Windows

Вам понадобится установить msys2 mingw, а также следующие пакеты:

    pacman -S mingw-w64-x86_64-gcc
    pacman -S mingw-w64-x86_64-glfw
    pacman -S mingw-w64-x86_64-libtre-git
    pacman -S scons
    pacman -S make

Затем для компиляции введите:

    make release


