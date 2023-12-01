import zipfile
import os
import shutil
import sys
from dotenv import load_dotenv
import tempfile

load_dotenv()
metadata_filename = os.getenv('METADATA_FILENAME')
game_data_folder = os.getenv('GAME_DATA_FOLDER')


def read_game_metadata_in_zip(conj, metadata_property):
    with zipfile.ZipFile(conj, 'r') as zip_ref:
        with zip_ref.open(metadata_filename) as text_file:
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


def write_game_metadata(metadata_property, value):
    pass


def extract_conj(zip_file, output_folder):
    nested_folder = os.path.join(output_folder, game_data_folder)
    with zipfile.ZipFile(zip_file, 'r') as zip_ref:
        for file_info in zip_ref.infolist():
            file_path = os.path.join(output_folder, file_info.filename)
            if not file_info.is_dir():
                os.makedirs(os.path.dirname(file_path), exist_ok=True)
                with zip_ref.open(file_info) as source, open(file_path, 'wb') as destination:
                    if file_info.filename == game_data_folder:
                        os.makedirs(nested_folder, exist_ok=True)
                        with zipfile.ZipFile(source, 'r') as game_zip:
                            game_zip.extractall(nested_folder)
                    else:
                        destination.write(source.read())

    os.remove(f"{nested_folder}.zip")


def unzip_game_file(dir_path):
    game_data_zip = os.getenv('GAME_DATA_FOLDER_NAME')

    with zipfile.ZipFile(dir_path + f"/{game_data_zip}.zip", 'r') as zip_ref:
        zip_ref.extractall(f"{dir_path}/{game_data_zip}")

    os.remove(dir_path + f"/{game_data_zip}.zip")


def unzip_conj(conj_dir_path, conj):
    conj_path = os.path.join(conj_dir_path, conj)

    collection_name = read_game_metadata_in_zip(conj_path, "collection")
    if not collection_name:
        collection_name = "Members Games"

    conjure_default_library_dir = os.path.expanduser(os.getenv('LIB_DIR'))

    dir_path = os.path.join(conjure_default_library_dir, collection_name, os.path.splitext(conj)[0])

    os.makedirs(dir_path, exist_ok=True)

    extract_conj(conj_path, dir_path)

    write_game_metadata("collection", collection_name)
    write_game_metadata("launch", "{file.path}")

    print(f"Successfully extracted {conj} content to {conjure_default_library_dir}/{collection_name}")


def game_exist(conj_dir, conj):
    pass


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
        if not game_exist(conj_dir, conj):
            unzip_conj(conj_dir, conj)


if __name__ == "__main__":
    main()
