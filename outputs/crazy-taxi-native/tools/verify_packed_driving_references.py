import pathlib,json,struct,hashlib,sys
root=pathlib.Path('outputs/crazy-taxi-native');out=root/'reports/original-core-reference';summaries=[]
prefix=sys.argv[1] if len(sys.argv)>1 else 'packeddriving'
for side in ('left','right'):
 label=prefix+side
 lite=json.loads((out/(label+'.actor-trace.json')).read_text());old=json.loads((out/('driving'+side+'.actor-trace.json')).read_text());assert lite==old
 path=out/(label+'.packed-driving-trace.json');pack=json.loads(path.read_text())['records'];assert len(pack)==361
 baseline=pack[0];previous=baseline['previous'];events=[]
 for index,(state,row) in enumerate(zip(pack[1:],lite)):
  actor=bytes.fromhex(state['actor']);assert len(actor)==0xea0 and list(struct.unpack_from('<iii',actor))==row['position']
  assert struct.unpack_from('<i',actor,0x1c)[0]==row['heading'] and struct.unpack_from('<i',actor,0x544)[0]==row['speed']
  if state['previous']!=previous:
   elapsed=(state['previous']-previous)&0xffffffff;assert 1<=elapsed<=60
   events.append({'source_tick':index,'elapsed_counter':elapsed,'prior_sample_frame_delta':pack[index]['frame_delta'],'post_frame_delta':state['frame_delta'],'raw_held_keys':1023^row['keyinput'],'packed_record_index':index+1,'expected_position':row['position'],'expected_heading':row['heading'],'expected_speed':row['speed']});previous=state['previous']
 manifest={'status':'INDEPENDENT_DIAGNOSTIC_REPLAY_REFERENCE','baseline':baseline,'packed_capture':str(path),'events':events,'scope':'Recorded presentation boundaries and complete actor/input/command snapshots. Prior/post sample time fields retained for caller-order audit; raw keys do not replace recovered input history. Native packed adapter and exact routine boundary ordering still required. No native trajectory equivalence claim.'}
 (root/f'reports/original-{side}-driving-replay-reference.json').write_text(json.dumps(manifest,indent=2)+'\n')
 summaries.append({'side':side,'samples':len(lite),'packed_states':len(pack),'boundary_events':len(events),'lite_matches_prior_capture':True,'packed_sha256':hashlib.sha256(path.read_bytes()).hexdigest()})
report={'status':'PASS_INDEPENDENT_PACKED_DRIVING_REFERENCES','captures':summaries,'scope':'720 independent original samples exactly repeat earlier left/right references; packed actor pose/heading/speed validated and cold baseline compared separately. Offline emulator used solely for reference capture, never native runtime.'}
(root/'reports/packed-driving-reference-verification.json').write_text(json.dumps(report,indent=2)+'\n');print(json.dumps(report,indent=2))
