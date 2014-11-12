# Piding remember read status plugin

## Installation

	mkdir build
	cd build
	cmake ..
	make
	make install

Enable plugin *Remember read status* from Pidgin plugins list.

## Running unit tests

	mkdir build
	cd build
	cmake -DCMAKE_BUILD_TYPE=Debug ..
	make
	make test

### Running tests with valgrind

	G_SLICE=always-malloc valgrind --tool=memcheck --leak-check=yes --leak-resolution=high --num-callers=20 --track-fds=yes ./test/UnitTests
