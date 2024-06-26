import collections

# Python has enums now, but Goobuntu machines seem to run very old python. From:
# http://stackoverflow.com/questions/36932/how-can-i-represent-an-enum-in-python
def enum(*keys):
  return collections.namedtuple('Enum', keys)(*keys)
