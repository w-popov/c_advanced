##### Склонировать каталог с ДЗ
* Создать где-то каталог, зайти в него, открыть в нем терминал.
* Ввести в терминале: 
```Bash
git clone --no-checkout https://github.com/w-popov/c_advanced.git

cd c_advanced/
git sparse-checkout set SnakeGame
git checkout
cd SnakeGame/
```
## Игра змейка
Игра змейка многопоточная версия. Графический интерфейс: библиотека ncurces/PDCurses.

<img width="1510" height="872" alt="zipgameplay" src="https://github.com/user-attachments/assets/3091c308-b421-445e-ac5f-fc66a78dbdec" />
