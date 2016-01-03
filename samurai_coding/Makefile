all: managerDir playersDir

TAGS: manager1.1/*.cpp manager1.1/*.hpp players/*.cpp players/*.hpp
	etags $^

managerDir:
	cd manager1.1; make

playersDir:
	cd players; make

tar: clean
	tar zcvf ../samurai.tgz .

clean:
	rm -f TAGS *~
	cd manager; make clean
	cd players; make clean
