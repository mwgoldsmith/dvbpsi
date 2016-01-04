## Process this file with automake to produce Makefile.in

EXTRA_DIST=index.doxygen structure.doxygen usage.doxygen newdec.doxygen \
	   migrate-v0-to-v1.doxygen migrate-v1-to-v2.doxygen \
           decoder.dot doxygen.cfg

MOSTLYCLEANFILES=decoder.png

doc: decoder.png
	doxygen doxygen.cfg
	cp decoder.png doxygen/html

decoder.png: decoder.dot
	dot -Tpng -o $@ $<

