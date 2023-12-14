import configparser
import os
import shutil

from ConjureDecompression import read_game_metadata_in_zip, find_all_conj_file, write_game_metadata_JSON, \
    read_game_metadata, extract_conj
from MetadataProperties import MetadataProperties

from tests.test_data.metadataObject import MetaDataObject

config = configparser.ConfigParser()
config.read(os.path.join(os.path.dirname(__file__), '../config.ini'))
conj_ext = config.get('filename', 'conj_ext')
conjure_default_conj_dir = os.path.expanduser(config.get('path', 'conj_dir'))
metadata_filename = config.get('filename', 'metadata_filename')

test_conj_dir = 'test_data'
test_conj_name = 'test_conj'
test_conj = f'{test_conj_name}{conj_ext}'
test_conj_path = f'{test_conj_dir}/{test_conj}'

metaDataObjet_test = MetaDataObject()


def test_read_game_metadata_in_zip():
    for property_line in MetadataProperties:
        property_result = read_game_metadata_in_zip(test_conj_path, property_line.value)
        expected_result = metaDataObjet_test.get(property_line)
        assert expected_result == property_result


def test_find_all_conj_file():
    result = find_all_conj_file(test_conj_dir)
    assert 1 == len(result)
    assert test_conj == result[0]


def test_write_game_metadata_JSON():
    folder = f"{test_conj_dir}/empty/"
    metadata_file = f"{folder}{metadata_filename}"

    with open(metadata_file, 'w') as file_clear:
        pass

    json = {
        "launch": "{file.path}",
        "collection": "collection_name"
    }
    expected_text = "collection: collection_name\nlaunch: {file.path}\n"

    write_game_metadata_JSON(folder, json, 0)

    with open(metadata_file, 'r') as result_file_contents:
        assert expected_text == result_file_contents.read()


def test_read_game_metadata():
    for property_line in MetadataProperties:
        property_result = read_game_metadata(test_conj_dir, property_line.value)
        expected_result = metaDataObjet_test.get(property_line)
        assert expected_result == property_result


def test_extract_conj():
    folder = f"{test_conj_dir}/ExtractGame"
    os.makedirs(folder, exist_ok=True)
    extract_conj(test_conj_path, folder)

    expected_files = [f"{metadata_filename}", "game/game.exe",
                      "medias/gameplayImage.png", "medias/thumbnail.png"]

    for expected_file in expected_files:
        file_path = f"{folder}/{expected_file}"
        assert os.path.exists(file_path)

    shutil.rmtree(folder)
