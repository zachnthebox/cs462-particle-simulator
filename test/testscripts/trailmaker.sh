#!/bin/sh

for((j = 0; j < 5; j++))
do {
  echo -n > trail.$j.txt
  for((i = 1; i <= 16; i *= 2))
  do {
  y=$(($j + 1));
  line=$((head -n $y $i.txt) | tail -n 1)
  tokens=($line);
  echo -n "$i " >> trail.$j.txt
  echo ${tokens[1]} >> trail.$j.txt
  }
  done

}
done