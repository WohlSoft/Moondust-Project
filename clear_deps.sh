#!/bin/bash
if [ -d $PWD/_Libs/_sources/_build_cache ];
then
	echo "Deleting $PWD/_Libs/_sources/_build_cache..."
	rm -Rf $PWD/_Libs/_sources/_build_cache
fi
