"""Independent integer reference for source5554c on original extracted paths."""
import pathlib,struct
root=pathlib.Path('outputs/crazy-taxi-native')
def s(v):return (v+2**31)%2**32-2**31
def angle(x,z):
    quadrant=(x<0)+2*(z<0);x=s(-x) if x<0 else x or 1;z=s(-z) if z<0 else z or 1
    numerator=s(min(x,z)<<16);divisor=max(x,z)
    quotient=abs(numerator)//abs(divisor)*(-1 if (numerator<0)!=(divisor<0) else 1)
    a=quotient//128 if x>z else 1024-quotient//128
    return [a,2048-a,-a,2048+a][quadrant]
def difference(a,b):
    a&=4095;b&=4095;d=b-a
    return d-4096 if d>=2048 else d+4096 if d<=-2048 else d
for city in ('city1','city2'):
    raw=(root/f'assets/ai/{city}-path.cta').read_bytes()
    points=list(struct.iter_unpack('<IiiI',raw[8:]))
    def target(q):
        q&=65535
        for a,b in zip(points,points[1:]):
            if a[0]<=q<=b[0]:
                ratio=((q-a[0])*4096)//(b[0]-a[0])
                return [s(a[k]+s((b[k]-a[k])*ratio)//4096) for k in (1,2)]
        raise ValueError(q)
    output=bytearray()
    for phase in range(0,65536,17):
        for parity in range(2):
            x,z=3,4;heading=0;tx,tz=target(phase+300)
            error=difference(0,angle(s(tx-x),s(tz-z))-1024)
            command=240|(2 if error < -50 else 1 if error>50 else 0)
            moved=abs(difference(0,phase>>4))>384
            if moved:
                q=4096 if parity else 61440;x,z=target(q);tx,tz=target(q+300)
                heading=s((angle(s(tx-x),s(tz-z))-1024)<<12)
            output+=struct.pack('<IiiiiiI',int(moved),x,z,heading,heading,command,parity+int(moved))
    (root/f'reports/{city}-ai-controller-reference.bin').write_bytes(output)
    print(city,len(output)//28,'controller cases')
