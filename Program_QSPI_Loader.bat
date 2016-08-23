@echo off
rem  This batch file will load the QSPI boot loader program.
rem  Please change the JLINK.EXE path below if it is different in your system. 
echo LOAD BOOT LOADER INTO QSPI FLASH (IC26)
echo ========================================================================
echo ***    Remember to update the JLink.exe path specified               ***
echo ***    in this batch file if using a different version than          ***
echo ***    that installed by the RSK+RZA1H_E2 installer.                 ***
echo ***    This batch file was tested and works with Ver 4.90e           ***
echo ***    of the JLink.exe                                              ***
echo ***                                                                  ***
echo ***    Please refer to the:                                          ***
echo ***    RSK+RZA1H QSPI Boot Loader Application Note (R01AN2141EG0100) *** 
echo ***    for more information.                                         ***
echo ========================================================================
echo .
echo The Load QSPI Boot Loader option will only load the QSPI boot loader  
echo program into the QSPI device (IC26). 
echo .
echo Please make sure you have read the Description.txt file in the doc 
echo folder before selecting an option.
echo ------------------------------------------------------------------------
echo .
:OPTIONS
echo OPTIONS
echo 1 = Load QSPI Boot Loader (HardwareDebug).
echo 2 = Load QSPI Boot Loader (Release).
echo 3 = Exit
SET /P REPLY=Choose options (1, 2 or 3):   
if "%REPLY%"== "1" (goto PROGRAM)
if "%REPLY%"== "2" (goto PROGRAM)
if "%REPLY%"== "3" (goto END)  
echo ERROR: Please select 1, 2 or 3 only.
goto OPTIONS
:PROGRAM
echo Remove power (5V) to the board before continuing. 
echo Set SW6 as instructed below:
echo SW6-1 OFF, SW6-2 ON, SW6-3 OFF, SW6-4 ON, SW6-5 ON, SW6-6 ON
echo Reconnect power (5V) to the board before continuing. 
pause
echo ENSURE YOU HAVE RECONFIGURED THE SW6 SWITCHES ON THE TARGET
echo - IT IS IMPORTANT -
pause
if not exist "C:\Program Files\SEGGER\JLink_V490e\JLink.exe" goto 64BIT_HOST
echo ------------------------------------------------------------------------
if "%REPLY%"== "1" (goto HARDWARE_DEBUG_32)  else (goto RELEASE_32)
:HARDWARE_DEBUG_32
echo Downloading QSPI boot loader (HardwareDebug) and user application.
"C:\Program Files\SEGGER\JLink_V490e\JLink.exe" -speed 12000 -if JTAG -device R7S721001 -CommanderScript Program_QSPI_Loader_HDB.Command
goto OUT
if "%REPLY%"== "2" (goto RELEASE_32)
:RELEASE_32
echo Downloading QSPI boot loader (Release) and user application.
"C:\Program Files\SEGGER\JLink_V490e\JLink.exe" -speed 12000 -if JTAG -device R7S721001 -CommanderScript Program_QSPI_Loader_REL.Command
goto OUT
:64BIT_HOST
if "%REPLY%"== "1" (goto HARDWARE_DEBUG_64) else (goto RELEASE_64)
:HARDWARE_DEBUG_64
echo Downloading QSPI boot loader (HardwareDebug) and user application.
"C:\Program Files (x86)\SEGGER\JLink_V490e\JLink.exe" -speed 12000 -if JTAG -device R7S721001 -CommanderScript Program_QSPI_Loader_HDB.Command
goto OUT
if "%REPLY%"== "2" (goto RELEASE_64) 
:RELEASE_64
echo Downloading QSPI boot loader (Release) and user application.
"C:\Program Files (x86)\SEGGER\JLink_V490e\JLink.exe" -speed 12000 -if JTAG -device R7S721001 -CommanderScript Program_QSPI_Loader_REL.Command
:OUT
echo ------------------------------------------------------------------------
pause
echo That's it.
goto END
echo ------------------------------------------------------------------------
if "%REPLY%"== "1" (goto HARDWAREDEBUG_ERROR)
:HARDWAREDEBUG_ERROR
echo Error:
echo File "HardwareDebug\RZ_A1H_QSPI_LOADER.bin" does not exist.
echo Did you build the RZ_A1H_QSPI_LOADER project in HardwareDebug mode?
goto EXIT
if "%REPLY%"== "2" (goto RELEASE_ERROR)
:RELEASE_ERROR
echo Error:
echo File "Release\RZ_A1H_QSPI_LOADER.bin" does not exist.
echo Did you build the RZ_A1H_QSPI_LOADER project in Release mode?
goto EXIT
:ERROR
echo ------------------------------------------------------------------------
echo File "user_application\RZ_A1H_Tutorial_RSK.bin" does not exist.
echo Please provide a user application binary file.
:EXIT
echo Press any key to exit.
echo ------------------------------------------------------------------------
pause
:END