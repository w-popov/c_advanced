##### Склонировать каталог с ДЗ
* Создать где-то каталог, зайти в него, открыть в нем терминал.
* Ввести в терминале: 
```Bash
git clone --no-checkout https://github.com/w-popov/c_advanced.git

cd c_advanced/
git sparse-checkout set SnakeGame
git checkout
cd SnakeGame/