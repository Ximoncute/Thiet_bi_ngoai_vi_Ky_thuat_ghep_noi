@echo off
:: Di chuyển vào thư mục backend để chạy server và api
cd /d "%~dp0backend"

:: Chạy server.js trong một cửa sổ mới
echo Dang khoi dong server.js...
start "MQTT Server" node server.js

:: Chạy api.js trong một cửa sổ mới
echo Dang khoi dong api.js...
start "Express API" node api.js

:: Mở file index.html bằng trình duyệt mặc định
echo Dang mo giao dien nguoi dung...
start "" "%~dp0arduino_ide\code_phan_mem\HTML\index.html"

echo Tat ca cac dich vu da duoc khoi dong!
pause
