all: compile

compile:
	@ g++ CacheMemory.cpp -lreadline

testcase1:
	./a.out Testcases/cache_config1.txt

testcase2:
	./a.out Testcases/cache_config2.txt

testcase3:
	./a.out Testcases/cache_config3.txt

testcase4:
	./a.out Testcases/cache_config4.txt

testcase5:
	./a.out Testcases/cache_config5.txt

testcase6:
	./a.out Testcases/cache_config6.txt

testcase7:
	./a.out Testcases/cache_config7.txt

testcase8:
	./a.out Testcases/cache_config8.txt

testcase9:
	./a.out Testcases/cache_config9.txt

testcase10:
	./a.out Testcases/cache_config10.txt

testcase11:
	./a.out Testcases/cache_config11.txt

testcase12:
	./a.out Testcases/cache_config12.txt

testcase13:
	./a.out Testcases/cache_config13.txt

clean:
	rm -rf a.out