## Task I-1: Fildes

1. 輸入 cat fildes.c >&1 將fildes.c輸出到螢幕上來
2. 將會看到 fildes.c的程式碼當中 fd = atoi(argv[1]) - OxCAFE
3. 而0xCAFE 等於51966，因此你輸入的數字要大於等於51966才行
4. 而```strncmp()```所比較的單字為```YOUSHALLNOTPASS```
5. 因此先輸入```$ ./fildes 51966```來成功執行 fildes
6. 成功進入到 fildes 後，再輸入```YOUSHALLNOTPASS```
7. 成功獲取 flag!!!

參考資料

1. [File descriptor與I/O導向](https://blog.xuite.net/tzeng015/twblog/113272117-File+descriptor%E8%88%87I%2FO%E5%B0%8E%E5%90%91)
2. [read(2) - Linux man page](https://linux.die.net/man/2/read)
3. [Number info: 0xcafe](https://numbermonk.com/hexadecimal/51966/en)

## Task I-2: You-should-read-manual

1. 先用 vim 來看 ```you-should-read-manual.c```的程式碼
2. 會發現他是用rand()來生成的
3. rand()生成的變數若沒有用srand()來設定亂數種子的話會相同，所以可以去/tmp資料夾生成看看
4. 在/tmp的資料夾裡面用vim 生成一個test.c檔案(可以複製原本的來改)
5. 再test.c裡面新增 int test = 0x0B16DEA1^random 然後加上printf("%d\n");
6. 輸入```gcc test.c -o ans```來重新編譯後，執行ans獲取答案
7. 得到答案後，再去執行```$ ./you-should-read-manual```
8. 輸入剛剛得到的答案，成功獲取 flag!!!

參考資料

1. [GCC 下載安裝與基本使用教學](https://kaiching.org/pydoing/c/gcc.html)
2. [你的程式夠「亂」嗎？](https://www.ithome.com.tw/voice/110007)
3. [rand(3) - Linux man page](https://linux.die.net/man/3/rand)
4. [C語言中的RAND 函數產生的數值不變](https://zhidao.baidu.com/question/2053472265334017667.html)

## Task I-3: Nasty-rules

   一開始先使用```vim nasty.c```將程式碼輸出來，注意```if (fd=open("password", O_RDONLY, 0400) < 0)```這段程式碼，會發現一件事情，```<```的優先度會比```=```還高，所以會是```fd = (open("password", O_RDONLY, 0400) < 0)```這樣，因此 fd 的值只會等於 0 或 1 ，又等於 1 的時候程式不會進行下去，因此fd = 0。
   
   而當fd = 0 的時候，```read(fd, password_buf1, PASSWORD_LENGTH)```就會出現變化，因為fd = 0會被視作standard input，因此會要你輸入東西，而你輸入的東西就會被塞進password_buf1，所以你只要在這時候輸入你想要的password(小於15字元)，然後再輸入password_buf2的時候，輸入```password_buf1 ^ 0xA```的結果進去，最後再比對的時候，答案就會是相同的了，因此就會獲得flag!!!
   
   單純紀錄一下```snprintf(format, sizeof(format), "%%%ds", PASSWORD_LENGTH);```，裡面的```%%%ds```會等於```%15s```，前兩個% 得出%，再來的%d 對應到PASSWORD_LENGTH。
   
參考資料

1.  [read (C System Call)](http://codewiki.wikidot.com/c:system-calls:read)
2. [C和C++運算子](https://zh.wikipedia.org/wiki/C%E5%92%8CC%2B%2B%E9%81%8B%E7%AE%97%E5%AD%90)
3. [ASCII](https://zh.wikipedia.org/wiki/ASCII)
4. [C Operator Precedence](https://en.cppreference.com/w/c/language/operator_precedence)
5. [用 snprintf / asprintf 取代不安全的 sprintf](https://kheresy.wordpress.com/2010/01/28/%E7%94%A8-snprintf-asprintf-%E5%8F%96%E4%BB%A3%E4%B8%8D%E5%AE%89%E5%85%A8%E7%9A%84-sprintf/)
6. [C formatting printf](http://www.c4learn.com/c-programming/c-formatting-printf/)
7. [open(3) - Linux man page](https://linux.die.net/man/3/open)
8. [read(2) - Linux man page](https://linux.die.net/man/2/read)
9. [Linux系統調用之open(), close()](http://joe.is-programmer.com/posts/17463.html)

## Task II-1: Lucky-pot

這題真的蠻無聊的，一樣先把code弄出來看，然後你會看到
```        
for (i = 0; i < LUCKY_POT_LENGTH; ++i) {
    lucky[i] = (lucky[i] % 49) + 1;  // 1 ~ 49
}
```
首先lucky是個char，而你給他int的數值，他會用ASCII做轉換，因此實際能輸入的ascii是從32開始，所以你只需要輸入32~49的ascii對應的字元就行了

再來繼續把code往下看，你會看到
```
for (i = 0; i < LUCKY_POT_LENGTH; ++i) {
    printf("%d ", lucky[i]);
}
```
會發現它輸出的數字是ascii，因為她是char卻用%d來做輸出
再來則是
```
for (i = 0; i < LUCKY_POT_LENGTH; ++i) {
    for (j = 0; j < LUCKY_POT_LENGTH; ++j) {
        if (lucky[i] == submit[j]) {
            match++;
        }
    }
}
```
你會發現，他只要有比對到就會把match的分數往上加，而不會發現是否已經比對過了，因此你只需要輸入6個相同的字元，然後對中其中一個lucky number，你就會獲得6分了，因此只需要在32~49的ascii對應的字元中選出一個然後輸入6次，開始刷，刷到中為止，就可以獲得flag!!!

參考資料

1.  [ASCII](https://zh.wikipedia.org/wiki/ASCII)

## Task II-2: Flagbin

一開始先從 [flagbin](http://nctuics.louie.lu:20008/flagbin) 下載flagbin的檔案，然後使用```chmod +x flagbin```來提高權限執行，執行後會看到```I'll malloc() and strncpy the flag to here```不過不太重要就是了。

之後使用 ```upx -d flagbin```來將flagbin解壓縮，再輸入```gdb flagbin```進入gdb的模式裡面。

在gdb裡面可以輸入```disassemble main```來看memory的使用以及相對的東西，會出現![flagbin_pic1](/flagbin_pic1.png)

參考資料

1.  [GDB Installation on Mac OS X](https://www.ics.uci.edu/~pattis/common/handouts/macmingweclipse/allexperimental/mac-gdb-install.html)
2. [How can I get information about a binary file that won't execute?](https://serverfault.com/questions/730922/how-can-i-get-information-about-a-binary-file-that-wont-execute)

## Task II-3: Random-pass-auth