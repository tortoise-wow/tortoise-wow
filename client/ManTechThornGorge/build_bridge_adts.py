"""Append native MDDF ramps only to the four Thorn bridge ADTs.

All terrain, water, texture data and existing placement records are preserved.
MCRF, MCNK subchunk offsets, MCIN and MHDR are rebuilt after insertion.
"""
from pathlib import Path
import struct,json,hashlib,argparse
parser=argparse.ArgumentParser(description=__doc__)
parser.add_argument('source',type=Path,help='Directory containing the four original extracted ADTs')
parser.add_argument('output',type=Path,help='New patch contents directory')
args=parser.parse_args()
recipe=json.loads(Path(__file__).with_name('bridge-ramps.json').read_text())
ramps=recipe['ramps']
assert all(not r['surface_conflicts'] for r in ramps)
filename=b'World\\Generic\\PassiveDoodads\\Ships\\ShipRamps\\ShipRamp01.mdx\0'
def u32(b,o):return struct.unpack_from('<I',b,o)[0]
def chunks(b):
 out=[];p=0
 while p<len(b):
  tag=b[p:p+4];n=u32(b,p+4);assert p+8+n<=len(b)
  out.append([tag,bytearray(b[p+8:p+8+n]),p]);p+=8+n
 assert p==len(b);return out
def serialize(chunks):return b''.join(tag+struct.pack('<I',len(d))+d for tag,d,_ in chunks)
def overlaps(a,b):return all(a[0][k]<=b[1][k] and b[0][k]<=a[1][k] for k in (0,1))
manifest=[]
for tx,ty in ((28,27),(28,28),(29,27),(29,28)):
 name=f'netherstormbg_{tx}_{ty}.adt';source=args.source/name;raw=source.read_bytes();
 assert hashlib.sha256(raw).hexdigest()==recipe['sourceADTs'][name], 'Source ADT changed: '+name
 cs=chunks(raw);by={t:d for t,d,_ in cs if t!=b'KNCM'}
 selected=[]
 tilebox=([17066.6666667-(ty+1)*1600/3,17066.6666667-(tx+1)*1600/3],[17066.6666667-ty*1600/3,17066.6666667-tx*1600/3])
 for r in ramps:
  box=r['bounds']
  if overlaps(box,tilebox):selected.append((r,box))
 if not selected:continue
 ids=struct.unpack('<'+'I'*(len(by[b'DIMM'])//4),by[b'DIMM']);mdx=by[b'XDMM'];end=ids[-1]+mdx[ids[-1]:].index(0)+1
 assert not any(mdx[end:]);del mdx[end:]
 assert len(mdx.split(b'\0'))-1==len(ids), 'native extractor indexes string order'
 newname=len(ids);by[b'DIMM']+=struct.pack('<I',len(mdx));mdx+=filename
 oldcount=len(by[b'FDDM'])//36;existing={u32(by[b'FDDM'],i*36+4) for i in range(oldcount)}
 for r,_ in selected:
  assert r['id'] not in existing
  x,y,z=r['pos'];by[b'FDDM']+=struct.pack('<II6fHH',newname,r['id'],y,z,x,*r['rot'],round(r['scale']*1024),0)
 references={r['id']:0 for r,_ in selected}
 for c in cs:
  if c[0]!=b'KNCM':continue
  d=c[1];x,y,z=struct.unpack_from('<3f',d,104);box=([x-1600/48,y-1600/48],[x,y]);new=[]
  for index,(r,b) in enumerate(selected):
   if overlaps(box,b):new.append(oldcount+index);references[r['id']]+=1
  if not new:continue
  oldD,oldW=u32(d,16),u32(d,56);rf=u32(d,32)-8
  assert d[rf:rf+4]==b'FRCM' and u32(d,rf+4)==4*(oldD+oldW)
  insertion=rf+8+4*oldD;delta=4*len(new)
  d[insertion:insertion]=struct.pack('<'+'I'*len(new),*new)
  struct.pack_into('<I',d,16,oldD+len(new));struct.pack_into('<I',d,rf+4,4*(oldD+oldW)+delta)
  for off in (20,24,28,32,36,44,88,96,116):
   value=u32(d,off)
   if value and value-8>=insertion:struct.pack_into('<I',d,off,value+delta)
 assert all(references.values()),references
 # Rebase file-level offsets; relative offsets inside untouched chunks stay valid.
 at=0;newOffsets={}
 for tag,d,old in cs:newOffsets[old]=at;at+=8+len(d)
 hdr=by[b'RDHM'];oldhdr=next(old+8 for tag,d,old in cs if tag==b'RDHM');newhdr=newOffsets[oldhdr-8]+8
 for off in range(4,44,4):
  value=u32(hdr,off)
  if value:struct.pack_into('<I',hdr,off,newOffsets[value+oldhdr]-newhdr)
 index=by[b'NICM'];sizeByOld={old:len(d)+8 for tag,d,old in cs if tag==b'KNCM'}
 for off in range(0,len(index),16):
  old=u32(index,off)
  if old:struct.pack_into('<II',index,off,newOffsets[old],sizeByOld[old])
 output=serialize(cs);check=chunks(output);valid={old:tag for tag,d,old in check}
 for off in range(4,44,4):
  value=u32(hdr,off)
  if value:assert value+newhdr in valid
 for tag,d,at in check:
  if tag!=b'KNCM':continue
  for field,expected in ((20,b'TVCM'),(24,b'RNCM'),(28,b'YLCM'),(32,b'FRCM'),(36,b'LACM'),(44,b'HSCM'),(88,b'ESCM'),(96,b'QLCM')):
   value=u32(d,field)
   if value:assert d[value-8:value-4]==expected,(name,field,value,d[value-8:value-4])
  rf=u32(d,32)-8;nd,nw=u32(d,16),u32(d,56)
  assert u32(d,rf+4)==4*(nd+nw)
  refs=struct.unpack_from('<'+'I'*nd,d,rf+8)
  assert all(v<oldcount+len(selected) for v in refs)
 dest=args.output/'World/Maps/netherstormbg'/name;dest.parent.mkdir(parents=True,exist_ok=True);dest.write_bytes(output)
 manifest.append({'file':str(dest),'before':hashlib.sha256(raw).hexdigest(),'after':hashlib.sha256(output).hexdigest(),'ramps':list(references),'chunkReferences':references})
 print(name,'ramps',len(selected),'references',references,'bytes added',len(output)-len(raw))
(args.output/'bridge-patch-manifest.json').write_text(json.dumps(manifest,indent=2))
