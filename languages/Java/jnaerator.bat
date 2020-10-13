@echo off  
set dst=XAPI.jar
set dll=*.dll
set inc=*.h
set sdk=XAPI
set rt=JNA
java -jar jnaerator-0.12-shaded.jar -jar %dst% -library %sdk%  -runtime %rt% %dll% %inc% -mode Jar -noMangling