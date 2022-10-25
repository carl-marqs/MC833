#!/bin/bash

# Inicializar variáveis
port='1327'
address='127.0.0.1'

# Iniciar o servidor
echo "Starting server on port $port"
./servidor $port &
sleep 1 # dormir 2s

i=1
while [[ $i -le 12 ]] 
do
    # Iniciar cliente
    echo "Staring client: $i"
    ./cliente $address $port &
    # Incrementar contador
    ((i++))
done

# Exibir resultados
echo
echo ' ----- RESULTS ----- '
echo " ----- Established: $(netstat -taulpn | grep $port | grep 'ESTABLISHED' | grep '-' | wc -l)"
echo " ----- Total: $(netstat -taulpn | grep $port | wc -l)"
echo
netstat -taulpn | grep $port | grep '-'
echo

# Encerrar processos
echo "Killing server..."
pkill servidor
echo "Killing clients..."
killall cliente