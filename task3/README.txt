task3
Задача:
Описать программу, которая, пока не получен конец файла, выполняет в цикле
следующие действия:
1) cчитывает из stdin очередную строку,
2) преобразует ее в последовательность (список) слов,
3) выводит в stdout полученную последовательность слов в исходном порядке,
4) выводит слова последовательности в лексикографическом порядке.
При выводе в первой строке напечатать длину списка, в каждой последующей –
очередной элемент списка.

Функции

void null_list(char **mass, int *sizelist, int *curlist)
-инициализирует список mass
-обнуляет размер списка sizelist
-обнуляет счетчик текущего свободного элемента curlist

void clearlist(char **mass, int *sizelist, int *curlist)
-Очищает список mass
-Обнуляет счетчики sizelist и  curlist

void termlist(char ***mass, int *sizelist, int *curlist)
-Корректирует размер списка mass
-Корректирует счетчики списка sizelist и curlist

void printlist(char ***mass, int *sizelist)
-Печатает список mass

void sortlist(char ***mass, int *sizelist)
-Сортирует в лексикографическом порядке список mass

void null_buf(char *buf, int *sizebuf, int *curbuf)
-Очищает буфер buf

void addsym(int c, char **buf, int *sizebuf, int *curbuf)
-Добавляет символ с в буфер buf
-Меняет счетчики буфера sizebuf и curbuf

int check1(int c)
-Проверяет символ с на то специальный он или нет

int check2(int c)
-Проверяет символ с на повторяющиеся специальные символы

int symset(int c)
-проверяет символ с на непечатающиеся символы

void addword(char **buf, int *sizebuf, int *curbuf, char ***mass, int *sizelist, int *curlist)
-Добавляет в список mass, слово из buf
-Обновляет счетчики буфера и списка

int upd(int *curinp, char *inp)
-Обновляет буфер ввода inp
-Обновляет счетчик ввода curinp

int getsym(int *curinp, char *inp)
-Возвращает считанный символ из буфера inp
-Обновляет счетчик ввода curinp
