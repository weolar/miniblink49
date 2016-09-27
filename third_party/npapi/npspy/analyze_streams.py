# A script for analyzing the output of NPSPY and merging data about streams.

import sys


def ReadFile(filename, flags='rb'):
  """Returns the contents of a file."""
  file = open(filename, flags)
  result = file.read()
  file.close()
  return result


def WriteFile(filename, contents):
  """Overwrites the file with the given contents."""
  file = open(filename, 'w')
  file.write(contents)
  file.close()  
  

# sample line: 'NPP_NewStream(0x645c898, 0x56ba900("application/x-shockwave-flash"), 0x64bb3b0 (http://weeklyad.target.com/target/flash/target/target.swf?ver=090326), TRUE, NP_NORMAL)'
class Stream:
  def __init__(self, line):
    split = line.split(', ')
    
    self.mime_type = split[1].split('"')[1]
    self.url = split[2].split(' ')[1].strip('()')
    self.seekable = split[3]
    self.type = split[4].strip(')')
    self.size = 0
    self.status = ''
    try:
      self.address = split[2].split(' ')[0]
    except:
      print 'parsing error on ' + line
      self.address = ''

    if self.type != 'NP_NORMAL':
      print 'line got unexpected type: ' + line

    
def main(argv=None):
  if argv is None:
    argv = sys.argv
    
  streams = []

  if len(argv) != 2:
    print 'need filename'
    return
  file = ReadFile(argv[1])
  for line in file.splitlines():
    if line.startswith('NPP_NewStream('):
      if line.count('(') < 3:
        print 'unknown format for line: ' + line
        continue

      s = Stream(line)
      streams.append(s)
    elif line.startswith('NPP_Write('):
      # sample: NPP_Write(0x645c898, 0x64bb3b0, 0, 16384, 0x56c1000("CW")))
      split = line.split(', ')
      address = split[1]
      start = int(split[2])
      size = int(split[3])
      found = False
      for stream in streams:
        if stream.address == address:
          if stream.size != start:
            print 'error: starting at wrong place for write ' + stream.url + ' ' + str(stream.size) + ' ' + str(start)
          stream.size += size
          found = True
          break
          
      if not found:
        print "couldn't find stream to match NPP_Write " + line
    elif line.startswith('NPP_DestroyStream('):
      # sample: NPP_DestroyStream(0x645c898, 0x64bb3b0, NPRES_DONE)
      split = line.split(', ')
      address = split[1]
      status = split[2].strip(')')
      found = False
      for stream in streams:
        if stream.address == address:
          stream.status = status
          stream.address = ''  # address can be reused
          found = True
          break
          
      if not found:
        print "couldn't find stream to match NPP_DestroyStream " + line


  output = []
  for stream in streams:
    if stream.status != 'NPRES_DONE':
      print 'error: no NPP_DestroyStream with success for ' + stream.url + ' ' + stream.status + '.'
    output.append(', '.join([stream.url, stream.mime_type, str(stream.size), stream.seekable]))
  output_file = argv[1].replace('.', '_analyzed.')
  
  WriteFile(output_file, '\n'.join(output))
  
  
if __name__ == "__main__":
  sys.exit(main())
