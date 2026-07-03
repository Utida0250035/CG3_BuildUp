@echo off
chcp 65001 >nul

setlocal

set "TOOLS_DIR=%~dp0"

echo [] delete spellCheckers\inited

if exist "spellCheckers\inited" (

    echo delete.

    rmdir /q /s spellCheckers\inited

) else (

   echo not exist.

)

cd ..\..\..

echo [] delete .husky\

if exist ".husky" (

    echo delete.

    rmdir /q /s .husky

) else (

    echo not exist.

)

echo [] delete node_modules\

if exist "node_modules" (

    echo delete.


    rmdir /q /s node_modules

) else (

    echo not exist.

)

echo [] delete package-lock.json

if exist "package-lock.json" (

    echo delete.

    del package-lock.json

) else (

    echo not exist.

)

cd %TOOLS_DIR%

echo -
echo -

echo call ".\spellCheckersSetup.cmd"

call ".\spellCheckersSetup.cmd"

pause

exit