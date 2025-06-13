#!/bin/bash
# Limpia todos los archivos de bloque fuera y dentro de la carpeta 'blocks' en cada disco

for disk in tecmfs_disk/disk{0..3}
  do
    # Elimina bloques fuera de 'blocks'
    find "$disk" -maxdepth 1 -type f -name 'blk_*.dat' -exec rm -v {} \;
    # Elimina bloques dentro de 'blocks'
    find "$disk/blocks" -type f -name 'blk_*.dat' -exec rm -v {} \;
  done

echo "Todos los archivos de bloque eliminados."
