import zipfile
import os
import shutil
import sys

file_extension = ".conj"

curdir = os.path.expanduser('~/Documents')
conj_dir = os.path.expanduser('~/AppData/Local/conjure/conjure-os/conj')
conjure_library_dir = 'ConjureGames'
conjure_default_library_dir = os.path.join(curdir, conjure_library_dir)
extracted_metadata_filename = 'metadata.txt'
compress_game_data_filename = 'game'

if not os.path.exists(conjure_default_library_dir):
    os.mkdir(conjure_default_library_dir)


def read_game_meta_data_collection_name_in_conj(conj):
    search_word = "collection"
    default_collection = "MemberGames"

    with zipfile.ZipFile(conj, 'r') as zip_ref:
        zip_ref.extract(extracted_metadata_filename, path='temp')
        collection_name = default_collection
        found = False
        with open(f"./temp/{extracted_metadata_filename}", "r") as file:
            for line in file:
                if line.startswith(f"{search_word}:"):
                    collection_name = line.strip()
                    collection_name = collection_name.split(f"{search_word}:", 1)[1]
                    collection_name = collection_name.strip()
                    print(f"Found collection: {collection_name}")
                    found = True
                    break
            if not found:
                print(f"No collection found for {conj}. Using the default collection \"{default_collection}\"")
        shutil.rmtree('temp')
        return collection_name


def find_conj_file(directory):
    files = os.listdir(directory)
    count = 0
    conj_file = ""
    for file in files:
        if file.endswith(file_extension):
            count += 1
            if count > 1:
                print(f"more than one file with '{file_extension}' extension found in {directory}")
                break
            conj_file = file

    if count <= 0:
        print(f"No file with '{file_extension}' extension found in {directory}")

    elif count == 1:
        print(f"File with '{file_extension}' extension found: {file}")

    return conj_file


def find_all_conj_file(directory):
    files = os.listdir(directory)
    count = 0
    conj_files_paths = []

    for file in files:
        if file.endswith(file_extension):
            count += 1
            conj_files_paths.append(file)

    if count <= 0:
        print(f"No file with '{file_extension}' extension found in {directory}")

    elif count >= 1:
        for file in conj_files_paths:
            print(f"File with '{file_extension}' extension found: {file}")

    return conj_files_paths


def check_collection_value_and_update(dir_path, collection_name):
    search_word = "collection"
    new_lines = []

    with open(f"{dir_path}/{extracted_metadata_filename}", "r") as file:
        updated = False
        for line in file:
            if line.strip().startswith(f"{search_word}:"):
                updated = True
                new_lines.insert(0, f"{search_word}: {collection_name}\n")
            else:
                new_lines.append(line)

    if not updated:
        print(f"Add collection line in metadata at {dir_path}")
        new_lines.insert(0, f"{search_word}: {collection_name}\n")

    new_lines.append("\nlaunch: {file.path}")

    with open(f"{dir_path}/{extracted_metadata_filename}", "w") as output:
        output.writelines(new_lines)

    return updated


def unzip_conj(conj_dir_path, conj):
    conj_path = conj_dir_path + '/' + conj
    collection_name = read_game_meta_data_collection_name_in_conj(conj_path)

    dir_path = conjure_default_library_dir + "/" + collection_name + "/" + os.path.splitext(conj)[0]

    if not os.path.exists(dir_path):
        os.makedirs(dir_path)

    with zipfile.ZipFile(conj_path, 'r') as zip_ref:
        zip_ref.extractall(dir_path)

    check_collection_value_and_update(dir_path, collection_name)

    print(f"Successfully extracted .conj content to {conjure_default_library_dir}/{collection_name}")

    return dir_path


def unzip_game_file(dir_path):
    with zipfile.ZipFile(dir_path + f"/{compress_game_data_filename}.zip", 'r') as zip_ref:
        zip_ref.extractall(f"{dir_path}/{compress_game_data_filename}")

    os.remove(dir_path + f"/{compress_game_data_filename}.zip")


def main():
    print("----Decompressing script for Conjure Arcade library games----")

    conj_file_path = conj_dir
    if len(sys.argv) == 1:
        print("Search for *.conj in " + conj_dir)
    elif len(sys.argv) == 2:
        conj_file_path = sys.argv[1]
    else:
        print(f"Usage: python {os.path.basename(__file__)}.py [.conj_dir_path]")

    conj_paths = find_all_conj_file(conj_file_path)

    for conj in conj_paths:
        dir_path = unzip_conj(conj_file_path, conj)
        unzip_game_file(dir_path)


if __name__ == "__main__":
    main()
