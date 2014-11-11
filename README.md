# Conversation last said memory


## Installation

	mkdir build
	cmake ..
	make
	make install

## Running unit tests

	mkdir build
	cmake -DCMAKE_BUILD_TYPE=Debug ..
	make
	make test

### Running tests with valgrind

	G_SLICE=always-malloc valgrind --tool=memcheck --leak-check=yes --leak-resolution=high --num-callers=20 --track-fds=yes ./test/UnitTests
