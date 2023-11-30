import os
import zipfile
import os
import shutil
import sys
import requests
from dotenv import load_dotenv
import re


class BearerAuth(requests.auth.AuthBase):
    def __init__(self, token):
        self.token = token

    def __call__(self, r):
        r.headers["Authorization"] = "Bearer " + self.token
        return r


def download_game(game_id):
    load_dotenv()
    conj_dir = os.getenv('CONJ_DIR')
    full_conj_dir = os.path.expanduser(conj_dir)

    domain = os.getenv('DOMAIN')

    url = "http://" + domain + "/games/" + game_id + "/download"

    key = os.getenv('BEARER_KEY')

    response = requests.get(url, auth=BearerAuth(key))

    content_disposition = response.headers['Content-Disposition']
    pattern = r"filename=([^\s']+)"
    match = re.search(pattern, content_disposition)
    if match:
        filename = match.group(1)
        open(full_conj_dir + filename, 'wb').write(response.content)
        print(filename + " saved at " + full_conj_dir)
    else:
        print("Filename not found in HTTP response")


def main():
    print("----Downloading Conjure Games from Server----")

    if len(sys.argv) == 1:
        print("Getting games from server")
    elif len(sys.argv) == 2:
        id_game = sys.argv[1]
        download_game(id_game)
    else:
        print(f"Usage: python {os.path.basename(__file__)}.py [id of the game]")

    id_test = 'f8836594-29a6-465f-b85b-36b9eecb155a.conj'
    download_game(id_test)  # TODO call one or all games to download


if __name__ == "__main__":
    main()
