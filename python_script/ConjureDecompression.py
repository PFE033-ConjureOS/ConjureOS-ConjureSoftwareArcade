import zipfile
import os
import shutil
import sys
from dotenv import load_dotenv
import tempfile

load_dotenv()
extracted_metadata_filename = os.getenv('METADATA_FILENAME')


def read_game_metadata_in_zip(conj, metadata_property):
    with zipfile.ZipFile(conj, 'r') as zip_ref:
        with zip_ref.open(extracted_metadata_filename) as text_file:
            content = text_file.read().decode('utf-8')
    for line in content.split('\n'):
        key, value = line.split(': ', 1)
        if key == metadata_property:
            return value

def find_all_conj_file(directory):
    file_extension = os.getenv('CONJ_EXT')
    conj_files_paths = [file for file in os.listdir(directory) if file.endswith(file_extension)]

    if not conj_files_paths:
        print(f"No file with '{file_extension}' extension found in {directory}")
    else:
        for file in conj_files_paths:
            print(f"File with '{file_extension}' extension found: {file}")

    return conj_files_paths


def unzip_conj(conj_dir_path, conj):
    conj_path = os.path.join(conj_dir_path, conj)

    collection_name = read_game_metadata_in_zip(conj_path, "collection")
    if collection_name is None:
        collection_name = "Members Games"

    conjure_default_library_dir = os.path.expanduser(os.getenv('LIB_DIR'))

    dir_path = os.path.join(conjure_default_library_dir, collection_name, os.path.splitext(conj)[0])

    os.makedirs(dir_path, exist_ok=True)

    with zipfile.ZipFile(conj_path, 'r') as zip_ref:
        zip_ref.extractall(dir_path)

    # TODO update collection iin metadata

    print(f"Successfully extracted {conj} content to {conjure_default_library_dir}/{collection_name}")

    unzip_game_file(dir_path)


def unzip_game_file(dir_path):
    game_data_zip = os.getenv('GAME_DATA_FOLDER_NAME')
    zip_file_path = os.path.join(dir_path, f"{game_data_zip}.zip")

    with zipfile.ZipFile(zip_file_path, 'r') as zip_ref:
        zip_ref.extractall(f"{dir_path}/{game_data_zip}")

    os.remove(zip_file_path)


def main():
    print("----Decompressing script for Conjure Arcade library games----")

    conj_dir = os.path.expanduser(os.getenv('CONJ_DIR'))
    if len(sys.argv) == 1:
        print("Search for *.conj in " + conj_dir)
        os.makedirs(conj_dir, exist_ok=True)
    elif len(sys.argv) == 2:
        conj_dir = sys.argv[1]
    else:
        print(f"Usage: python {os.path.basename(__file__)}.py [.conj_dir_path]")

    conj_paths = find_all_conj_file(conj_dir)
    for conj in conj_paths:
        unzip_conj(conj_dir, conj)


if __name__ == "__main__":
    main()
