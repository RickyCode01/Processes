#!/bin/sh

printf "\tAREOPORTO - Riccardo Manoni - 2021/2022\n\n"

out=$(gcc -w -lpthread -o AEROPORTO  Aeroporto.c 2>&1)
printf "$out"

if [[ -z "$out" ]]
then
./AEROPORTO
fi
