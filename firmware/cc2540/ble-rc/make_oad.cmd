@echo off

del *.bin

for /F "usebackq tokens=1,2 delims==" %%i in (`wmic os get LocalDateTime /VALUE 2^>NUL`) do if '.%%i.'=='.LocalDateTime.' set ldt=%%j
set ldt=%ldt:~0,4%-%ldt:~4,2%-%ldt:~6,2%_%ldt:~8,2%.%ldt:~10,2%.%ldt:~12,6%
echo Local date is [%ldt%]

..\common\cc2540\cc254x_sim2bin.exe .\CC2541DB\CC2541-OAD-ImgA\Exe\ble-rc.sim ble-rc-a-%ldt%.bin
..\common\cc2540\cc254x_sim2bin.exe .\CC2541DB\CC2541-OAD-ImgB\Exe\ble-rc.sim ble-rc-b-%ldt%.bin

..\common\cc2540\cc254x_sim2bin.exe .\CC2541DB\Simple-OAD-ImgA\Exe\ble-rc.sim rc-simple-a-%ldt%.bin
..\common\cc2540\cc254x_sim2bin.exe .\CC2541DB\Simple-OAD-ImgB\Exe\ble-rc.sim rc-simple-b-%ldt%.bin

..\common\cc2540\cc254x_sim2bin.exe .\CC2541DB\MiniZ-OAD-ImgA\Exe\ble-rc.sim mini-z-a-%ldt%.bin
..\common\cc2540\cc254x_sim2bin.exe .\CC2541DB\MiniZ-OAD-ImgB\Exe\ble-rc.sim mini-z-b-%ldt%.bin

..\common\cc2540\cc254x_sim2bin.exe .\CC2541DB\MiniZ-BLDC-ImgA\Exe\ble-rc.sim mini-z-bldc-a-%ldt%.bin
..\common\cc2540\cc254x_sim2bin.exe .\CC2541DB\MiniZ-BLDC-ImgB\Exe\ble-rc.sim mini-z-bldc-b-%ldt%.bin

..\common\cc2540\cc254x_sim2bin.exe .\CC2541DB\dNano-OAD-ImgA\Exe\ble-rc.sim dnano-a-%ldt%.bin
..\common\cc2540\cc254x_sim2bin.exe .\CC2541DB\dNano-OAD-ImgB\Exe\ble-rc.sim dnano-b-%ldt%.bin

..\common\cc2540\cc254x_sim2bin.exe .\CC2541DB\EscServo-OAD-ImgA\Exe\ble-rc.sim esc-servo-a-%ldt%.bin
..\common\cc2540\cc254x_sim2bin.exe .\CC2541DB\EscServo-OAD-ImgB\Exe\ble-rc.sim esc-servo-b-%ldt%.bin
