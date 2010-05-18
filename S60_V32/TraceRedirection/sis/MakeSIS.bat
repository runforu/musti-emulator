@ECHO OFF
makesis.exe -v TraceRedirection.pkg TraceRedirection.sis
signsis TraceRedirection.sis TraceRedirection.sisx RDTest_02.der RDTest_02.key

del TraceRedirection.sis
