@echo off
xcopy /Y NordicSemiconductor.nRF5_SDK_Drivers_14.2.0.json ..\..\..\build\debug\NordicSemiconductor.nRF5_SDK_Drivers_14.2.0\
xcopy /Y NordicSemiconductor.nRF5_SDK_Libraries_14.2.0.json ..\..\..\build\debug\NordicSemiconductor.nRF5_SDK_Libraries_14.2.0\
..\..\utils\7z.exe a -tzip ..\..\..\build\debug\_NordicSemiconductor.nRF5_SDK_Drivers_14.2.0.jpack -r ..\..\..\build\debug\NordicSemiconductor.nRF5_SDK_Drivers_14.2.0\*
..\..\utils\7z.exe a -tzip ..\..\..\build\debug\_NordicSemiconductor.nRF5_SDK_Libraries_14.2.0.jpack -r ..\..\..\build\debug\NordicSemiconductor.nRF5_SDK_Libraries_14.2.0\*
pause