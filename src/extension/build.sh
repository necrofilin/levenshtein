#!/bin/bash
CDIR=$(pwd)
DIR="${CDIR}/$(dirname ${BASH_SOURCE[0]})"

cd "$DIR"

PHP_VERSION="" # ANY INSTALLED
#PHP_VERSION=5.6
phpPHP_VERSION=7.0

PHPIZE="phpize${PHP_VERSION}"
PHP_CONFIG="php-config${PHP_VERSION}"

$PHPIZE --clean\
    && $PHPIZE \
    && ./configure --with-php-config="$PHP_CONFIG"\
    && make\
    && make test \
    && sudo make install
