import random
import sys
import string

if len(sys.argv) != 3:
    print("Usage: ", sys.argv[0], " n outDir")
    exit(1)

n = int(sys.argv[1])
out_dir = sys.argv[2]

def rand_str(leng):
    letters = string.ascii_lowercase
    return ''.join(random.choice(letters) for i in range(leng))

for file_num in range(n):
    with open(f'{out_dir}/{file_num}.txt', 'w') as f:
        for _ in range(random.randint(4, 30)):
            delt = f'{random.randint(1, 100)} DEL {random.randint(0, 10)}'
            add = f'{random.randint(1, 100)} ADD {rand_str(random.randint(1, 40))}'

            print(f' {random.choice([delt, add])}', file=f)