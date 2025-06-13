import xml.etree.ElementTree as ET

def load_config(config_file):
    tree = ET.parse(config_file)
    root = tree.getroot()
    return {
        'ip': root.find('IP').text,
        'port': int(root.find('Port').text),
        'path': root.find('Path').text
    }
