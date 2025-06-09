from flask import Flask, request, jsonify
import os
from utils.config_parser import load_config

BLOCK_SIZE = 4096  # 4 KiB
TOTAL_SIZE = 64 * 1024 * 1024  # 64 MiB
NUM_BLOCKS = TOTAL_SIZE // BLOCK_SIZE

app = Flask(__name__)
config = load_config("config.xml")
disk_path = config['path']

# Asegura la existencia del directorio
os.makedirs(disk_path, exist_ok=True)

# Inicializar bloques
for i in range(NUM_BLOCKS):
    block_file = os.path.join(disk_path, f"blk_{i:05}.dat")
    if not os.path.exists(block_file):
        with open(block_file, 'wb') as f:
            f.write(b'\x00' * BLOCK_SIZE)

@app.route('/ping', methods=['GET'])
def ping():
    return jsonify({'status': 'OK'}), 200

@app.route('/read_block/<int:block_id>', methods=['GET'])
def read_block(block_id):
    if block_id < 0 or block_id >= NUM_BLOCKS:
        return jsonify({'error': 'Block ID out of range'}), 400

    block_file = os.path.join(disk_path, f"blk_{block_id:05}.dat")
    try:
        with open(block_file, 'rb') as f:
            data = f.read()
        return data, 200
    except Exception as e:
        return jsonify({'error': str(e)}), 500

@app.route('/write_block/<int:block_id>', methods=['POST'])
def write_block(block_id):
    if block_id < 0 or block_id >= NUM_BLOCKS:
        return jsonify({'error': 'Block ID out of range'}), 400

    data = request.get_data()
    if len(data) != BLOCK_SIZE:
        return jsonify({'error': f'Data must be exactly {BLOCK_SIZE} bytes'}), 400

    block_file = os.path.join(disk_path, f"blk_{block_id:05}.dat")
    try:
        with open(block_file, 'wb') as f:
            f.write(data)
        return jsonify({'status': 'OK'}), 200
    except Exception as e:
        return jsonify({'error': str(e)}), 500

if __name__ == '__main__':
    app.run(host=config['ip'], port=config['port'])
