import string
import random

def random_char(x):
  return ''.join(random.choice(string.ascii_lowercase) for y in range(x))

num_files = 3
num_chars_in_files = 10
file_names = []

for x in range(0, num_files): #zero based still, aww snap
  random_chars = random_char(num_chars_in_files)
  #print(random_chars, " is random chars")
  with open((random_chars + '.txt'), "w") as f:
    f.write(random_chars + '\n')
    file_names.append(random_chars + ".txt")

for name in file_names:
  with open(name, "r") as f:
    for line in f:
      print(line, end=''),

ar = random.randint(1,42)
br = random.randint(1,42)

print(ar)
print(br)
print(ar*br)
