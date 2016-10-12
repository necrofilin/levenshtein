#!/bin/bash
CDIR=$(pwd)
DIR="${CDIR}/$(dirname ${BASH_SOURCE[0]})"

cd "$DIR"

PHP_VERSION="" # ANY INSTALLED
# PHP_VERSION=5.6
# PHP_VERSION=7.0

PHPIZE="phpize${PHP_VERSION}"
PHP_CONFIG="php-config${PHP_VERSION}"
export NO_INTERACTION=true

$PHPIZE --clean\
    && $PHPIZE \
    && ./configure --with-php-config="$PHP_CONFIG" \
    && make\
    && make test \
    && sudo make install \
    && gdb -ex run --batch --args php${PHP_VERSION} tests/003.phpt
    
# cat tests/003.log


