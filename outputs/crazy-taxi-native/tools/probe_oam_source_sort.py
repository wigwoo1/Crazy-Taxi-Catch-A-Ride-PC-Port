"""Scalar source280c4 transcription probe, not yet a production renderer routine."""
import json
from pathlib import Path
def sort_source(items,lo=0,hi=None):
 if hi is None:hi=len(items)-1
 if lo>=hi:return
 if lo==hi-1:
  a,b=items[lo],items[hi]
  if a['z']>b['z'] or (a['z']==b['z'] and a['x']<b['x']):items[lo],items[hi]=b,a
  return
 pivot=items[(lo+hi)//2].copy();items[(lo+hi)//2]=items[hi];items[hi]=pivot.copy()
 left,right=lo,hi
 while left<right:
  while left<right and (items[left]['z']<pivot['z'] or (items[left]['z']==pivot['z'] and items[left]['x']<pivot['x'])):left+=1
  while left<right and (items[right]['z']>pivot['z'] or (items[right]['z']==pivot['z'] and items[right]['x']>=pivot['x'])):right-=1
  if left<right:items[left],items[right]=items[right],items[left]
 items[hi]=items[right];items[right]=pivot
 sort_source(items,lo,left-1);sort_source(items,right+1,hi)
if __name__=='__main__':
 cases=[]
 for xs in ([1,2],[1,2,3],[3,2,1],[1,1,1],[4,3,2,1],[0,-32768,32767]):
  rows=[{'x':x,'z':0,'id':i} for i,x in enumerate(xs)];sort_source(rows)
  cases.append({'input_x':xs,'output':rows})
 out=Path(__file__).resolve().parents[1]/'reports/oam-source-sort-tie-probes.json'
 out.write_text(json.dumps(cases,indent=2));print(json.dumps(cases))
