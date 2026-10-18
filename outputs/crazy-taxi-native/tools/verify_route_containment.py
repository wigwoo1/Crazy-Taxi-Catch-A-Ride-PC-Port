"""Independent scalar transcription of audited 080051cc; diagnostic, no ROM execution."""
import csv,json
from pathlib import Path
ROOT=Path(__file__).resolve().parents[1]
def signed(n):
 n&=0xffffffff
 return n-0x100000000 if n&0x80000000 else n
def edge(p,a,b):
 # Original add/sub/mul/MLAS low-word and ASR1 midpoint.
 x=signed(p[0]-(signed(a[0]+b[0])>>1))
 z=signed(p[1]-(signed(a[1]+b[1])>>1))
 return signed(signed(x*signed(a[1]-b[1]))+signed(z*signed(b[0]-a[0])))
def contains(p,v):
 second=edge(p,v[0],v[2])<0 if len(v)==4 else False
 triangle=(v[0],v[2],v[3]) if second else v[:3]
 values=[edge(p,triangle[i],triangle[(i+1)%3]) for i in range(3)]
 return sum(n>0 for n in values) in (0,3),second,values
if __name__=='__main__':
 rows=list(csv.DictReader((ROOT/'reports/route-query.csv').open()))
 results=[]
 for q in csv.DictReader((ROOT/'reports/route-query-obstacles.csv').open()):
  if q['waypoint'] not in ('11','12'):continue
  v=[(int(q[f'v{i}x']),int(q[f'v{i}z'])) for i in range(int(q['edge_count']))]
  tick=int(q['tick']);points={'attempted':(int(q['query_x']),int(q['query_z'])),'before':(int(rows[tick-1]['x']),int(rows[tick-1]['z'])),'pushed_out':(int(rows[tick]['x']),int(rows[tick]['z']))}
  evaluated={key:dict(zip(('inside','second_triangle','signed_edges'),contains(value,v))) for key,value in points.items()}
  assert evaluated['attempted']['inside'],(tick,evaluated)
  results.append({'tick':tick,'polygon':int(q['polygon']),'points':points,'containment':evaluated})
 assert len(results)==6
 (ROOT/'reports/route-containment-check.json').write_text(json.dumps(results,indent=2))
 print('All six attempted points inside source wall triangles; before/pushout evidence retained.')
 for row in results:print(row['tick'],{k:v['inside'] for k,v in row['containment'].items()})
