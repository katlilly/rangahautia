indexer :
	g++ -O3 -Wall vbyte_compress.cpp athtable.cpp growablearray.cpp tokeniser.cpp indexer.cpp -o indexer

clean :
	rm indexer search newsearch

debug_indexer :
	g++ -g athtable.cpp growablearray.cpp tokeniser.cpp indexer.cpp -o debug_indexer

search :
	g++ vbyte_decompress.cpp athtable.cpp growablearray.cpp search.cpp -o search

newsearch :
	g++ vbyte_decompress.cpp athtable.cpp newsearch.cpp -lm -o newsearch


debug_search :
	g++ -g athtable.cpp growablearray.cpp search.cpp -o debug_search
