all:
	g++ master.cpp -o master.out
	g++ mmv.cpp -o mmv.out

clean:
	rm -rf master.out mmv.out happy.tmp happy2.tmp 

test:
	cat blake.txt
	ls
	./master.out
	cat happy.tmp
	ls
	./master.out blake.txt happy2.tmp
	cat happy2.tmp