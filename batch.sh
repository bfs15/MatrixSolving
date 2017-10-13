#!/bin/bash

#32 33 64 65 128 129 256 257 512 1000 2000
er() { echo "\$ $@" ; "$@" ; }

for size in 512; do
	for version in "master" "optm" "tri" "tri-pad"; do
		# er git checkout $version
		# er make rebuild
		# er mv invmat invmat_${version}
		for ((i=0; i<2; i++)); do
			args="-r$size -i3 -oout_${i}_${version}.txt"
			er ./MatrixSolving_${version}.exe $args
			# er ./invmat_${version} $args
		done
	done
done