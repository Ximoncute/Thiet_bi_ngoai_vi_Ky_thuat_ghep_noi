@echo off

cd /d "%~dp0backend"

echo Dang khoi dong server.js...
start "MQTT Server" node server.js

echo Dang khoi dong api.js...
start "Express API" node api.js

:: đợi backend khởi động
echo Dang doi backend khoi dong...
timeout /t 3 > nul

:: mở web như cũ (file://)
echo Dang mo giao dien nguoi dung...
start "" "%~dp0arduino_ide\code_phan_mem\HTML\index.html"

echo Tat ca cac dich vu da duoc khoi dong!
pause