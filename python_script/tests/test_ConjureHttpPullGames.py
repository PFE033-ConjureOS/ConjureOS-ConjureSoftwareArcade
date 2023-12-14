import os
from unittest.mock import patch

from ConjureHttpPullGames import current_games

test_conj_dir = 'test_data'


@patch('ConjureHttpPullGames.conjure_default_conj_dir', os.path.abspath(test_conj_dir))
def test_current_games():
    result = current_games()
    expected = '[{"id": "00000000-0000-0000-0000-000000000000", "version": "0.1.1"}]'
    assert expected == result
