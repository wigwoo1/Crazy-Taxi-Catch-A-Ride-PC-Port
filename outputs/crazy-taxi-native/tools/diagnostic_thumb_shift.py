def thumb_shift_result(kind,a,count,carry,register):
 a&=0xffffffff;count=(count&255) if register else count
 if not register and kind in ('lsrs','asrs') and count==0:count=32
 if count==0:return a,bool(carry)
 if kind=='lsls':
  return ((a<<count)&0xffffffff if count<32 else 0),bool((a>>(32-count))&1) if count<=32 else False
 if kind=='lsrs':
  return (a>>count if count<32 else 0),bool((a>>(count-1))&1) if count<=32 else False
 assert kind=='asrs'
 sign=bool(a&0x80000000)
 if count>=32:return (0xffffffff if sign else 0),sign
 signed=a-0x100000000 if sign else a
 return (signed>>count)&0xffffffff,bool((a>>(count-1))&1)
