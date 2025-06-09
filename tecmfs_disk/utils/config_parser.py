import requests

# Escribir un bloque
block_id = 0
data = bytes([1] * 4096)
response = requests.post(f'http://127.0.0.1:8000/write_block/{block_id}', data=data)
print(response.json())

# Leer un bloque
response = requests.get(f'http://127.0.0.1:8000/read_block/{block_id}')
if response.status_code == 200:
    data = response.content
    print(f'Recibidos {len(data)} bytes')

# Verificar salud del nodo
response = requests.get('http://127.0.0.1:8000/ping')
print(response.json())
