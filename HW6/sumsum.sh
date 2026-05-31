#!/bin/sh

sum() {
    TOTAL=0
    for ARG in "$@"; do
        TOTAL=$(expr "$TOTAL" + "$ARG" 2>/dev/null)
        if [ $? -ne 0 ]; then
            echo 0
            return
        fi
    done
    echo "$TOTAL"
}

read LINE1
read LINE2

SUM1=$(sum $LINE1)
SUM2=$(sum $LINE2)

if [ "$SUM1" -eq "$SUM2" ]; then
    echo "Equal"
else
    echo "Not equal"
fi
