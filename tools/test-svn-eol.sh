#!/bin/sh

# Prints files that don't have the "svn:eol-style" property set.
#
# This shell script will search for files ending with ".cpp", ".h" and ".pro"
# in the current directory and all sub directories. If the found file doesn't
# have the "svn:eol-style" property set, its file file name is printed to
# standard output.
#
# Hidden files / directories are exluded from the search.
#
# No command line switches are available

for file in `find . \( ! -regex '.*/\..*' -a -name "*.cpp" -o -name "*.h" -o -name "*.pro"  \) -type f`
do
	if [ "x`svn propget svn:eol-style $file`" = "x" ]
	then	
		echo "$file"
	fi
done

exit 0
