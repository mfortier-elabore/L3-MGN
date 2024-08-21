# Installer cppUTest

Ajout le source comme un sous-module
$ git submodule add https://github.com/cpputest/cpputest.git

# Suivre "Using CppUTest with MakefileWorker.mk and gcc" sur https://cpputest.github.io/
$ cd cpputest \
$ autoreconf . -i
$ ./configure
$ make tdd
$ export CPPUTEST_HOME=$(pwd)

