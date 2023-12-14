import zipfile
import os
import sys
import configparser

from MetadataProperties import MetadataProperties

config = configparser.ConfigParser()
config.read(os.path.join(os.path.dirname(__file__), 'config.ini'))

metadata_filename = config.get('filename', 'metadata_filename')
game_data_folder = config.get('filename', 'game_data_folder')
conj_ext = config.get('filename', 'conj_ext')

conjure_default_library_dir = os.path.expanduser(config.get('path', 'lib_dir'))
conjure_default_conj_dir = os.path.expanduser(config.get('path', 'conj_dir'))

default_collection_name = config.get('conjure', 'default_collection_name')


def read_game_metadata_in_zip(conj, metadata_property):
    with zipfile.ZipFile(conj, 'r') as zip_ref:
        with zip_ref.open(metadata_filename) as text_file:
            content = text_file.read().decode('utf-8')
    for line in content.split('\n'):
        key, value = line.split(': ', 1)
        if key == metadata_property:
            return value.strip('\n\r')


def find_all_conj_file(directory):
    conj_files_paths = [file for file in os.listdir(directory) if file.endswith(conj_ext)]

    if not conj_files_paths:
        print(f"No file with '{conj_ext}' extension found in {directory}")
    else:
        for file in conj_files_paths:
            print(f"File with '{conj_ext}' extension found: {file}")

    return conj_files_paths


def write_game_metadata_JSON(dir_path, json_object, index=None):
    metadata_file = os.path.join(dir_path, metadata_filename)

    with open(metadata_file, "r") as file:
        file_content = file.readlines()

    for key, value in json_object.items():
        property_line = f"{key}: {value}\n"
        if index is not None and 0 <= index < len(file_content):
            file_content.insert(index, property_line)
        else:
            for i, line in enumerate(file_content):
                if line.startswith(key):
                    file_content[i] = property_line
                    break
            else:
                file_content.append(property_line)

    with open(metadata_file, "w") as file:
        file.writelines(file_content)


def read_game_metadata(dir_path, metadata_property):
    metadata_file = os.path.join(dir_path, metadata_filename)

    with open(metadata_file, "r") as file:
        file_content = file.readlines()

    for line in file_content:
        if line.startswith(metadata_property):
            _, value = line.split(":")
            return value.strip()

    return None


def extract_conj(zip_file, output_folder):
    nested_folder = os.path.join(output_folder, game_data_folder)
    with zipfile.ZipFile(zip_file, 'r') as zip_ref:
        for file_info in zip_ref.infolist():
            file_path = os.path.join(output_folder, file_info.filename)
            if not file_info.is_dir():
                os.makedirs(os.path.dirname(file_path), exist_ok=True)
                with zip_ref.open(file_info) as source, open(file_path, 'wb') as destination:
                    if file_info.filename == f"{game_data_folder}.zip":
                        os.makedirs(nested_folder, exist_ok=True)
                        with zipfile.ZipFile(source, 'r') as game_zip:
                            game_zip.extractall(nested_folder)
                    else:
                        destination.write(source.read())

    os.remove(f"{nested_folder}.zip")


def unzip_conj(conj_dir_path, conj):
    conj_path = os.path.join(conj_dir_path, conj)

    collection_name = read_game_metadata_in_zip(conj_path, MetadataProperties.COLLECTION.value) or default_collection_name

    dir_path = os.path.join(conjure_default_library_dir, collection_name, os.path.splitext(conj)[0])

    os.makedirs(dir_path, exist_ok=True)

    extract_conj(conj_path, dir_path)

    write_game_metadata_JSON(dir_path, {
        "launch": "{file.path}",
        MetadataProperties.COLLECTION.value: collection_name
    }, 0)

    print(f"Successfully extracted {conj} content to {conjure_default_library_dir}/{collection_name}")


def get_id_from_file(file_path):
    with open(file_path, 'r') as file:
        for line in file:
            if line.startswith("id:"):
                _, id_value = line.split(":")
                return id_value.strip('\n\r')

    return None


def get_all_ids_in_folder(folder_path):
    all_ids = []

    for root, dirs, files in os.walk(folder_path):
        for filename in files:
            if filename == metadata_filename:
                file_path = os.path.join(root, filename)
                id_value = get_id_from_file(file_path)

                if id_value is not None:
                    all_ids.append({"id": id_value,
                                    "root": root})

    return all_ids


def game_to_update(conj_dir, conj, ids):
    conj_path = os.path.join(conj_dir, conj)
    zip_game_id = read_game_metadata_in_zip(conj_path, MetadataProperties.METADATA_ID.value)

    for a_id in ids:
        my_id = a_id["id"].strip()
        if my_id == zip_game_id:
            zip_game_version = read_game_metadata_in_zip(conj_path, MetadataProperties.VERSION.value)
            path = a_id["root"]
            game_version = read_game_metadata(path, "version")

            return zip_game_version > game_version

    return True


def print_usage():
    print('\033[93m', f"Usage of {os.path.basename(__file__)} :"
          f"\n  [with path in param] --> extract game at specified path"
          f"\n  [no param]           --> extract game at default path: {conjure_default_conj_dir}")


def main():
    print("----Decompressing script for Conjure Arcade library games----")

    conj_dir = conjure_default_conj_dir
    if len(sys.argv) == 1:
        print("Search for *.conj in " + conj_dir)
        os.makedirs(conj_dir, exist_ok=True)
    elif len(sys.argv) == 2:
        conj_dir = sys.argv[1]
        if not os.path.exists(conj_dir):
            print('\033[93m', f"path '{conj_dir}' not found\n")
            print_usage()
            return
    else:
        print_usage()
        return

    conj_paths = find_all_conj_file(conj_dir)
    all_ids = get_all_ids_in_folder(conjure_default_library_dir)
    for conj in conj_paths:
        if game_to_update(conj_dir, conj, all_ids):
            print(f"Extracting {conj}")
            unzip_conj(conj_dir, conj)


if __name__ == "__main__":
    main()
