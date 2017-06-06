# быстрая компиляция, неоптимальный код – для дебага
debug:
	g++-4.8 sort-mapcount.cpp timer.cpp -o sort-mapcount -std=c++11

# медленная компиляция, оптимальный код – для релиза или тестов больших input'ов
o3:
	g++-4.8 sort-mapcount.cpp timer.cpp -o sort-mapcount -std=c++11 -fopenmp -O3 #-DNO_DEBUG_INFO

# для linux релиз
build:
	g++ sort-mapcount.cpp timer.cpp -o sort-mapcount -std=c++11 -fopenmp -O3 -DNO_DEBUG_INFO

clean:
	rm -rf sort-mapcount

test-tiny:
	./sort-mapcount -i tests/ids_tiny.bin