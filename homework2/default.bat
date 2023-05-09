g++ -Ofast code.cpp -o program

IF "%6"=="" (.\program) ELSE (.\program %1 %2 %3 %4 %5 %6)

python show.py