import json
import zipfile
import os
import sys
import dotenv
import requests
import re
import configparser
from zipfile import ZipFile
from io import BytesIO

from MetadataProperties import MetadataProperties

dotenv.load_dotenv()
domain = os.getenv('DOMAIN')
username = os.getenv('CONJ_USERNAME')
password = os.getenv('CONJ_PASSWORD')

config = configparser.ConfigParser()
config.read(os.path.join(os.path.dirname(__file__), 'config.ini'))

metadata_filename = config.get('filename', 'metadata_filename')
conj_ext = config.get('filename', 'conj_ext')

conjure_default_conj_dir = os.path.expanduser(config.get('path', 'conj_dir'))


class BearerAuth(requests.auth.AuthBase):
    def __init__(self, token):
        self.token = token

    def __call__(self, r):
        r.headers["Authorization"] = "Bearer " + self.token
        return r


def bearer_auth():
    return BearerAuth(os.getenv('BEARER_KEY'))


def download_game(game_id):
    url = f"http://{domain}/games/{game_id}/download"
    auth = bearer_auth()
    response = requests.get(url, auth=auth)

    print(response)

    if response.status_code == 200:

        content_disposition = response.headers['Content-Disposition']
        pattern = r"filename=([^\s']+)"
        match = re.search(pattern, content_disposition)
        if match:
            filename = match.group(1)
            with open(f"{conjure_default_conj_dir}{filename}{conj_ext}", "wb") as output_file:
                output_file.write(response.content)
            print(filename + " saved at " + conjure_default_conj_dir)
    else:
        print(f"Error: {response.status_code}")
        print(response.text)


def extract_download(content, path_dir):
    print("Extracting zip to " + path_dir)
    with ZipFile(BytesIO(content), 'r') as zip_ref:
        zip_ref.extractall(path_dir)


def download_all_game():
    url = f"http://{domain}/games/download"
    auth = bearer_auth()
    response = requests.get(url, auth=auth)

    print(response)

    if response.status_code == 200:
        extract_download(response.content, conjure_default_conj_dir)
    else:
        print(f"Error: {response.status_code}")
        print(response.text)


def current_games():
    data_list = []

    for root, dirs, files in os.walk(conjure_default_conj_dir):
        for file in files:
            if file.endswith(conj_ext):
                zip_file_path = os.path.join(root, file)
                with zipfile.ZipFile(zip_file_path, 'r') as zip_ref:
                    for zip_file_info in zip_ref.infolist():
                        if zip_file_info.filename == metadata_filename:
                            with zip_ref.open(zip_file_info.filename) as text_file:
                                content = text_file.read().decode('utf-8')
                                data = {}
                                for line in content.split('\n'):
                                    key, value = line.split(': ', 1)
                                    data[key] = value.strip()
                                data_list.append({
                                    MetadataProperties.METADATA_ID.value:
                                        data.get(MetadataProperties.METADATA_ID.value),
                                    MetadataProperties.VERSION.value:
                                        data.get(MetadataProperties.VERSION.value)})

    print("Current games:")
    print(json.dumps(data_list, indent=2))
    return json.dumps(data_list)


def update_games():
    url = f"http://{domain}/games/download"
    headers = {'Content-Type': 'application/json'}
    data = current_games()
    auth = bearer_auth()
    response = requests.post(url, headers=headers, data=data, auth=auth)

    print(response)
    if response.status_code == 200:
        extract_download(response.content, conjure_default_conj_dir)
    else:
        print(f"Error: {response.status_code}")
        print(response.text)


def print_usage():
    print(f" -- Usage of {os.path.basename(__file__)}.py :")
    print(f"        -all                 -> (Pull a zip of all games)")
    print(f"        -update             -> (Update games)")
    print(f"        -id [id of the game] -> (Pull a game by id)")


def execute():
    if len(sys.argv) == 2:
        args = sys.argv[1]
        if args == "-all":
            print("Getting all games from the server.")
            download_all_game()
            return True
        elif args == "-update":
            print("Update all games from the server.")
            update_games()
            return True
    elif len(sys.argv) == 3:
        if sys.argv[1] == "-id":
            id_game = sys.argv[2]
            print(f"Getting games with id({id_game}) from the server.")
            download_game(id_game)
            return True
    print_usage()
    return False


def connexion():
    url = f"http://{domain}/login"
    headers = {'Content-Type': 'application/x-www-form-urlencoded'}
    data = {
        'username': username,
        'password': password
    }
    response = requests.post(url, headers=headers, data=data)

    if response.status_code == 200:
        print("Login succesful")
        content = response.json()

        token = content["token"]

        dotenv.set_key("../.env", "BEARER_KEY", token)
        return True
    else:
        print(f"Error: {response.status_code}")
        print(response.text)
        return False


def main():
    if connexion():
        execute()


if __name__ == "__main__":
    main()
