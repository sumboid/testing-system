### Testing system
#### Creating building script
`aclocal -I m4 --install && autoconf --force && libtoolize && automake --add-missing`

#### Building and installing
`./configure && make && make install`
