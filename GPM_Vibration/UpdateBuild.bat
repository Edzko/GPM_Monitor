@ECHO OFF
call build.bat
set /A b = %__MY_BUILD%+1
echo set __MY_BUILD=%b% > Build.bat
echo #define __MY_BUILD %b% > Build.h
echo #define __MY_DATE "%date:~-10,2%/%date:~-7,2%/%date:~-4,4% at %TIME:~0,2%:%TIME:~3,2%:%TIME:~6,2%" > MyDate.h
echo Build number now %b%. 

