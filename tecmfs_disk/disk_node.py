from flask import Flask, request, jsonify
import os
import sys
from utils.config_parser import load_config

BLOCK_SIZE = 4096  # Tamaño de bloque en bytes
TOTAL_SIZE = 64 * 1024 * 1024  # Tamaño total del disco en bytes
NUM_BLOCKS = TOTAL_SIZE // BLOCK_SIZE

app = Flask(__name__)

config_file = sys.argv[1] if len(sys.argv) > 1 else "config.xml"
config = load_config(config_file)
disk_path = os.path.join(config['path'], 'blocks')

os.makedirs(disk_path, exist_ok=True)

# Inicializa los archivos de bloque si no existen
for i in range(NUM_BLOCKS):
    block_file = os.path.join(disk_path, f"blk_{i:05}.dat")
    if not os.path.exists(block_file):
        with open(block_file, 'wb') as f:
            f.write(b'\x00' * BLOCK_SIZE)

@app.route('/ping', methods=['GET'])
def ping():
    """Verifica si el nodo de disco está activo"""
    return jsonify({'status': 'OK'}), 200

@app.route('/read_block/<int:block_id>', methods=['GET'])
def read_block(block_id):
    """Lee un bloque específico del disco"""
    if block_id < 0 or block_id >= NUM_BLOCKS:
        return jsonify({'error': 'Block ID fuera de rango'}), 400
    block_file = os.path.join(disk_path, f"blk_{block_id:05}.dat")
    try:
        with open(block_file, 'rb') as f:
            data = f.read()
        return data, 200
    except Exception as e:
        return jsonify({'error': str(e)}), 500

@app.route('/write_block/<int:block_id>', methods=['POST'])
def write_block(block_id):
    """Escribe datos en un bloque específico del disco"""
    if block_id < 0 or block_id >= NUM_BLOCKS:
        return jsonify({'error': 'Block ID fuera de rango'}), 400
    data = request.get_data()
    if len(data) != BLOCK_SIZE:
        return jsonify({'error': f'Los datos deben tener exactamente {BLOCK_SIZE} bytes'}), 400
    block_file = os.path.join(disk_path, f"blk_{block_id:05}.dat")
    try:
        with open(block_file, 'wb') as f:
            f.write(data)
        return jsonify({'status': 'OK'}), 200
    except Exception as e:
        return jsonify({'error': str(e)}), 500

if __name__ == '__main__':
    # Inicia el servidor Flask en la IP y puerto configurados
    app.run(host=config['ip'], port=config['port'])
