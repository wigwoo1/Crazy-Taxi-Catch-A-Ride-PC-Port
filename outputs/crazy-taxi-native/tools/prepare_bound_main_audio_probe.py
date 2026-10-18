from pathlib import Path
import hashlib,json
root=Path('outputs/crazy-taxi-native');main=root/'src/scene_viewer.c';s=main.read_text();c=(root/'tests/scene_viewer_bound_audio_cleanup_probe.c').read_text()
def braced(text,marker):
 a=text.index(marker);j=text.index('{',a)+1;depth=1
 while depth:
  if text[j]=='{':depth+=1
  elif text[j]=='}':depth-=1
  j+=1
 return text[a:j]
helper=c[c.index('static int bound_audio_capture_active;'):c.index('static void native_sound_controller_step')]
pos=s.index('static void native_sound_controller_step');s=s[:pos]+helper+s[pos:]
s=s.replace('static void reset_drive_observed(TaxiPassengerInitObserver observer,void *context);','static void bound_audio_capture_state(const TaxiGameAudioController *v);\nstatic void reset_drive_observed(TaxiPassengerInitObserver observer,void *context);')
needle='        taxi_game_audio_controller_reset(&live_sound_state,&live_sound_auxiliary_timer);'
s=s.replace(needle,'''        int capture_reset=bound_retry_check&&audio_live&&getenv("TAXI_BOUND_AUDIO_RETRY_CHECK")!=NULL;
        if(capture_reset){printf("BOUND AUDIO RESET:");bound_audio_capture_state(&live_sound_state);printf(" %d",live_sound_auxiliary_timer);}
'''+needle+'''
        if(capture_reset){bound_audio_capture_state(&live_sound_state);printf(" %d\\n",live_sound_auxiliary_timer);}
''')
a=c.index('            unsigned count;\n            bound_audio_capture_active');b=c.index('            taxi_game_audio_frame_commit',a);captured=c[a:b]
a=s.index('            unsigned count;',s.index('static void native_sound_controller_step'));b=s.index('            taxi_game_audio_frame_commit',a);s=s[:a]+captured+s[b:]
s=s.replace('if(audio_preview&&((!drive_mode&&!startup_mode)||image_path))','if(audio_preview&&((!drive_mode&&!startup_mode)||(image_path&&!((bound_failure_check||bound_retry_check)&&recovered_game_audio&&audio_output_disabled))))')
assets=braced(c,'        if((bound_failure_check||bound_retry_check)&&image_path&&recovered_game_audio&&audio_output_disabled)')
needle='        load_drive_graphics();\n';assert s.count(needle)==1;s=s.replace(needle,needle+assets+'\n')
needle=' drive_tick_inner(pad,scripted);\n';assert s.count(needle)==1;s=s.replace(needle,needle+' if((bound_failure_check||bound_retry_check)&&audio_output_disabled&&recovered_game_audio&&audio_live&&!native_audio_capture)audio_frame();\n')
(root/'tests/scene_viewer_bound_main_audio_probe.c').write_text(s)
b=(root/'tools/rebuild_bound_audio_cleanup_probe.ps1').read_text().replace('tests/scene_viewer_bound_audio_cleanup_probe.c','tests/scene_viewer_bound_main_audio_probe.c');(root/'tools/rebuild_bound_main_audio_probe.ps1').write_text(b)
(root/'reports/bound-main-audio-overlay-source.json').write_text(json.dumps(dict(main_source_sha256=hashlib.sha256(main.read_bytes()).hexdigest(),scope='Main source plus observational controller/reset traces and strictly scoped logicalaudio snapshot asset/mixer harness; no replacement driving/camera/effects/menu/reset functional bodies.'),indent=2))
