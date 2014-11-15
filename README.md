# Pidgin remember read status plugin

[![Build Status](https://travis-ci.org/nablaa/pidgin-remember-read-status.svg?branch=master)](https://travis-ci.org/nablaa/pidgin-remember-read-status)

This plugin will remember the read status of chat conversations when
disconnecting from a chat. Without this plugin, when connecting to a chat
room, the chat room will be shown as if there are unseen messages even if
you had previously seen all messages.

This plugin fixes that by remembering the last messages that have arrived to
a certain chatroom. If the user has seen previously the latest message, the
chat will be be marked as unseen.

## Installation

Install pre-requisite `pidgin-devel` and `libpurple-devel` packages. CMake is
used for building and therefore it is also required. On Ubuntu:

	sudo apt-get build-dep pidgin
	sudo apt-get install pidgin-dev libpurple-dev cmake cpputest

This will compile the plugin and install it to `~/.purple/plugins/`:

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

## Running static code analysis

	./run_cppcheck.sh
