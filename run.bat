@echo off

cd /d "%~dp0backend"

echo Dang khoi dong server.js...
start "MQTT Server" node server.js

echo Dang khoi dong api.js...
start "Express API" node api.js

:: đợi backend khởi động
echo Dang doi backend khoi dong...
timeout /t 3 > nul

:: mở web thông qua server thay vì (file://)
echo Dang mo giao dien nguoi dung...
start http://localhost:3000

echo Tat ca cac dich vu da duoc khoi dong!
pause