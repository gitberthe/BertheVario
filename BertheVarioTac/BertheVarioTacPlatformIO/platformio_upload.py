
# Allows PlatformIO to upload directly to ElegantOTA
#
# To use:
# - copy this script into the same folder as your platformio.ini
# - set the following for your project in platformio.ini:
#
# extra_scripts = platformio_upload.py
# upload_protocol = custom
# upload_url = <your upload URL>
# 
# An example of an upload URL:
# upload_URL = http://192.168.1.123

import requests
import hashlib
from urllib.parse import urlparse
Import("env")

try:
    from requests_toolbelt import MultipartEncoder, MultipartEncoderMonitor
    from tqdm import tqdm
except ImportError:
    env.Execute("$PYTHONEXE -m pip install requests_toolbelt")
    env.Execute("$PYTHONEXE -m pip install tqdm")
    from requests_toolbelt import MultipartEncoder, MultipartEncoderMonitor
    from tqdm import tqdm

def on_upload(source, target, env):
    firmware_path = str(source[0])
    upload_url = env.GetProjectOption('upload_url')

    with open(firmware_path, 'rb') as firmware:
        md5 = hashlib.md5(firmware.read()).hexdigest()

        parsed_url = urlparse(upload_url)
        host_ip = parsed_url.netloc

        # Führe die GET-Anfrage aus
        start_url = f"{upload_url}/ota/start?mode=fr&hash={md5}"
        # start_response = requests.get(start_url)
        start_headers = {
            'Host': host_ip,
            'User-Agent': 'Mozilla/5.0 (X11; Ubuntu; Linux x86_64; rv:109.0) Gecko/20100101 Firefox/118.0',
            'Accept': '*/*',
            'Accept-Language': 'de,en-US;q=0.7,en;q=0.3',
            'Accept-Encoding': 'gzip, deflate',
            'Referer': f'{upload_url}/update',
            'Connection': 'keep-alive'
            }

        start_response = requests.get(start_url, headers=start_headers)
        
        # Drucke Anfrage- und Antwortkopfzeilen für die GET-Anfrage
        # print("GET Anfragekopfzeilen:", start_response.request.headers)
        # print("GET Antwortkopfzeilen:", start_response.headers)

        if start_response.status_code != 200:
            print("Start-Request fehlgeschlagen " + str(start_response.status_code))
            return

        firmware.seek(0)
        encoder = MultipartEncoder(fields={
            'MD5': md5,
            'firmware': ('firmware', firmware, 'application/octet-stream')}
        )

        bar = tqdm(desc='Upload Progress',
                   total=encoder.len,
                   dynamic_ncols=True,
                   unit='B',
                   unit_scale=True,
                   unit_divisor=1024
                   )

        monitor = MultipartEncoderMonitor(encoder, lambda monitor: bar.update(monitor.bytes_read - bar.n))

        post_headers = {
            'Host': host_ip,
            'User-Agent': 'Mozilla/5.0 (X11; Ubuntu; Linux x86_64; rv:109.0) Gecko/20100101 Firefox/118.0',
            'Accept': '*/*',
            'Accept-Language': 'de,en-US;q=0.7,en;q=0.3',
            'Accept-Encoding': 'gzip, deflate',
            'Referer': f'{upload_url}/update',
            'Connection': 'keep-alive',
            'Content-Type': monitor.content_type,
            'Content-Length': str(monitor.len),
            'Origin': f'{upload_url}'
        }


        response = requests.post(f"{upload_url}/ota/upload", data=monitor, headers=post_headers)
        
        # Drucke Anfrage- und Antwortkopfzeilen für die POST-Anfrage
        # print("POST Anfragekopfzeilen:", response.request.headers)
        # print("POST Antwortkopfzeilen:", response.headers)

        bar.close()

        if response.status_code != 200:
            print("Upload fehlgeschlagen")
        else:
            print("Upload erfolgreich", response.text)

            
env.Replace(UPLOADCMD=on_upload)