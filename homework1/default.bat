g++ -Ofast code.cpp -o program

set one = %1

IF "%1"=="" (.\program) ELSE (.\program %1)

python show.py