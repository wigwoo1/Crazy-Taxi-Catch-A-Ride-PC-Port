"""Bounded offline original ARM angle owners; never linked into the native game."""
import json
from pathlib import Path

def execute_angle_owner(rom,registers,entry=0x531c):
    regs=list(registers);initial=regs.copy();memory={};writes=[];pc=entry;flags=(False,False,False,False);steps=0
    def signed(v):return v-0x100000000 if v&0x80000000 else v
    def compare(a,b):
        result=(a-b)&0xffffffff
        return (bool(result&0x80000000),result==0,a>=b,bool(((a^b)&(a^result))&0x80000000))
    for _ in range(100):
        assert (0x52b4<=pc<0x531c or 0x531c<=pc<=0x535c) and pc%4==0,hex(pc)
        w=int.from_bytes(rom[pc:pc+4],'little');steps+=1;n,z,c,v=flags;cond=w>>28
        assert cond in (14,10,11,12),hex(w)
        if not {14:True,10:n==v,11:n!=v,12:not z and n==v}[cond]:pc+=4;continue
        if w==0xe92d4070:
            regs[13]-=16
            for j,k in enumerate((4,5,6,14)):memory[regs[13]+j*4]=regs[k];writes.append((regs[13]+j*4,regs[k]))
        elif w==0xe8bd4070:
            for j,k in enumerate((4,5,6,14)):regs[k]=memory[regs[13]+j*4]
            regs[13]+=16
        elif w&0x0ffffff0==0x012fff10:
            assert w&15==14
            if pc==0x535c or entry==0x52b4:
                assert regs[13]==initial[13] and regs[4:12]==initial[4:12]
                return dict(registers=regs,flags=flags,writes=writes,instructions=steps)
            pc=regs[14];continue
        elif w&0x0e000000==0x0a000000:
            offset=w&0xffffff;offset=offset-0x1000000 if offset&0x800000 else offset;dest=pc+8+offset*4
            if w&(1<<24):assert dest==0x52b4;regs[14]=pc+4
            pc=dest;continue
        elif w&0x0c000000==0x04000000:
            assert w==0xe51f20f4;address=pc+8-0xf4;regs[2]=int.from_bytes(rom[address:address+4],'little')
        else:
            assert w&0x0c000000==0
            rn=(w>>16)&15;rd=(w>>12)&15;op=(w>>21)&15
            if w&(1<<25):
                operand=w&255;rotation=((w>>8)&15)*2
                if rotation:operand=((operand>>rotation)|(operand<<(32-rotation)))&0xffffffff
            else:assert w&0xff0==0;operand=regs[w&15]
            if op==10:assert w&(1<<20);flags=compare(regs[rn],operand)
            else:
                assert not w&(1<<20)
                if op==0:value=regs[rn]&operand
                elif op==2:value=regs[rn]-operand
                elif op==3:value=operand-regs[rn]
                elif op==4:value=regs[rn]+operand
                elif op==13:value=operand
                else:raise AssertionError((hex(pc),hex(w)))
                regs[rd]=value&0xffffffff
        pc+=4
    raise AssertionError('original angle owner instruction limit')
