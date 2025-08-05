import unittest
from your_module import validate_note

class TestValidateNote(unittest.TestCase):
    def test_valid_note(self):
        self.assertTrue(validate_note('C4'))

    def test_invalid_note(self):
        self.assertFalse(validate_note('H8'))

if __name__ == '__main__':
    unittest.main()