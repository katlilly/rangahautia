indexer :
	g++ -O3 -Wall vbyte_compress.cpp athtable.cpp growablearray.cpp tokeniser.cpp indexer.cpp -o indexer

clean :
	rm indexer

debug_indexer :
	g++ -g athtable.cpp growablearray.cpp tokeniser.cpp indexer.cpp -o debug_indexer

search :
	g++ vbyte_decompress.cpp athtable.cpp growablearray.cpp search.cpp -o search

debug_search :
	g++ -g athtable.cpp growablearray.cpp search.cpp -o debug_search
