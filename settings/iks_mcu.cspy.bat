@REM This batch file has been generated by the IAR Embedded Workbench
@REM C-SPY Debugger, as an aid to preparing a command line for running
@REM the cspybat command line utility using the appropriate settings.
@REM
@REM Note that this file is generated every time a new debug session
@REM is initialized, so you may want to move or rename the file before
@REM making changes.
@REM
@REM You can launch cspybat by typing the name of this batch file followed
@REM by the name of the debug file (usually an ELF/DWARF or UBROF file).
@REM
@REM Read about available command line parameters in the C-SPY Debugging
@REM Guide. Hints about additional command line parameters that may be
@REM useful in specific cases:
@REM   --download_only   Downloads a code image without starting a debug
@REM                     session afterwards.
@REM   --silent          Omits the sign-on message.
@REM   --timeout         Limits the maximum allowed execution time.
@REM 


"C:\Program Files (x86)\IAR Systems\Embedded Workbench 6.4\common\bin\cspybat" "C:\Program Files (x86)\IAR Systems\Embedded Workbench 6.4\arm\bin\armproc.dll" "C:\Program Files (x86)\IAR Systems\Embedded Workbench 6.4\arm\bin\armjlink.dll"  %1 --plugin "C:\Program Files (x86)\IAR Systems\Embedded Workbench 6.4\arm\bin\armbat.dll" --macro "D:\users\Anton\Documents\IAR_projects\plc_module_digital_output\config\flashloader\Milandr\MDR1986VE1T\Flash_MDR1986VE1T.mac" --flash_loader "D:\users\Anton\Documents\IAR_projects\plc_module_digital_output\config\flashloader\Milandr\MDR1986VE1T\Flash_MDR1986VE1T.board" --backend -B "--endian=little" "--cpu=Cortex-M1" "--fpu=None" "-p" "C:\Program Files (x86)\IAR Systems\Embedded Workbench 6.4\arm\CONFIG\debugger\Milandr\MDR1986VE1.ddf" "--drv_verify_download" "--semihosting" "--device=MDR1986VE1" "--drv_communication=USB0" "--jlink_speed=auto" "--jlink_initial_speed=1000" "--jlink_reset_strategy=0,0" "--jlink_interface=SWD" "--drv_catch_exceptions=0x000" "--drv_swo_clock_setup=144000000,1,2000000" 


