import argparse, hashlib, json, pathlib, struct

ROM_SHA = '39996ec9c561f68e16e5435b781bb7c73f4e0123c13002c109f8150ab6e89de5'

def sha(path):
    return hashlib.sha256(path.read_bytes()).hexdigest()

def call_target(rom, address):
    off = address - 0x08000000
    first, second = struct.unpack_from('<HH', rom, off)
    assert first & 0xf800 == 0xf000 and second & 0xf800 == 0xf800
    rel = ((first & 0x7ff) << 12) | ((second & 0x7ff) << 1)
    if rel & 0x400000:
        rel -= 0x800000
    return 0x08000000 + off + 4 + rel

def main():
    ap = argparse.ArgumentParser(description='Audit source branch path for the tick-632 contextual-voice busy query.')
    ap.add_argument('--project-root', required=True, type=pathlib.Path)
    ap.add_argument('--rom', required=True, type=pathlib.Path)
    ap.add_argument('--output', required=True, type=pathlib.Path)
    a = ap.parse_args()
    root = a.project_root
    reportdir = root / 'reports' / 'original-core-reference'
    rom = a.rom.read_bytes()
    assert hashlib.sha256(rom).hexdigest() == ROM_SHA
    rng_path = reportdir / 'route31rngprobe627.rng-callers.json'
    voice_path = reportdir / 'route31voicewriteprobe627.voice-reset-writes.json'
    frontier_path = reportdir / 'route31-source-traffic-replay-frontier.json'
    checkpoint_path = reportdir / 'route31-audio-context-handle-checkpoint.json'
    rng = json.loads(rng_path.read_text())
    voice = json.loads(voice_path.read_text())
    frontier = json.loads(frontier_path.read_text())
    checkpoint = json.loads(checkpoint_path.read_text())
    assert rng['instrumentation']['source_rom_sha256'] == ROM_SHA
    assert voice['instrumentation']['source_rom_sha256'] == ROM_SHA
    # Original 0x0801134A call checks the low byte of the busy helper's return.
    assert call_target(rom, 0x0801134a) == 0x080277a4
    busy_branch = rom[0x0801134e - 0x08000000:0x08011356 - 0x08000000]
    assert busy_branch.hex() == '0006002800d07be0'
    # The observed follow-up RNG callsite is part of the zero-busy branch.
    assert call_target(rom, 0x080113ea) == 0x0801e72c
    rng_events = [e for e in rng['call_events'] if e['call_site'].lower() == '0x80113ea' and e['caller_r5'].lower() == '0x20025d6']
    writes = [e for e in voice['call_events'] if e['target'].lower() == '0x20025d6' and e['source_counter'] == 4828 and e['before_value'] == 8 and e['write_value'] == 255]
    assert len(rng_events) == 1 and len(writes) == 1
    assert rng_events[0]['rng_before'] == 478488862
    assert rng_events[0].get('tick') == 115 and writes[0]['observed_after_core_tick'] == 115
    src = frontier['source_events']['tick_632_voice_reset_frontier']
    assert src['rng_callsite'].lower() == '0x80113ea' and src['source_counter'] == 4828
    assert src['voice_before'] == 8 and src['voice_after'] == 255
    snaps = {x['source_vblank']: x for x in checkpoint['checks']}
    assert snaps[631]['passenger_voice_25d6'] == 8 and snaps[631]['context_handle_2604'] == '0x02007594'
    assert snaps[632]['passenger_voice_25d6'] == 255 and snaps[632]['context_handle_2604'] == '0x00000000'
    direct_path = reportdir / 'route31busyreturnlive.audio-busy-return-trace.json'
    direct = json.loads(direct_path.read_text())
    assert direct['instrumentation']['source_rom_sha256'] == ROM_SHA
    direct_events = direct['call_events']
    assert direct['record_count'] == len(direct_events) == 17
    assert all(e['busy_helper_return'] == 1 for e in direct_events[:-1])
    assert direct_events[-1] == {'observed_after_core_tick': 115, 'source_counter': 4828, 'busy_helper_return': 0}
    source_tick_by_local = {110: 627, 114: 631, 115: 632}
    snapshot_checks=[]
    mask_lo,mask_hi=0x3f000,0x3f284
    for local_tick,source_tick in source_tick_by_local.items():
        baseline_path=reportdir/f'route31audioctxstate.ewram-vblank-{local_tick:05d}.bin'
        direct_snapshot_path=reportdir/f'route31busyreturnlive.ewram-vblank-{local_tick:05d}.bin'
        base=baseline_path.read_bytes(); observed=direct_snapshot_path.read_bytes()
        assert len(base)==len(observed)==0x40000
        expected_base_hash=snaps[source_tick]['snapshot_sha256']
        assert sha(baseline_path)==expected_base_hash
        diffs=[i for i,(x,y) in enumerate(zip(base,observed)) if x!=y and not mask_lo<=i<mask_hi]
        assert not diffs, (local_tick,diffs[:12])
        snapshot_checks.append({'capture_tick':local_tick,'source_vblank':source_tick,'baseline_sha256':sha(baseline_path),'instrumented_sha256':sha(direct_snapshot_path),'non_logger_byte_differences':len(diffs),'voice_25d6':observed[0x25d6],'context_handle_2604':f'0x{struct.unpack_from("<I",observed,0x2604)[0]:08X}','source_counter_3520':struct.unpack_from('<I',observed,0x3520)[0]})
    route=json.loads((reportdir/'route31busyreturnlive.route-follow.json').read_text())
    assert route['status']=='INCOMPLETE_SOURCE_ROUTE31_CAPTURE' and route['completed_count']==0 and route['fare']==[0,0]
    report = {
        'status': 'PASS_DIRECT_BUSY_RETURN_AND_CAPTURE_EQUIVALENCE',
        'rom_sha256': ROM_SHA,
        'evidence_sha256': {p.name: sha(p) for p in (rng_path, voice_path, frontier_path, checkpoint_path, direct_path)},
        'direct_busy_return_trace': {'record_count':len(direct_events),'busy_values_before_completion':[e['busy_helper_return'] for e in direct_events[:-1]],'completion_event':direct_events[-1],'instrumented_rom_sha256':direct['instrumentation']['instrumented_rom_sha256']},
        'capture_equivalence': {'reserved_logger_range_ewram':['0x0203F000','0x0203F284'],'snapshots':snapshot_checks,'route_capture_status':route['status'],'completed_count':route['completed_count'],'fare':route['fare']},
        'original_control_flow': {
            'busy_call_address': '0x0801134A', 'busy_target': '0x080277A4',
            'post_call_bytes': busy_branch.hex(),
            'interpretation': 'The original code tests the low byte of the busy helper result; zero branches to 0x08011356 and proceeds to follow-up selection, while nonzero skips to 0x0801144E.' ,
            'followup_rng_call_address': '0x080113EA', 'rng_target': '0x0801E72C'
        },
        'dynamic_observation': {
            'rng_call': rng_events[0], 'voice_reset_write': writes[0],
            'shared_source_counter': 4828, 'observed_core_tick': 115,
            'voice_handle_before_after_source_vblank': [snaps[631]['context_handle_2604'], snaps[632]['context_handle_2604']],
            'voice_before_after_source_vblank': [snaps[631]['passenger_voice_25d6'], snaps[632]['passenger_voice_25d6']]
        },
        'conclusion': 'The original busy helper returned 1 for 16 observed calls and returned 0 at source counter4828/VBlank632. This directly confirms the completion transition and matches the original follow-up RNG/voice-reset path. EWRAM snapshots are byte-identical to the uninstrumented capture outside the reserved logger page. The capture was a scripted route probe that still completed zero deliveries and fare; it is not a playable-port result. Handle-to-channel/asset mapping and controller import/dispatch/commit cadence remain unresolved.',
        'scope': 'Direct source-ROM helper return logging with original displaced instruction replay, cross-checked against independently captured RNG/store events and matched uninstrumented EWRAM. Instrumentation only patches the temporary in-memory libretro ROM copy; supplied ROM and production runtime remain unchanged. No audible-output or playable-port claim.'
    }
    a.output.parent.mkdir(parents=True, exist_ok=True)
    a.output.write_text(json.dumps(report, indent=2) + '\n')
    print(json.dumps({'status': report['status'], 'busy_values':[e['busy_helper_return'] for e in direct_events], 'source_completion_counter':4828, 'capture_equivalent_outside_logger':True, 'next':'map handle 0x02007594 to the actual playing sound and trace native controller commit cadence'}, indent=2))

if __name__ == '__main__':
    main()

