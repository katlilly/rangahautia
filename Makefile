testavx :
	g++ -Wall -march=native simple10avx.cpp search_avx_compress.cpp -lm -o testavx


indexer :
	g++ -O3 -Wall vbyte_compress.cpp growablearray.cpp tokeniser.cpp indexer.cpp -o indexer

clean :
	rm indexer search testavx

debug_indexer :
	g++ -g -Wall vbyte_compress.cpp growablearray.cpp tokeniser.cpp indexer.cpp -o debug_indexer


search :
	g++ -Wall vbyte_compress.cpp search.cpp -lm -o search


debug_search :
	g++ -g -Wall vbyte_compress.cpp athtable.cpp search.cpp -lm -o debug_search

