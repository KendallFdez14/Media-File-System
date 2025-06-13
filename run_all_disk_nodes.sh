#!/bin/bash
# Lanza todos los Disk Nodes en segundo plano en el mismo terminal

for i in {0..3}
  do
    python3 tecmfs_disk/disk_node.py tecmfs_disk/disk$i/config$i.xml > tecmfs_disk/disk$i/node.log 2>&1 &
    echo "Disk Node $i iniciado (log: tecmfs_disk/disk$i/node.log)"
done

echo "Todos los Disk Nodes están corriendo en segundo plano."
