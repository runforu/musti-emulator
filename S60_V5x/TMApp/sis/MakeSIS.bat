@ECHO OFF
makesis.exe -v TraceApp.pkg TraceApp.sis
signsis TraceApp.sis TraceApp.sisx RDTest_02.der RDTest_02.key

makesis.exe -v TraceApp_Debug.pkg TraceApp_Debug.sis
signsis TraceApp_Debug.sis TraceApp_Debug.sisx RDTest_02.der RDTest_02.key


del TraceApp.sis
del TraceApp_Debug.sis