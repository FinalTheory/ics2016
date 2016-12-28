import os
import sys

START_OFFSET = 1048576;

FILES = [
     '1.rpg',
     '2.rpg',
     '3.rpg',
     '4.rpg',
     '5.rpg',
     'abc.mkf',
     'ball.mkf',
     'data.mkf',
     'desc.dat',
     'fbp.mkf',
     'fire.mkf',
     'f.mkf',
     'gop.mkf',
     'map.mkf',
     'mgo.mkf',
     'm.msg',
     'mus.mkf',
     'pat.mkf',
     'rgm.mkf',
     'rng.mkf',
     'scrn0000.bmp',
     'sss.mkf',
     'voc.mkf',
     'wor16.asc',
     'wor16.fon',
     'word.dat'
 ]

def main():
    game = sys.argv[1]
    base, _ = os.path.split(sys.argv[0])
    cursor = 0
    with open('nemu-pal', 'w') as fid:
        fid.write('\0' * START_OFFSET)
        cursor += START_OFFSET
        for f in FILES:
            data = open(os.path.join(base, f), 'r').read()
            fid.write(data)
            print '{"%s", %d, %d}, ' % (f, len(data), cursor)
            cursor += len(data)
        fid.seek(0)
        game_data = open(game, 'r').read()
        fid.write(game_data)
        print 'game size = %d' % len(game_data)


if __name__ == "__main__":
    main()
