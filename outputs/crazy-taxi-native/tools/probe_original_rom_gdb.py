"""Bounded independent ROM diagnostic; mGBA is never a native-port dependency."""
import pathlib,subprocess,os,time,socket,json,hashlib,sys
root=pathlib.Path('outputs/crazy-taxi-native').resolve();qt='--qt' in sys.argv;folder=root/('reports/original-rom-diagnostic-qt' if qt else 'reports/original-rom-diagnostic');folder.mkdir(exist_ok=True)
source=pathlib.Path('Crazy Taxi - Catch a Ride (USA).gba')
rom=source.read_bytes();assert hashlib.sha256(rom).hexdigest()=='39996ec9c561f68e16e5435b781bb7c73f4e0123c13002c109f8150ab6e89de5'
copy=folder/'reference.gba'
if not copy.exists():copy.write_bytes(rom)
assert copy.read_bytes()==rom
env=os.environ.copy();env.update(SDL_VIDEODRIVER='dummy',SDL_AUDIODRIVER='dummy',QT_QPA_PLATFORM='offscreen')
startup=subprocess.STARTUPINFO();startup.dwFlags|=subprocess.STARTF_USESHOWWINDOW;startup.wShowWindow=0
report=dict(status='UNAVAILABLE',scope='Independent original ROM debugger connectivity only, not original framebuffer/gameplay proof; copied ROM and isolated save directory.')
with (folder/'stdout.log').open('wb') as out,(folder/'stderr.log').open('wb') as err:
 p=subprocess.Popen([('./' if qt else './'),'-g','-C','videoRenderer=0',str(copy)],cwd=folder,env=env,stdout=out,stderr=err,startupinfo=startup)
 try:
  connection=None
  for attempt in range(20):
   if p.poll() is not None:break
   try:connection=socket.create_connection(('127.0.0.1',2345),timeout=.2);break
   except OSError as e:report['last_connection_error']=str(e);time.sleep(.1)
  if connection:
   connection.settimeout(3)
   def packet(command):
    data=command.encode();connection.sendall(b'$'+data+b'#'+f'{sum(data)%256:02x}'.encode())
    while connection.recv(1)!=b'$':pass
    result=bytearray()
    while True:
     b=connection.recv(1)
     if b==b'#':break
     if not b:raise RuntimeError('GDB disconnected')
     result.extend(b)
    checksum=connection.recv(2);assert int(checksum,16)==sum(result)%256
    connection.sendall(b'+');return result.decode()
   report['stop_reply']=packet('?');report['registers']=packet('g');report['rom_header']=packet('m8000000,40')
   assert bytes.fromhex(report['rom_header'])==rom[:64]
   report['status']='PASS';connection.close()
  else:
   report['process_exit']=p.poll();report['owned_process_id']=p.pid
   listing=subprocess.run(['netstat','-ano','-p','tcp'],capture_output=True,text=True,timeout=5).stdout
   report['owned_tcp_rows']=[line.strip() for line in listing.splitlines() if line.split() and line.split()[-1]==str(p.pid)]
 except Exception as e:report['error']=str(e)
 finally:
  if p.poll() is None:p.terminate()
  p.wait(timeout=5)
report['stderr']=(folder/'stderr.log').read_text(errors='replace')[:3000]
report['source_rom_preserved']=source.read_bytes()==rom
(folder/'connection-result.json').write_text(json.dumps(report,indent=2));print(json.dumps(report))



