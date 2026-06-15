@echo off
setlocal

echo [] delete .checkers\.inited

if exist ".checkers\.inited" (

    echo delete.

    rmdir /q /s .checkers\.inited

) else (

   echo not exist.

)

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

echo -
echo -

echo call ".\_checkersSetup.cmd"

call ".\_checkersSetup.cmd"