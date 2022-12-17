#!/bin/sh

printf "\tAREOPORTO - Riccardo Manoni - 2021/2022\n\n"

if test -p "/tmp/myfifo"
then 
rm "/tmp/myfifo"
fi

out=$(gcc -w -o AEROPORTO  Aeroporto.c 2>&1)

if test -z "$out"
then
./AEROPORTO
else
printf "$out"
fi
