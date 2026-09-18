"""Patch only Thorn Gorge's geographic map rectangle; preserve every other record."""
import argparse
import math
from pathlib import Path
import struct

def patch(data):
    b=bytearray(data)
    if b[:4] != b'WDBC': raise ValueError('expected WDBC')
    count,fields,size,strings=struct.unpack_from('<4I',b,4)
    if fields != 8 or size != 32 or len(b) != 20+count*size+strings:
        raise ValueError('unexpected WorldMapArea layout')
    matches=[]
    for i in range(count):
        at=20+i*size
        if struct.unpack_from('<I',b,at+4)[0]==821: matches.append(at)
    if len(matches)!=1: raise ValueError('expected one map 821 row')
    at=matches[0]
    if struct.unpack_from('<III',b,at)!=(702,821,5722): raise ValueError('unexpected Thorn Gorge row')
    old=(2105.,1058.,2522.,1828.)
    scale=694/870
    new=(1581.5+523.5/scale,1581.5-523.5/scale,2610.,1740.)
    current=struct.unpack_from('<4f',b,at+16)
    if not all(math.isclose(a,c,abs_tol=.001) for a,c in zip(current,old)) and not all(math.isclose(a,c,abs_tol=.001) for a,c in zip(current,new)):
        raise ValueError('source bounds changed; review artwork alignment first')
    struct.pack_into('<4f',b,at+16,*new)
    assert b[:at+16]==data[:at+16] and b[at+32:]==data[at+32:]
    for x,y in [(2554.2,1597.33),(1806.6,1538.97),(1819.945801,1542.085083)]:
        assert 0<(new[0]-y)/(new[0]-new[1])<1 and 0<(new[2]-x)/(new[2]-new[3])<1
    return bytes(b)

if __name__=='__main__':
    parser=argparse.ArgumentParser();parser.add_argument('source',type=Path);parser.add_argument('output',type=Path)
    args=parser.parse_args();args.output.parent.mkdir(parents=True,exist_ok=True)
    args.output.write_bytes(patch(args.source.read_bytes()))
    print('Patched map 821 bounds; every other record and all strings preserved.')
