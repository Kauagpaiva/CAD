#!/bin/bash

mkdir -p resultados
echo "versao,tamanho,threads,execucao,tempo" > resultados/tempos.csv

# Vetores de tamanhos e threads/processos
tamanhos=(1000 5000 10000 50000 100000)
threads=(1 2 4 8)

# Serial
for tam in "${tamanhos[@]}"; do
  for i in {1..3}; do
    tempo=$(./odd_even_serial $tam | grep Tempo | awk '{print $3}')
    echo "serial,$tam,1,$i,$tempo" >> resultados/tempos.csv
  done
done

# OpenMP
for tam in "${tamanhos[@]}"; do
  for th in "${threads[@]}"; do
    for i in {1..3}; do
      tempo=$(./odd_even_openmp $tam $th | grep Tempo | awk '{print $3}')
      echo "openmp,$tam,$th,$i,$tempo" >> resultados/tempos.csv
    done
  done
done

# MPI
for tam in "${tamanhos[@]}"; do
  for pr in "${threads[@]}"; do
    for i in {1..3}; do
      tempo=$(mpirun -np $pr ./odd_even_mpi $tam | grep "Tempo Total" | grep -Eo '[0-9]+\.[0-9]+')
      echo "mpi,$tam,$pr,$i,$tempo" >> resultados/tempos.csv
    done
  done
done

