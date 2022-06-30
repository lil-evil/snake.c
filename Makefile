default: build
.SILENT: run build clean

run: build
	./snake
	rm snake

build:
	echo compiling executable as "./snake"
	gcc -o snake snake.c -lncurses || (printf "\nnote: run \"make fix\" to fix dependencies errors\n" && exit 1)
	echo compiled successfuly

fix:
	sudo apt install -qqq build-essential libncurses-dev

clean:
	rm snake