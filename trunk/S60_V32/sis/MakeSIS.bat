@ECHO OFF
cd ..\TMApp\sis\
call MakeSIS.bat
cd ..\..\sis
copy ..\TMApp\sis\TraceApp.sisx   .
copy ..\TMApp\sis\TraceApp_Debug.sisx   .

cd ..\TraceRedirection\sis\
call MakeSIS.bat
cd ..\..\sis
copy ..\TraceRedirection\sis\TraceRedirection.sisx   .


makesis.exe -v TraceManager.pkg TraceManager.sis
signsis TraceManager.sis TraceManager.sisx RDTest_02.der RDTest_02.key

makesis.exe -v TraceManager_Debug.pkg TraceManager_Debug.sis
signsis TraceManager_Debug.sis TraceManager_Debug.sisx RDTest_02.der RDTest_02.key

del TraceManager.sis
del TraceManager_Debug.sis