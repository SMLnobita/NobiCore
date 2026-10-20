"""Add a locally verifiable ad-hoc signature container, not a distribution identity."""
from pathlib import Path
import struct,hashlib,json
p=Path(__file__).resolve().parent/'Clone6-khtn';b=bytearray((p.parent/'Clone6-khtn-raw').read_bytes());n,cmdsize=struct.unpack_from('<II',b,16);pos=32;linkedit=None;first_section=len(b);text_size=0
for i in range(n):
 c,s=struct.unpack_from('<II',b,pos)
 assert c!=0x1d,'Already signed'
 if c==0x19:
  name=bytes(b[pos+8:pos+24]).split(b'\0')[0]
  if name==b'__LINKEDIT':linkedit=pos
  if name in [b'__TEXT',b'__DATA_CONST',b'__DATA']:
   vm=struct.unpack_from('<Q',b,pos+32)[0];fo=struct.unpack_from('<Q',b,pos+40)[0];fs=struct.unpack_from('<Q',b,pos+48)[0]
   av=(vm+0x3fff)&~0x3fff;af=(fs+0x3fff)&~0x3fff
   assert not any(b[fo+fs:fo+af]),'Expected zero page padding'
   struct.pack_into('<Q',b,pos+32,av);struct.pack_into('<Q',b,pos+48,af)
   if name==b'__TEXT':text_size=af
  for j in range(struct.unpack_from('<I',b,pos+64)[0]):
   q=pos+72+j*80;offset=struct.unpack_from('<I',b,q+48)[0]
   if offset:first_section=min(first_section,offset)
 pos+=s
assert linkedit is not None
assert 32+cmdsize+16<=first_section,'Need load-command header padding'
assert not any(b[32+cmdsize:32+cmdsize+16])
limit=(len(b)+15)&~15;reserve=0x4000;b+=bytes(limit-len(b))
struct.pack_into('<IIII',b,32+cmdsize,0x1d,16,limit,reserve);struct.pack_into('<II',b,16,n+1,cmdsize+16)
fileoff=struct.unpack_from('<Q',b,linkedit+40)[0];newsize=limit+reserve-fileoff
struct.pack_into('<Q',b,linkedit+48,newsize);struct.pack_into('<Q',b,linkedit+32,(newsize+0x3fff)&~0x3fff)
ident=b'local.clone6.framework\0';hashoff=(88+len(ident)+7)&~7;slots=(limit+4095)//4096;cdsize=hashoff+slots*32
cd=bytearray(cdsize)
struct.pack_into('>9I4B4I4Q',cd,0,0xfade0c02,cdsize,0x20400,2,hashoff,88,0,slots,limit,32,2,0,12,0,0,0,0,0,0,text_size,0)
cd[88:88+len(ident)]=ident
for i in range(slots):cd[hashoff+i*32:hashoff+(i+1)*32]=hashlib.sha256(b[i*4096:min((i+1)*4096,limit)]).digest()
blob=struct.pack('>5I',0xfade0cc0,20+len(cd),1,0,20)+cd
assert len(blob)<reserve;b+=blob+bytes(reserve-len(blob));p.write_bytes(b)
# Verify every page against the final serialized file, independently of construction buffers.
check=p.read_bytes();cs=check[limit:limit+reserve];magic,length,count=struct.unpack_from('>III',cs);assert magic==0xfade0cc0 and count==1
_,offset=struct.unpack_from('>II',cs,12);c=cs[offset:];assert struct.unpack_from('>I',c,0)[0]==0xfade0c02
hpos=struct.unpack_from('>I',c,16)[0];pages=struct.unpack_from('>I',c,28)[0];code_limit=struct.unpack_from('>I',c,32)[0]
for i in range(pages):assert c[hpos+i*32:hpos+(i+1)*32]==hashlib.sha256(check[i*4096:min((i+1)*4096,code_limit)]).digest()
report={'module':str(p),'sha256':hashlib.sha256(check).hexdigest(),'signature_offset':limit,'signature_reserved_bytes':reserve,'code_directory_version':'0x20400','hash':'SHA256','verified_pages':pages,'cms_distribution_signature':False,'notes':'16KiB normalized mapped segments, dynamic executable range, ad-hoc container only. Complete certificate re-signing required.'}
p.with_suffix('.signature.json').write_text(json.dumps(report,indent=2));print(json.dumps(report,indent=2))
