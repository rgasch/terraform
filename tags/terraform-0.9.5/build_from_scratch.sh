#!/bin/sh

echo "Running aclocal";
aclocal
echo "Running autoheader";
autoheader
echo "Running autoconf";
autoconf
echo "Running automake --add-missing";
automake --add-missing

