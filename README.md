# Grid
in build/ :/bin/bash -c "cd tests && ./unit_test"
test start
in build/ :/bin/bash -c "cd binary && sudo su root"" export GRID_CONFIG="/blabla" && ./Grid start containerid"
need sudo(for mount)
format
find -not -path "./externals/*" -iname *.hpp -o -iname *.cpp | xargs clang-format -i
