# Project 2

## Task1 步驟
***
1. 先安裝好助教提供的ubuntu image檔案

2. 在ubuntu創立一個新的使用者，帳號密碼都設定為victim

3. 然後下載助教提供的 worm_sample 的檔案

4. 用sudo ./worm_sample來執行

	1. 若無法執行的話，先執行```sudo chmod +x worm_sample a b c```，來調高檔案的權限。

5. 使用```sudo apt install htop```安裝htop

6. 會發現有個```Flood_Attack```一直在跑，將worm_sample關閉後，重新開機登入victim，再開啟htop來看，會發現```Flood_Attack```還是在執行。

7. ```Flood_Attack```所在的資料夾為```/.etc/.module/```

8. 使用```sudo apt install vim```安裝vim

9. 在根目錄使用vim 打開```/etc/crontab```會發現裡面有指令，讓```Flood_Attack```開機後就開始執行，將該指令刪除後，重新開機後，便不會執行```Flood_Attack```了。

10. 不太會用```vi```開啟檔案，所以只知道用```vi```開啟後，不能編輯，若亂輸入東西後，可以先按```ESC```再輸入```:qa!```，這樣就不會存檔，並且順利離開檔案。
