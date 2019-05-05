indexer :
	g++ -O3 -Wall vbyte_compress.cpp athtable.cpp growablearray.cpp tokeniser.cpp indexer.cpp -o indexer

clean :
	rm indexer search

debug_indexer :
	g++ -g athtable.cpp growablearray.cpp tokeniser.cpp indexer.cpp -o debug_indexer

oldsearch :
	g++ vbyte_decompress.cpp athtable.cpp growablearray.cpp oldsearch.cpp -o oldsearch

search :
	g++ -Wall vbyte_compress.cpp athtable.cpp search.cpp -lm -o search


debug_search :
	g++ -g athtable.cpp growablearray.cpp search.cpp -o debug_search
