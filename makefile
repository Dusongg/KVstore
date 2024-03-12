kvserver:kvstore.cc
	g++ -o $@ $^ -std=c++11

.PHONY:clean
clean:
	rm -rf kvserver