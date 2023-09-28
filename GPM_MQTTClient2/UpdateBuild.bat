@ECHO OFF

REM this script is called from .\Bin\Debug folder

REM cd c:\projects\EA\EdzkoLib\EdzkoLib

REM copy chm\Reference.chm HelpFiles /y
REM copy chm\Reference.hhc HelpFiles /y
REM "C:\Program Files (x86)\HTML Help Workshop\hhc.exe" HelpFiles/EAAddin.hhp

cd ..\..\Resources

REM get the last build number
call build.bat

REM increment the build number and store on file
set /A b = %__MY_BUILD%+1
echo set __MY_BUILD=%b% > Build.bat

REM create the souce file for the project with the current build number and date
del b2.cs /q
del b4.cs /q
echo         public int build = %b%; > b2.cs
echo         public string data = "%date:~-10,2%/%date:~-7,2%/%date:~-4,4% at %time:~-11,5%"; > b4.cs
copy b?.cs ..\build.cs /y

REM echo namespace EdzkoLib {  private class VersionInfo {  int build = %b%;  string data = "%date:~-10,2%/%date:~-7,2%/%date:~-4,4% at %time:~-11,5%";} } > Build.cs

REM report current build number in Visual Studio
echo Build number now %b%. 

cd ../Properties

echo using System.Reflection; > AssemblyBuild.cs
echo using System.Runtime.CompilerServices; >> AssemblyBuild.cs
echo using System.Runtime.InteropServices; >> AssemblyBuild.cs
echo.>> AssemblyBuild.cs
echo [assembly: AssemblyVersion("1.5.%b%.0")] >> AssemblyBuild.cs
echo [assembly: AssemblyFileVersion("1.0.%b%")] >> AssemblyBuild.cs
