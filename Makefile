indexer :
	g++ -O3 -Wall athtable.cpp growablearray.cpp tokeniser.cpp indexer.cpp -o indexer

clean :
	rm indexer

debug_indexer :
	g++ -g athtable.cpp growablearray.cpp tokeniser.cpp indexer.cpp -o debug
