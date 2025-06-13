#!/bin/bash
# Detiene todos los procesos de Disk Node (python3 ejecutando disk_node.py)

pids=$(ps aux | grep 'python3 tecmfs_disk/disk_node.py' | grep -v grep | awk '{print $2}')

if [ -z "$pids" ]; then
  echo "No hay Disk Nodes corriendo."
else
  echo "Deteniendo Disk Nodes con PIDs: $pids"
  kill $pids
  echo "Todos los Disk Nodes han sido detenidos."
fi
