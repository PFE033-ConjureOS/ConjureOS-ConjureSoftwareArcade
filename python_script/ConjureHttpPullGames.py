import json
import zipfile
import os
import sys
from datetime import datetime

import requests
from dotenv import load_dotenv
import re

from zipfile import ZipFile
from io import BytesIO


class BearerAuth(requests.auth.AuthBase):
    def __init__(self, token):
        self.token = token

    def __call__(self, r):
        r.headers["Authorization"] = "Bearer " + self.token
        return r


def bearer_auth():
    return BearerAuth(os.getenv('BEARER_KEY'))


def create_conj_folder_if_dont_exist(path):
    if not os.path.exists(path):
        os.makedirs(path)
        print(f"Folder '{path}' created")


def conj_folder():
    conj_dir = os.getenv('CONJ_DIR')
    full_conj_dir = os.path.expanduser(conj_dir)
    create_conj_folder_if_dont_exist(full_conj_dir)
    return full_conj_dir


def download_game(game_id):
    full_conj_dir = conj_folder()

    url = f"http://{os.getenv('DOMAIN')}/games/{game_id}/download"
    auth = bearer_auth()
    response = requests.get(url, auth=auth)

    print(response)

    if response.status_code == 200:

        content_disposition = response.headers['Content-Disposition']
        pattern = r"filename=([^\s']+)"
        match = re.search(pattern, content_disposition)
        if match:
            filename = match.group(1)
            with open(f"{full_conj_dir}{filename}.conj", "wb") as output_file:
                output_file.write(response.content)
            print(filename + " saved at " + full_conj_dir)
    else:
        print(f"Error: {response.status_code}")
        print(response.text)


def extract_download(content, path_dir):
    print("Extracting zip to " + path_dir)
    with ZipFile(BytesIO(content), 'r') as zip_ref:
        zip_ref.extractall(path_dir)


def download_all_game():
    full_conj_dir = conj_folder()

    url = f"http://{os.getenv('DOMAIN')}/games/download"
    auth = bearer_auth()
    response = requests.get(url, auth=auth)

    print(response)

    if response.status_code == 200:
        extract_download(response.content, full_conj_dir)
    else:
        print(f"Error: {response.status_code}")
        print(response.text)


def current_games():
    extracted_metadata_filename = os.getenv('METADATA_FILENAME')

    data_list = []

    conf_folder_path = conj_folder()

    for root, dirs, files in os.walk(conf_folder_path):
        for file in files:
            if file.endswith('.conj'):
                zip_file_path = os.path.join(root, file)
                with zipfile.ZipFile(zip_file_path, 'r') as zip_ref:
                    for zip_file_info in zip_ref.infolist():
                        if zip_file_info.filename == extracted_metadata_filename:
                            with zip_ref.open(zip_file_info.filename) as text_file:
                                content = text_file.read().decode('utf-8')
                                data = {}
                                for line in content.split('\n'):
                                    key, value = line.split(': ', 1)
                                    data[key] = value.strip()
                                data_list.append({'id': data.get('id'), 'version': data.get('version')})

    print("Current games:")
    print(json.dumps(data_list, indent=2))
    return json.dumps(data_list)


def update_games():
    full_conj_dir = conj_folder()

    url = f"http://{os.getenv('DOMAIN')}/games/download"
    headers = {'Content-Type': 'application/json'}
    data = current_games()
    auth = bearer_auth()
    response = requests.post(url, headers=headers, data=data, auth=auth)

    print(response)
    if response.status_code == 200:
        extract_download(response.content, full_conj_dir)
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


def test_connection():
    pass


def main():
    load_dotenv()
    test_connection()
    execute()


if __name__ == "__main__":
    main()
